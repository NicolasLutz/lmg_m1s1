#include "main.h"



static const float fCameraZInit = 4.f;


//====================================================================================================================================
TPGLWindow::TPGLWindow()
    : m_vObjectTranslate    (0)
    , m_vObjectEulerAngles  (0)
    , m_mtxObjectWorld      (0)
    , m_mtxCameraView       (0)
    , m_mtxCameraProjection (0)
    , m_iUniformWorld       (0)
    , m_iUniformView        (0)
    , m_iUniformProjection  (0)
    , m_iTexture            (0)
    , m_iTexture2           (0)
    , m_iUniformTextureUnit (0)
    , m_iUniformTextureUnit2(0)
    , m_bAlphaBlend         ( false )
    , m_iFBO                ( 0 ) //--------------------------------------
    , m_iColorRenderTexture ( 0 )
    , m_iZRenderBuffer      ( 0 )
    , m_GPUProgramPhongTextured    ()
    , m_MeshSphere          ()
    , m_MeshScreen          ()
{
    m_vCameraPosition   = glm::vec3(0,0,fCameraZInit);

    m_timer.start();

    // request the window to render in loop
    setAnimating(true);
}

//====================================================================================================================================
void TPGLWindow::createRenderTarget()
{
    // Creates the Frame Buffer Object - FBO - that will be used as a replacement for the default Frame Buffer -----------------
    glGenFramebuffers( 1, & m_iFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, m_iFBO );

    // Creates the Texture Object that will be used as color buffer ------------------------------------------------------------
    glGenTextures( 1 , & m_iColorRenderTexture );

    // "Bind" the created texture : all future texture functions will modify this texture
    glBindTexture( GL_TEXTURE_2D, m_iColorRenderTexture );

    const float retinaScale = devicePixelRatio();
    GLsizei W = width() * retinaScale;
    GLsizei H =  height() * retinaScale;

    // Put an empty image to OpenGL - see the last "0"
    glTexImage2D( GL_TEXTURE_2D, 0,GL_RGB, W,  H, 0,GL_RGB, GL_UNSIGNED_BYTE, 0 );

    // Required - but no need for a linea filtering
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );

    // Set "m_iColorRenderTexture" as the color_buffer_0
    glFramebufferTexture( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_iColorRenderTexture, 0 );


    // Creates the Render Buffer that will be used as Z buffer -----------------------------------------------------------------
    glGenRenderbuffers( 1, & m_iZRenderBuffer );
    glBindRenderbuffer( GL_RENDERBUFFER, m_iZRenderBuffer );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT, W, H );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_iZRenderBuffer );

    // Specify list of draw buffers --------------------------------------------------------------------------------------------
    GLenum aDrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers( 1, aDrawBuffers );

    glBindFramebuffer( GL_FRAMEBUFFER, 0 );

    // Make sure the FBO has been created properly
    TP_ASSERT( glCheckFramebufferStatus( GL_FRAMEBUFFER ) == GL_FRAMEBUFFER_COMPLETE, "FBO %d creation is incomplete!\n ", m_iFBO );
}

//====================================================================================================================================
void TPGLWindow::destroyRenderTarget()
{
    glDeleteRenderbuffers( 1, & m_iZRenderBuffer );
    glDeleteTextures( 1 , & m_iColorRenderTexture );
    glDeleteFramebuffers( 1, & m_iFBO );
}

//====================================================================================================================================
void TPGLWindow::bindRenderTarget()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_iFBO );
    const float retinaScale = devicePixelRatio();
    glViewport( 0, 0, width() * retinaScale, height() * retinaScale );
}

//====================================================================================================================================
void TPGLWindow::unbindRenderTarget()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    const float retinaScale = devicePixelRatio();
    glViewport( 0, 0, width() * retinaScale, height() * retinaScale );
}

//====================================================================================================================================
TPGLWindow::~TPGLWindow()
{
    m_GPUProgramPhongTextured.destroy();
    m_GPUProgramPostProcess.destroy();

    destroyMesh();
    destroyTextures();
    destroyRenderTarget();
}

