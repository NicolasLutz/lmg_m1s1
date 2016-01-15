#ifndef PARTICLE_H
#define PARTICLE_H

#include "common.h"
#include <QOpenGLFunctions_3_3_Core>

//The following is a standard layout type.
//We pack it to pass it to the glsl vertex shader without word alignment

#pragma pack(push, 0)
class Particle
{
public:
    Particle();
    ~Particle();

    void update();

    //Setters//

    void setPosition(const glm::vec3& position);

    void setColor(const glm::vec4 &color);

    void setRotation(float rad);

    void setWidth(float w);
    void setHeight(float h);

    void setTtl(float ttl);

    //Getters//

    const glm::vec3& getPosition();

    const glm::vec4 &getColor();

    float getRotation();

    float getWidth();
    float getHeight();

    float getTtl() const;

    //Non attributes//

    const glm::vec3& getVelocity() const;
    void setVelocity(const glm::vec3 &v);

    int getElapsedTime() const;
    void resetElapsedTime();

private:

    //Attributes//
    glm::vec3       m_afPosition;       ///< current position of the particle

    glm::vec4       m_afColor;

    float           m_fRotation;

    float           m_fWidth;           ///< current width of the particle
    float           m_fHeight;          ///< current height of the particle
    float           m_fTtl;            ///< time to live (in float because we need to pass it to the gpu)

    //Non attributes: will be ignored by our vao (but still fetched to the vbo!!)//

    glm::vec3       m_afVelocity;   ///< used by the functors to work according to the current particle's velocity.
    int             m_iElapsedTime;
};
#pragma pack(pop)

#endif // PARTICLE_H
