#include "main.h"

static const float fCameraXInit = 0.f;
static const float fCameraZInit = 8.f;
static const float fCameraYInit = 10.f;

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
    , m_bUseColor           (GL_TRUE)
    , m_iUniformUseColor    (0)
    , m_iTexture            (0)
    , m_iUniformTextureUnit (0)
    , m_bAlphaBlend         ( true )
    , m_iFBO                ( 0 ) //--------------------------------------
    , m_iColorRenderTexture ( 0 )
    , m_GPUProgram          ()
    , m_ParticleSystem      ()
    , m_funcInit            (glm::vec3(0,0,0.01f))
    , m_funcPosition        (glm::vec3(0,0,1), 0.01f)
    , m_funcColor           ()
    , m_funcRotation        ()
    , m_funcSize            ()
    , m_bSpreadFire         ( false )
    , m_funcSpreadFire      ()
    , m_status              (GL_NO_ERROR)
{
    m_vCameraPosition   = glm::vec3(fCameraXInit,fCameraYInit,fCameraZInit);

    m_timer.start();

    // request the window to render in loop
    setAnimating(true);
}

//====================================================================================================================================
void TPGLWindow::destroyRenderTarget()
{
    glDeleteTextures( 1 , & m_iColorRenderTexture );
    glDeleteFramebuffers( 1, & m_iFBO );

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}

//====================================================================================================================================
void TPGLWindow::bindRenderTarget()
{
    glBindFramebuffer( GL_FRAMEBUFFER, m_iFBO );
    const float retinaScale = devicePixelRatio();
    glViewport( 0, 0, width() * retinaScale, height() * retinaScale );

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}

//====================================================================================================================================
void TPGLWindow::unbindRenderTarget()
{
    glBindFramebuffer( GL_FRAMEBUFFER, 0 );
    const float retinaScale = devicePixelRatio();
    glViewport( 0, 0, width() * retinaScale, height() * retinaScale );

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}

//====================================================================================================================================
TPGLWindow::~TPGLWindow()
{
    m_GPUProgram.destroy();

    destroyTextures();
    destroyRenderTarget();
}

//====================================================================================================================================
void TPGLWindow::getUniformLocations()
{
    //--------------------------------------------------------------------------------------------------------------------
    // precondition
    TP_ASSERT( 0 != m_GPUProgram.getID() , "m_GPUProgram.getID() should not be 0 (here it is '%d') - Did you call createFromFiles() on it ?.\n", m_GPUProgram.getID() );
    //--------------------------------------------------------------------------------------------------------------------

    const GLuint iProgram = m_GPUProgram.getID();

    m_iUniformWorld             = glGetUniformLocation( iProgram, "u_mtxWorld" );
    m_iUniformView              = glGetUniformLocation( iProgram, "u_mtxView" );
    m_iUniformProjection        = glGetUniformLocation( iProgram, "u_mtxProjection" );

    // Retrieves the uniform location for the texture SAMPLER used in the GLSL Fragment Shader
    m_iUniformTextureUnit       = glGetUniformLocation( iProgram, "u_tex" );

    m_iUniformUseColor          = glGetUniformLocation( iProgram, "u_bUseColor");

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}

//====================================================================================================================================
void TPGLWindow::sendUniformsToGPU()
{
    //
    glUniformMatrix4fv( m_iUniformWorld, 1, GL_FALSE, glm::value_ptr(m_mtxObjectWorld) );
    glUniformMatrix4fv( m_iUniformView, 1, GL_FALSE, glm::value_ptr(m_mtxCameraView) );
    glUniformMatrix4fv( m_iUniformProjection, 1, GL_FALSE, glm::value_ptr(m_mtxCameraProjection) );

    // Specify on which texture unit the uniform texture sampler must be bound
    glUniform1i( m_iUniformTextureUnit, 0 );

    glUniform1i( m_iUniformUseColor,  m_bUseColor);

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}

//====================================================================================================================================
void TPGLWindow::createTextures()
{
    // IMAGE EARTH
    // pDataImage0 : POINTER on IMAGE RGBA (4 bytes per pixel)
    std::string filename = TP_PATH_TO_DATA "/Particles/fire4_alpha.png";
    QImage img(filename.c_str());
    QImage img0 = img.convertToFormat(QImage::Format_RGBA8888_Premultiplied,Qt::NoOpaqueDetection);

    unsigned char* pDataImage0 = img0.scanLine(0);

    glEnable(GL_TEXTURE_2D);

    // Creates the Texture object
    glGenTextures( 1, & m_iTexture );

    glBindTexture( GL_TEXTURE_2D, m_iTexture );
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

        glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, img0.width(), img0.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, pDataImage0 );
    }
    glBindTexture( GL_TEXTURE_2D, 0 );

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}

//====================================================================================================================================
void TPGLWindow::destroyTextures()
{
    glDeleteTextures( 1, & m_iTexture );
}

//====================================================================================================================================
void TPGLWindow::setupTexturesInUnit()
{
    // Specify on which texture unit the uniform texture sampler must be bound
    // Activates the texture unit 0
    glActiveTexture( GL_TEXTURE0 + 0 );
    //
    glBindTexture( GL_TEXTURE_2D, m_iTexture );

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}

//====================================================================================================================================
void TPGLWindow::initialize()
{
    // Prints the GL Version
    TP_LOG("GL Version : %s\n",(char*)glGetString(GL_VERSION));

    m_GPUProgram.createFromFiles( "VS.glsl", "GS.glsl", "FS.glsl" );

    getUniformLocations();

    m_ParticleSystem.create(GL_POINTS);
    createTextures();

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
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

    static int siFrameID        = 0;

    if(!m_bSpreadFire)
        m_ParticleSystem.update(true, m_funcInit, m_funcPosition, m_funcColor, m_funcRotation, m_funcSize, m_mtxObjectWorld, m_vCameraPosition);
    else
        m_ParticleSystem.update(false, m_funcInit, m_funcSpreadFire, m_funcColor, m_funcRotation, m_funcSize, m_mtxObjectWorld, m_vCameraPosition);


    // make sure the matrix data are init to some valid values
    updateMatrices();

    siFrameID++;

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
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

        // Disables Depth Write -------------------------------------------------------------------------
        glDepthMask( GL_FALSE );

        // Enables Alpha Blending ----------------------------------------------------------------------
        glEnable( GL_BLEND );
        glBlendFunc( GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
        //glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
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
    glClearColor( 0, 0, 0, 1.0f );

    // Clears the framebuffer ---------------------------------------------------------------------
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );


    // Starts using the given GPU Program ---------------------------------------------------------
    m_GPUProgram.bind();
    {
        // Setup the Texture to be used
        setupTexturesInUnit();

        // Sends the uniforms var from the CPU to the GPU -----------------------------------------
        sendUniformsToGPU();

        m_ParticleSystem.draw();
    }
    // Stops using the GPU Program ----------------------------------------------------------------
    m_GPUProgram.unbind();

    unbindRenderTarget();

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
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

        case Qt::Key_C:
            m_bUseColor = !m_bUseColor;

        case Qt::Key_S:
            m_bSpreadFire = !m_bSpreadFire;

        case Qt::Key_R:
            m_vObjectTranslate = glm::vec3(0, 0, 0);
            m_vCameraPosition  = glm::vec3(fCameraXInit,fCameraYInit,fCameraZInit);
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

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
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