//====================================================================================================================================
void TPGLWindow::getUniformLocations()
{
    //--------------------------------------------------------------------------------------------------------------------
    // precondition
    TP_ASSERT( 0 != m_GPUProgramPhongTextured.getID() , "m_GPUProgramNormal.getID() should not be 0 (here it is '%d') - Did you call createFromFiles() on it ?.\n", m_GPUProgramPhongTextured.getID() );
    //--------------------------------------------------------------------------------------------------------------------

    const GLuint iProgram = m_GPUProgramPhongTextured.getID();

    m_aiUniformLightProp[0]     = glGetUniformLocation( iProgram, "u_light.vAmbiant" );
    m_aiUniformLightProp[1]     = glGetUniformLocation( iProgram, "u_light.vDiffuse" );
    m_aiUniformLightProp[2]     = glGetUniformLocation( iProgram, "u_light.vSpecular" );

    m_aiUniformMaterialProp[0]  = glGetUniformLocation( iProgram, "u_material.vAmbiant" );
    m_aiUniformMaterialProp[1]  = glGetUniformLocation( iProgram, "u_material.vDiffuse" );
    m_aiUniformMaterialProp[2]  = glGetUniformLocation( iProgram, "u_material.vSpecular" );
    m_aiUniformMaterialProp[3]  = glGetUniformLocation( iProgram, "u_material.fSpecPower" );

    m_iUniformCameraPosition    = glGetUniformLocation( iProgram, "u_vCameraPositionWS" );
    m_iUniformLightPosition     = glGetUniformLocation( iProgram, "u_vLightPositionWS" );


    m_iUniformWorld             = glGetUniformLocation( iProgram, "u_mtxWorld" );
    m_iUniformView              = glGetUniformLocation( iProgram, "u_mtxView" );
    m_iUniformProjection        = glGetUniformLocation( iProgram, "u_mtxProjection" );

    // Retrieves the uniform location for the texture SAMPLER used in the GLSL Fragment Shader
    m_iUniformTextureUnit       = glGetUniformLocation( iProgram, "u_texDiffuse" );
    m_iUniformTextureUnit2      = glGetUniformLocation( iProgram, "u_texEmissive" );

    // Retrieves the uniform location for the texture SAMPLER used in the GLSL Fragment Shader
    m_iUniformSamplerPost       = glGetUniformLocation( m_GPUProgramPostProcess.getID(), "u_tex" );
}

//====================================================================================================================================
void TPGLWindow::sendUniformsToGPU()
{
    //
    glUniformMatrix4fv( m_iUniformWorld, 1, GL_FALSE, glm::value_ptr(m_mtxObjectWorld) );
    glUniformMatrix4fv( m_iUniformView, 1, GL_FALSE, glm::value_ptr(m_mtxCameraView) );
    glUniformMatrix4fv( m_iUniformProjection, 1, GL_FALSE, glm::value_ptr(m_mtxCameraProjection) );


    glUniform3fv( m_aiUniformLightProp[0], 1, glm::value_ptr(m_lightProp.m_vAmbiant) );
    glUniform3fv( m_aiUniformLightProp[1], 1, glm::value_ptr(m_lightProp.m_vDiffuse) );
    glUniform3fv( m_aiUniformLightProp[2], 1, glm::value_ptr(m_lightProp.m_vSpecular) );

    glUniform3fv( m_aiUniformMaterialProp[0], 1, glm::value_ptr(m_materialProp.m_vAmbiant) );
    glUniform3fv( m_aiUniformMaterialProp[1], 1, glm::value_ptr(m_materialProp.m_vDiffuse) );
    glUniform3fv( m_aiUniformMaterialProp[2], 1, glm::value_ptr(m_materialProp.m_vSpecular) );
    glUniform1f( m_aiUniformMaterialProp[3], m_materialProp.m_fSpecularPower );

    glUniform3fv( m_iUniformCameraPosition, 1, glm::value_ptr( m_vCameraPosition ) );
    glUniform3fv( m_iUniformLightPosition, 1, glm::value_ptr( m_vLightPosition ) );

    // Specify on which texture unit the uniform texture sampler must be bound
    glUniform1i( m_iUniformTextureUnit, 0 );

    // Specify on which texture unit the uniform texture sampler must be bound
    glUniform1i( m_iUniformTextureUnit2 , 1 );
}

