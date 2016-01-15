#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "particle.h"
#include "common.h"
#include "func_updateparticles.h"
#include <stack>
#include <QOpenGLFunctions_3_3_Core>
#include <type_traits>
#include <cstddef>

class Func_CompareParticle
{
public:
    bool operator()(const Particle& object, const Particle& other) const
    {
        return object.getTtl() < other.getTtl();
    }
};


class ParticleSystem : QOpenGLFunctions_3_3_Core
{
public:
    ParticleSystem();

    bool update(bool newParticle, IFunc_UpdateParticle_Init& ui,
                IFunc_UpdateParticle_Position& up, IFunc_UpdateParticle_Color& uc,
                IFunc_UpdateParticle_Rotation& ur, IFunc_UpdateParticle_Size& us);

    void setParameters(Particle& p, IFunc_UpdateParticle_Init& ui) const;

    /// Renders this mesh
    void    draw();

    /// Creates a Mesh from the given position data, normal data, UV data, indices data
    bool    create(GLenum _eDrawMode) ;

    /// Destroys this Mesh
    void    destroy();

private:

    /// Creates a Buffer Obhect for the given target, ...
    GLuint  createBuffer( GLenum _eTarget, GLsizeiptr _iSize, GLvoid* _aData );

    /// Sets up the Vertex Attrib Pointer
    void    setupAttrib(GLuint _iVBO, GLuint _iAttribLocation, GLint _iSize , GLsizei _iStride);

    ///the particles themselves
    std::vector<Particle>   m_particles;
    int                     m_ifirstFreeIndex;

    glm::vec3               m_afOrigin;
    GLenum                  m_eDrawMode;

    GLuint      			m_iVAO;                     ///< VertexArrayObject used for drawing

    GLuint      			m_iVBOParticle;             ///< VertexBufferObject used to store position vertex data, used by the VAO
    GLuint      			m_iAttribParticle;          ///< GLSL attribute location for vertex position

    unsigned char*          m_ptrTexture;

    GLint                   m_status;

    unsigned int            m_maxSize;      ///< for buffer orphaning
};

#endif // PARTICLESYSTEM_H
