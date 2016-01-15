#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "particle.h"
#include "common.h"
#include "func_updateparticles.h"
#include <stack>
#include <QOpenGLFunctions_3_3_Core>
#include <type_traits>
#include <cstddef>

class Func_ParticleCompare
{
public:
    bool operator()(const Particle& object, const Particle& other)
    {
        if(object.getTtl()<=0)
        {
            if(other.getTtl()>0)
                return false;
        }
        return object.getDistanceToCamera()>other.getDistanceToCamera();
    }
};

class ParticleSystem : QOpenGLFunctions_3_3_Core
{
public:
    ParticleSystem();
    ~ParticleSystem();

    bool update(bool newParticle, IFunc_UpdateParticle_Init& ui,
                IFunc_UpdateParticle_Position& up, IFunc_UpdateParticle_Color& uc,
                IFunc_UpdateParticle_Rotation& ur, IFunc_UpdateParticle_Size& us,
                const glm::mat4 &objectToWorld, const glm::vec3& cameraWPos);

    void setParameters(Particle& p, IFunc_UpdateParticle_Init& ui) const;

    /// Renders this mesh
    void    draw();

    /// Creates a Mesh from the given position data, normal data, UV data, indices data
    bool    create(GLenum _eDrawMode) ;

    /// Destroys this Mesh
    void    destroy();

private:

    ///the particles themselves
    std::vector<Particle>   m_particles;

    glm::vec3               m_afOrigin;
    GLenum                  m_eDrawMode;

    GLuint      			m_iVAO;                     ///< VertexArrayObject used for drawing

    GLuint      			m_iVBOParticle;             ///< VertexBufferObject used to store position vertex data, used by the VAO
    GLuint      			m_iAttribParticle;          ///< GLSL attribute location for vertex position

    GLint                   m_status;

    unsigned int            m_maxSize;      ///< for buffer orphaning

    Func_ParticleCompare    m_funcCompare;


};

#endif // PARTICLESYSTEM_H