//====================================================================================================================================
void TPGLWindow::createMesh()
{
    // Creates a Sphere Mesh - rendered as points !
    {

        uint     iParaCount  = 40;
        uint     iMeriCount  = 60;
        float   r           = 0.8f;

        // Create a sphere --------------------------------------------------------------------------------
        GLuint iVertexCount = iParaCount * iMeriCount;

        glm::vec3* afPositions  = new glm::vec3[ iVertexCount ];
        glm::vec2* afTexCoords  = new glm::vec2[ iVertexCount ];

        float a1 = ( 180.0 / ( iParaCount + 1 ) ) * M_PI / 180.0;
        float a2 = ( 360.0 / ( iMeriCount - 1 ) ) * M_PI / 180.0;

        // parallels ---------------------------------------------------------------------------------------
        uint k = 0;
        for( uint i = 0; i < iParaCount; ++i )
        {
            float fAngle    = - M_PI / 2.0 + a1 * ( i + 1 );
            float z         = r * sin( fAngle );
            float fRadius   = r * cos( fAngle );

            for( uint j = 0; j < iMeriCount; ++j )
            {
                afPositions[ k ] = glm::vec3( fRadius * cos( a2 * j ), fRadius * sin( a2 * j ), z );
                afTexCoords[ k ] = glm::vec2( float( j )/ iMeriCount, float( iParaCount - i ) / iParaCount );
                k++;
            }
        }

        // compute normals --------------------------------------------------------------------------------
        // on a 0 centered sphere : you just need to normalise the position!
        glm::vec3* afNormals = new glm::vec3[ iVertexCount ];

        for( uint i = 0; i < iVertexCount; ++i )
        {
            afNormals[ i ] = glm::normalize( afPositions[ i ] );
        }

        GLuint iElementsCount = ( iMeriCount - 1 ) * ( iParaCount - 1 ) * 2 * 3;  // for quads split in 2

        GLuint* auiIndices = new GLuint[ iElementsCount ];

        k=0;
        for( uint i = 0; i < ( iParaCount - 1 ); ++i )
        {
            for( uint j = 0; j < ( iMeriCount - 1 ); ++j )
            {
                auiIndices[ k++ ] = iMeriCount * i + j;
                auiIndices[ k++ ] = iMeriCount * i + ( j + 1 );
                auiIndices[ k++ ] = iMeriCount * ( i + 1 ) + ( j + 1 );
                auiIndices[ k++ ] = iMeriCount * ( i + 1 ) + ( j + 1 );
                auiIndices[ k++ ] = iMeriCount * ( i + 1 ) + j;
                auiIndices[ k++ ] = iMeriCount * i + j;
            }
        }

        m_MeshSphere.createFrom( GL_TRIANGLES, iVertexCount, afPositions, afNormals, afTexCoords, iElementsCount, auiIndices );

        delete [] afPositions;
        delete [] afNormals;
        delete [] afTexCoords;
        delete [] auiIndices;
    }


    // Creates the Screen Mesh - used to blit the FBO texture onto the screen
    {
        const GLfloat afPositions[] =   { -1.0f,  1.0f, 0.0f
                                        , -1.0f, -1.0f, 0.0f
                                        , 1.0f,  1.0f, 0.0f
                                        , 1.0f, -1.0f, 0.0f
                                        };

        const GLfloat afUVs[] = { 0.0f, 1.0f
                                , 0.0f, 0.0f
                                , 1.0f, 1.0f
                                , 1.0f, 0.0f
                                };

        m_MeshScreen.createFrom( GL_TRIANGLE_STRIP, 4, (glm::vec3*) afPositions, 0, (glm::vec2*) afUVs, 0, 0 );
    }
}

//====================================================================================================================================
void TPGLWindow::destroyMesh()
{
    m_MeshScreen.destroy();

    m_MeshSphere.destroy();
}



//====================================================================================================================================
void TPGLWindow::createTextures()
{
    // IMAGE EARTH
    // pDataImage0 : POINTER on IMAGE RGB (3 bytes per pixel)
    std::string filename = TP_PATH_TO_DATA "/earth.png";
    QImage img(filename.c_str());
    QImage img0 = img.convertToFormat(QImage::Format_RGB888,Qt::NoOpaqueDetection);

    unsigned char* pDataImage0 = img0.scanLine(0);


    // IMAGE EARTH BY NIGHT :
    // pDataImage1 : POINTER on IMAGE RGB (3 bytes per pixel)
    filename = TP_PATH_TO_DATA "/earth_night.png";
    img = QImage(filename.c_str());
    QImage img1 = img.convertToFormat(QImage::Format_RGB888,Qt::NoOpaqueDetection);

    unsigned char* pDataImage1 = img1.scanLine(0);



    glEnable(GL_TEXTURE_2D);

    // Creates the Texture object
    glGenTextures( 1, & m_iTexture );

    glBindTexture( GL_TEXTURE_2D, m_iTexture );
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, img0.width(), img0.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pDataImage0 );
    }
    glBindTexture( GL_TEXTURE_2D, 0 );


    // Creates the Texture object
    glGenTextures( 1, & m_iTexture2 );

    glBindTexture( GL_TEXTURE_2D, m_iTexture2 );
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB8, img1.width(), img1.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, pDataImage1 );
    }
    glBindTexture( GL_TEXTURE_2D, 0 );
}

