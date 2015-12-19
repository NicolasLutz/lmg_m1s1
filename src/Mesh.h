#ifndef __MESH_H__
#define __MESH_H__

#include "common.h"

#include <QOpenGLFunctions_3_3_Core>



class Mesh : protected QOpenGLFunctions_3_3_Core
{
public:
    /// CTOR
    Mesh();    
    /// DTOR
    virtual ~Mesh();

    /// Renders this mesh
    void    draw();

    /// Creates a Mesh from the given position data, normal data, UV data, indices data
    bool    createFrom( GLenum _eDrawMode, GLuint _iVertexCount, glm::vec3* _afPositions, glm::vec3* _afNormals, glm::vec2* _afTexCoords, GLuint _iElementsCount, GLuint* _auiIndices ) ;

    /// Destroys this Mesh
    void    destroy();

private:

    /// Creates a Buffer Obhect for the given target, ...
    GLuint  createBuffer( GLenum _eTarget, GLsizeiptr _iSize, GLvoid* _aData );

    /// Sets up the Vertex Attrib Pointer
    void    setupVertexAttrib( GLuint _iVBO, GLuint _iAttribLocation, GLint _iSize );


    GLenum                  m_eDrawMode;

    GLuint      			m_iVertexCount;             ///< Number of vertices in the VAO
    GLuint      			m_iElementsCount;           ///< Number of indices used for drawing

    GLuint      			m_iVAO;                     ///< VertexArrayObject used for drawing
    GLuint      			m_iEBO;                     ///< ElementBufferObject used to store indices of vertices

    GLuint      			m_iVBOPosition;             ///< VertexBufferObject used to store position vertex data, used by the VAO
    GLuint                  m_iVBONormal;               ///< VertexBufferObject used to store normal vertex data, used by the VAO
    GLuint                  m_iVBOTexCoords;            ///< VertexBufferObject used to store texture coordinates vertex data, used by the VAO

    GLuint      			m_iAttribPosition;          ///< GLSL attribute location for vertex position
    GLuint                  m_iAttribNormal;            ///< GLSL attribute location for vertex normal
    GLuint                  m_iAttribTexCoords;         ///< GLSL attribute location for vertex texture coordinates
};

#endif // __MESH_H__

