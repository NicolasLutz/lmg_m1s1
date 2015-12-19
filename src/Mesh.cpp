#include "Mesh.h"

//====================================================================================================================================
Mesh::Mesh()
:   m_eDrawMode         ( 0 )
,   m_iVertexCount      ( 0 )
,   m_iElementsCount    ( 0 )
,   m_iVAO              ( 0 )
,   m_iEBO              ( 0 )
,   m_iVBOPosition      ( 0 )
,   m_iVBONormal        ( 0 )
,   m_iVBOTexCoords     ( 0 )
,   m_iAttribPosition   ( 0 )
,   m_iAttribNormal     ( 0 )
,   m_iAttribTexCoords  ( 0 )
{
    // by default we force the use of some hard coded attribute locations
    // they MUST be properly defined in the VertexShader file as well with layout ( location = 0 ) and such
    m_iAttribPosition   = 0;
    m_iAttribNormal     = 1;
    m_iAttribTexCoords  = 2;
}
//====================================================================================================================================
Mesh::~Mesh()
{
    TP_ASSERT( 0 == m_iVAO ,            "m_iVAO should be 0 (here it is '%d') - have you called destroy() ?.\n", m_iVAO );
    TP_ASSERT( 0 == m_iEBO ,            "m_iEBO should be 0 (here it is '%d') - have you called destroy() ?.\n", m_iEBO );
    TP_ASSERT( 0 == m_iVBOPosition ,    "m_iVBOPosition should be 0 (here it is '%d') - have you called destroy() ?.\n", m_iVBOPosition );
    TP_ASSERT( 0 == m_iVBONormal ,      "m_iVBONormal should be 0 (here it is '%d') - have you called destroy() ?.\n", m_iVBONormal );
    TP_ASSERT( 0 == m_iVBOTexCoords ,   "m_iVBOTexCoords should be 0 (here it is '%d') - have you called destroy() ?.\n", m_iVBOTexCoords );
}
//====================================================================================================================================
void Mesh::draw()
{
    //--------------------------------------------------------------------------------------------------------------------
    // precondition
    TP_ASSERT( 0 != m_iVAO ,           "m_iVAO should not be 0 (here it is '%d')      - Did you call createFrom(...) ?.\n", m_iVAO );
    //--------------------------------------------------------------------------------------------------------------------

    // Starts using the given VAO, and its associated VBOs
    glBindVertexArray( m_iVAO );
    {
        if( 0 == m_iEBO )
        {
            glDrawArrays( m_eDrawMode, 0, m_iVertexCount );
        }
        else
        {
            // Starts using the EBO / Indices VBO
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iEBO );
            {
                glDrawElements( m_eDrawMode, m_iElementsCount, GL_UNSIGNED_INT, 0 );
            }
            // Stops using the EBO / Indices VBO
            glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
        }
    }
     // Stops using the VAO
    glBindVertexArray( 0 );
}
//====================================================================================================================================
bool Mesh::createFrom( GLenum _eDrawMode, GLuint _iVertexCount, glm::vec3* _afPositions, glm::vec3* _afNormals, glm::vec2* _afTexCoords, GLuint _iElementsCount, GLuint* _auiIndices )
{
    //--------------------------------------------------------------------------------------------------------------------
    // precondition
    TP_ASSERT(      GL_POINTS           == _eDrawMode
                ||  GL_LINES            == _eDrawMode
                ||  GL_LINE_LOOP        == _eDrawMode
                ||  GL_LINE_STRIP       == _eDrawMode
                ||  GL_TRIANGLES        == _eDrawMode
                ||  GL_TRIANGLE_STRIP   == _eDrawMode
                ||  GL_TRIANGLE_FAN     == _eDrawMode
                , "_eDrawMode is invalid (here it is '%d').\n", _eDrawMode );
    TP_ASSERT( 0 != _iVertexCount , "_iVertexCount should not be 0 (here it is '%d').\n", _iVertexCount );
    //--------------------------------------------------------------------------------------------------------------------

    // required to have the OpenGL functions working - this is because of our use of OpenGL with Qt
    initializeOpenGLFunctions();

    m_eDrawMode         = _eDrawMode;
    m_iVertexCount      = _iVertexCount;
    m_iElementsCount    = _iElementsCount;

    if( 0 != _afPositions )
    {
        m_iVBOPosition = createBuffer( GL_ARRAY_BUFFER, m_iVertexCount * sizeof(glm::vec3), _afPositions );
    }
    if( 0 != _afNormals )
    {
        m_iVBONormal = createBuffer( GL_ARRAY_BUFFER, m_iVertexCount * sizeof(glm::vec3), _afNormals );
    }
    if( 0 != _afTexCoords )
    {
        m_iVBOTexCoords = createBuffer( GL_ARRAY_BUFFER, m_iVertexCount * sizeof(glm::vec2), _afTexCoords );
    }

    if( 0 != _auiIndices )
    {
        m_iEBO = createBuffer( GL_ELEMENT_ARRAY_BUFFER, m_iElementsCount * sizeof(GLuint), _auiIndices );
    }


    // Creates 1 VAO
    glGenVertexArrays( 1, &m_iVAO );

    // Binds the VAO, to say "we will work on this one from now"
    glBindVertexArray( m_iVAO );
    {
        if( 0 != m_iVBOPosition )
        {
            setupVertexAttrib( m_iVBOPosition, m_iAttribPosition, 3 );
        }

        if( 0 != m_iVBONormal )
        {
            setupVertexAttrib( m_iVBONormal, m_iAttribNormal, 3 );
        }

        if( 0 != m_iVBOTexCoords )
        {
            setupVertexAttrib( m_iVBOTexCoords, m_iAttribTexCoords, 2 );
        }
    }
    // UnBinds the VAO, we are done working on it !
    glBindVertexArray( 0 );
}
//====================================================================================================================================
void Mesh::destroy()
{
    //--------------------------------------------------------------------------------------------------------------------
    // precondition
    TP_ASSERT( 0 != m_iVAO ,            "m_iVAO should not be 0 (here it is '%d') - have you called createFrom(...) ?.\n", m_iVAO );
    //--------------------------------------------------------------------------------------------------------------------

    if( 0 != m_iEBO )
    {
        glDeleteBuffers( 1, & m_iEBO );
        m_iEBO = 0;
    }
    if( 0 != m_iVBOTexCoords )
    {
        glDeleteBuffers( 1, & m_iVBOTexCoords );
        m_iVBOTexCoords = 0;
    }
    if( 0 != m_iVBONormal )
    {
        glDeleteBuffers( 1, & m_iVBONormal );
        m_iVBONormal = 0;
    }
    if( 0 != m_iVBOPosition )
    {
        glDeleteBuffers( 1, & m_iVBOPosition );
        m_iVBOPosition = 0;
    }

    glDeleteVertexArrays( 1, & m_iVAO );
    m_iVAO = 0;

}
//====================================================================================================================================
GLuint Mesh::createBuffer( GLenum _eTarget, GLsizeiptr _iSize, GLvoid* _aData )
{
    GLuint iBuffer;

    // Creates 1 Buffer object for ELEMENTS ------------------------------------------------------------------------
    glGenBuffers( 1, & iBuffer );

    // Binds the Buffer, to say "we will work on this one from now"
    glBindBuffer( _eTarget, iBuffer );
    {
        // Defines the type of Buffer data, the size in bytes, fills it with the given positions,
        //      and finally specifies what kind of Draws we will do (so that the driver can optimize the how it will access it)
        glBufferData( _eTarget, _iSize, _aData, GL_STATIC_DRAW );
    }
    // Unbinds the Buffer, we are done working on it !
    glBindBuffer( _eTarget, 0 );

    return iBuffer;
}
//====================================================================================================================================
void Mesh::setupVertexAttrib( GLuint _iVBO, GLuint _iAttribLocation, GLint _iSize )
{
    // Binds the Buffer, to say "we will work on this one from now"
    glBindBuffer( GL_ARRAY_BUFFER, _iVBO );
    {
        // Enables the attribute id used for vtx_texCoords
        glEnableVertexAttribArray( _iAttribLocation );
        // Gives the right params
        glVertexAttribPointer( _iAttribLocation, _iSize, GL_FLOAT, GL_FALSE, 0, 0 );
    }
//        glBindBuffer( GL_ARRAY_BUFFER, 0 );
}