//====================================================================================================================================
void TPGLWindow::destroyTextures()
{
    glDeleteTextures( 1, & m_iTexture );
    glDeleteTextures( 1, & m_iTexture2 );
}

//====================================================================================================================================
void TPGLWindow::setupTexturesInUnit()
{
    // Specify on which texture unit the uniform texture sampler must be bound
    // Activates the texture unit 0
    glActiveTexture( GL_TEXTURE0 + 0 );
    //
    glBindTexture( GL_TEXTURE_2D, m_iTexture );

    glActiveTexture( GL_TEXTURE0 + 1 );
    //
    glBindTexture( GL_TEXTURE_2D, m_iTexture2 );
}

//====================================================================================================================================
void TPGLWindow::initialize()
{
    // Prints the GL Version
    TP_LOG("GL Version : %s\n",(char*)glGetString(GL_VERSION));

    m_GPUProgramPhongTextured.createFromFiles( "VS_phong_textured.glsl", "", "FS_phong_textured.glsl" );
    m_GPUProgramPostProcess.createFromFiles( "VS_blit.glsl", "", "FS_blit.glsl" );

    getUniformLocations();
    createMesh();
    createTextures();

    // ------------------------------------
    createRenderTarget();
}

//====================================================================================================================================
void TPGLWindow::updateMatrices()
{
    const glm::vec3 vRight  (1,0,0);
    const glm::vec3 vUp     (0,1,0);
    const glm::vec3 vFront  (0,0,-1);
    const glm::vec3 vCenter (0,0,-1);

    // Projection matrix is "simply" a perspective matrix, so use the right tool function from GLM
    m_mtxCameraProjection       = glm::perspective(glm::radians(45.0f), (float)width()/height(), 0.1f, 100.f);


    // View matrix is built from the common "lookAt" paradigm, using the RH (Right-Handed) coordinate system
    m_mtxCameraView             = glm::lookAtRH( m_vCameraPosition, vCenter, vUp );

    // Finally, build the World matrix for the model taking into account its translate and orientation
    glm::mat4 Model             = glm::translate(glm::mat4(1.0f), m_vObjectTranslate);
    m_mtxObjectWorld            = glm::rotate(Model, glm::radians(-90.0f), vRight);
    m_mtxObjectWorld            = glm::rotate(m_mtxObjectWorld, m_vObjectEulerAngles.y, vFront);
}

//====================================================================================================================================
void TPGLWindow::update()
{
    // NB : Just like render(), this method is called every frame, as often/fast as possible

//    static int siFrameID        = 0;

    // Update light position, so that it is animated
    float   fTimeElapsed        = (float) m_timer.elapsed();
    m_vLightPosition            = glm::vec3( 100 * cos(fTimeElapsed/1000), 0, 100 * sin(fTimeElapsed/1000) );

    // make sure the matrix data are init to some valid values
    updateMatrices();

//    siFrameID++;
}

//====================================================================================================================================
void TPGLWindow::render()
{
    // Specifies the viewport size -----------------------------------------------------------------
    const float retinaScale = devicePixelRatio();
    glViewport( 0, 0, width() * retinaScale, height() * retinaScale );

    bindRenderTarget();

    // Specify winding order Counter ClockZise (even though it's default on OpenGL) ----------------
    glFrontFace( GL_CCW );

    // Enables (Back) Face Culling -----------------------------------------------------------------
    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );


    if( m_bAlphaBlend )
    {
        // Disables Depth Test --------------------------------------------------------------------------
        glDisable( GL_DEPTH_TEST );

        // Enables Depth Write -------------------------------------------------------------------------
        glDepthMask( GL_FALSE );

        // Enables Alpha Blending ----------------------------------------------------------------------
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    //    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    }
    else
    {
        // Enables Depth Test --------------------------------------------------------------------------
        glEnable( GL_DEPTH_TEST );
        glDepthFunc( GL_LESS );

        // Enables Depth Write -------------------------------------------------------------------------
        glDepthMask( GL_TRUE );

        // Disables Alpha Blending ----------------------------------------------------------------------
        glDisable( GL_BLEND );
    }

    // Specify the color to use when clearing theframebuffer --------------------------------------
    glClearColor( 0.05f, 0.2f, 0.3f, 1.0f );

    // Clears the framebuffer ---------------------------------------------------------------------
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );


    // Starts using the given GPU Program ---------------------------------------------------------
    m_GPUProgramPhongTextured.bind();
    {
        // Setup the Texture to be used
        setupTexturesInUnit();

        // Sends the uniforms var from the CPU to the GPU -----------------------------------------
        sendUniformsToGPU();

        m_MeshSphere.draw();

    }
    // Stops using the GPU Program ----------------------------------------------------------------
    m_GPUProgramPhongTextured.unbind();

    unbindRenderTarget();


    // TODO : draw the FOB color texture onto a screen quad

    // Clears the framebuffer ---------------------------------------------------------------------
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

    m_GPUProgramPostProcess.bind();
    {
        glActiveTexture( GL_TEXTURE0 + 0 );

        glBindTexture( GL_TEXTURE_2D, m_iColorRenderTexture );

        // Specify on which texture unit the uniform texture sampler must be bound
        glUniform1i( m_iUniformSamplerPost, 0 );

        m_MeshScreen.draw();
    }
    m_GPUProgramPostProcess.unbind();
}


