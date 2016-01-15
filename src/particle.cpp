#include "particle.h"

Particle::Particle()
{

}

Particle::~Particle()
{

}

void Particle::update()
{
    m_fTtl-=1;
    m_iElapsedTime++;
}

//Setters//

void Particle::setPosition(const glm::vec3& position)
{
    m_afPosition=position;
}

void Particle::setColor(const glm::vec4& color)
{
    m_afColor=color;
}

void Particle::setRotation(float rad)
{
    m_fRotation=rad;
}

void Particle::setWidth(float w)
{
    m_fWidth=w;
}

void Particle::setHeight(float h)
{
    m_fHeight=h;
}

void Particle::setTtl(float ttl)
{
    m_fTtl=ttl;
}

//===============================

const glm::vec3& Particle::getPosition()
{
    return m_afPosition;
}

const glm::vec4& Particle::getColor()
{
    return m_afColor;
}

float Particle::getRotation()
{
    return m_fRotation;
}

float Particle::getWidth()
{
    return m_fWidth;
}

float Particle::getHeight()
{
    return m_fHeight;
}

float Particle::getTtl() const
{
    return m_fTtl;
}

//===============================

const glm::vec3& Particle::getVelocity() const
{
    return m_afVelocity;
}

void Particle::setVelocity(const glm::vec3& v)
{
    m_afVelocity=v;
}

int Particle::getElapsedTime() const
{
    return m_iElapsedTime;
}

void Particle::resetElapsedTime()
{
    m_iElapsedTime=0;
}