//====================================================================================================================================
void TPGLWindow::resizeEvent(QResizeEvent* /*_pEvent*/)
{
    // Force the update of the perspective matrix
    updateMatrices();
}

//====================================================================================================================================
void TPGLWindow::keyPressEvent(QKeyEvent* _pEvent)
{
    if(QEvent::KeyPress == _pEvent->type() )
    {
//        float   fTimeElapsed= (float) m_timer.elapsed();

        float   fMoveSpeed          = 0.07f;// * fTimeElapsed;
        bool    bHasObjectMoved     = false;


        // handle key press to move the 3D object
        switch( _pEvent->key() )
        {
        case Qt::Key_B:
            m_bAlphaBlend = !m_bAlphaBlend;
            break;

        case Qt::Key_P:
            writeFBOToFile( "FBO.png" );
            break;

        case Qt::Key_R:
            m_vObjectTranslate = glm::vec3(0, 0, 0);
            m_vCameraPosition  = glm::vec3(0,0,fCameraZInit);
            bHasObjectMoved = true;
            break;

        case Qt::Key_Minus:
            m_vObjectTranslate -= glm::vec3(0, 0, 1) * fMoveSpeed;
            bHasObjectMoved = true;
            break;

        case Qt::Key_Plus:
            m_vObjectTranslate += glm::vec3(0, 0, 1) * fMoveSpeed;
            bHasObjectMoved = true;
            break;

        case Qt::Key_Left:
            m_vObjectTranslate -= glm::vec3(1, 0, 0) * fMoveSpeed;
            bHasObjectMoved = true;
            break;

        case Qt::Key_Right:
            m_vObjectTranslate += glm::vec3(1, 0, 0) * fMoveSpeed;
            bHasObjectMoved = true;
            break;

        case Qt::Key_Up:
            m_vObjectTranslate += glm::vec3(0, 1, 0) * fMoveSpeed;
            bHasObjectMoved = true;
            break;

        case Qt::Key_Down:
            m_vObjectTranslate -= glm::vec3(0, 1, 0) * fMoveSpeed;
            bHasObjectMoved = true;
            break;

        case Qt::Key_Space:
            m_vObjectEulerAngles -= glm::vec3(0, glm::radians(1.0f), 0);
            bHasObjectMoved = true;
            break;

        case Qt::Key_Return:
        case Qt::Key_Enter:
            break;

        case Qt::Key_Escape:
            this->close();
            break;

        }
    }
}

//====================================================================================================================================
void TPGLWindow::writeFBOToFile( const std::string& _rstrFilePath )
{
    const float retinaScale = devicePixelRatio();
    GLsizei W = width() * retinaScale;
    GLsizei H = height() * retinaScale;

    // this is called Pixel Transfer operation: http://www.opengl.org/wiki/Pixel_Transfer
    uchar* aPixels = new uchar[ W * H * 4 ];
    memset( aPixels, 0, W * H * 4 );

    glBindFramebuffer( GL_FRAMEBUFFER, m_iFBO );
    glReadPixels( 0,0,  W, H, GL_RGBA, GL_UNSIGNED_BYTE, aPixels );

    glBindFramebuffer( GL_FRAMEBUFFER,  0 );

    QImage qImage = QImage( aPixels, W, H, QImage::Format_ARGB32 );
    // flip the image vertically, cause OpenGL has them upside down!
    qImage = qImage.mirrored(false, true);
    qImage = qImage.rgbSwapped();

    qImage.save( _rstrFilePath.c_str() );
}


//====================================================================================================================================
int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QSurfaceFormat format;

#ifdef Q_OS_MAC  // what about Linux ?
    format.setMajorVersion(3);
    format.setMinorVersion(3);
    format.setProfile(QSurfaceFormat::CoreProfile);
#endif
    format.setSamples(16);
    format.setDepthBufferSize(24);

    TPGLWindow window;
    window.setFormat(format);
    window.resize(800, 600);
    window.show();

    return app.exec();
}
