#include "func_updateparticles.h"

///Position

Func_UpdateParticle_Position_NoChange::Func_UpdateParticle_Position_NoChange()
{}

Particle& Func_UpdateParticle_Position_NoChange::operator()(Particle& particle)
{
    return particle;
}

//==============================================

Func_UpdateParticle_Position_Acceleration_RandomSpread::Func_UpdateParticle_Position_Acceleration_RandomSpread(const glm::vec3& acceleration, float spread):
    afAcceleration(acceleration), fSpread(spread)
{
    TP_ASSERT(fSpread!=0, "spread shouldn't be passed with value %f\n",fSpread);
}

Particle& Func_UpdateParticle_Position_Acceleration_RandomSpread::operator()(Particle& particle)
{
    glm::vec3 baseVelocity=particle.getVelocity();
    float fHalfSpread=fSpread/2;
    if(fSpread!=0)
    {
        float rx = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/fSpread)) - fHalfSpread;
        float ry = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/fSpread)) - fHalfSpread;
        float rz = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/fSpread)) - fHalfSpread;
        baseVelocity=glm::rotateZ(glm::rotateY(glm::rotateX(baseVelocity, rx), ry), rz);
    }
    particle.setPosition(particle.getPosition()
                         + particle.getVelocity()
                         + afAcceleration*(float)(particle.getElapsedTime()));
    particle.setVelocity(baseVelocity
                         + afAcceleration*(float)particle.getElapsedTime());
    return particle;
}

//==============================================

Func_UpdateParticle_Position_Direction_Converge::Func_UpdateParticle_Position_Direction_Converge()
{}

Func_UpdateParticle_Position_Direction_Converge::Func_UpdateParticle_Position_Direction_Converge(const glm::vec3& direction, float convertIntensity):
    afDirection(direction), fconvergeIntensity(convertIntensity)
{
    TP_ASSERT(fconvergeIntensity!=0, "invertedConvertIntensity shouldn't be passed with value %f\n",fconvergeIntensity);
}

Particle& Func_UpdateParticle_Position_Direction_Converge::operator()(Particle& particle)
{
    glm::vec3 currentVelocity(particle.getVelocity());
    //get length and normalize (without wasting time)
    float len=glm::length(currentVelocity);
    currentVelocity/=len;

    //We can now compare our vectors and complete them
    currentVelocity.x+=(currentVelocity.x>afDirection.x ? -fconvergeIntensity : fconvergeIntensity);
    currentVelocity=glm::normalize(currentVelocity);
    currentVelocity.y+=(currentVelocity.y>afDirection.y ? -fconvergeIntensity : fconvergeIntensity);
    currentVelocity=glm::normalize(currentVelocity);
    currentVelocity.z+=(currentVelocity.z>afDirection.z ? -fconvergeIntensity : fconvergeIntensity);
    currentVelocity=glm::normalize(currentVelocity);

    currentVelocity*=len;

    //give the same velocity, except modified with the proper angle
    particle.setPosition(particle.getPosition()
                         + currentVelocity);
    particle.setVelocity(currentVelocity);
    return particle;
}

///Color

Func_UpdateParticle_Color_NoChange::Func_UpdateParticle_Color_NoChange()
{}

Particle& Func_UpdateParticle_Color_NoChange::operator()(Particle& particle)
{
    return particle;
}

//==============================================

Func_UpdateParticle_Color_ChangeRandom::Func_UpdateParticle_Color_ChangeRandom()
{}

Particle& Func_UpdateParticle_Color_ChangeRandom::operator()(Particle& particle)
{
    if(particle.getElapsedTime()>1)
        return particle;
    glm::vec4 color;
    color.r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)) / 2;
    color.g = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)) / 2;
    color.b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX)) / 2;
    color.a = 1;
    particle.setColor(color);
    return particle;
}

//==============================================

Func_UpdateParticle_Color_Vanish::Func_UpdateParticle_Color_Vanish()
{}

Particle& Func_UpdateParticle_Color_Vanish::operator()(Particle& particle)
{
    //the ttl of the particle determines its appearance
    float ttl=particle.getTtl();
    float timeLived=(float)particle.getElapsedTime();
    float totalTime=timeLived+ttl;
    particle.setColor(glm::vec4(1,1,1,(ttl/totalTime)));
    return particle;
}

///Rotation

Func_UpdateParticle_Rotation_NoChange::Func_UpdateParticle_Rotation_NoChange()
{}

Particle& Func_UpdateParticle_Rotation_NoChange::operator()(Particle& particle)
{
    return particle;
}

//==============================================

Func_UpdateParticle_Rotation_RotateAngle::Func_UpdateParticle_Rotation_RotateAngle(float rad):
    fAngle(rad)
{}

Particle& Func_UpdateParticle_Rotation_RotateAngle::operator()(Particle& particle)
{
    float fTotalAngle=particle.getRotation()+fAngle;
    if(fTotalAngle>M_PI)
    {
        fTotalAngle-=2*M_PI;
    }
    else if(fTotalAngle<M_PI)
    {
        fTotalAngle+=2*M_PI;
    }
    particle.setRotation(fTotalAngle);
    return particle;
}

///Size

Func_UpdateParticle_Size_NoChange::Func_UpdateParticle_Size_NoChange()
{}

Particle& Func_UpdateParticle_Size_NoChange::operator()(Particle& particle)
{
    return particle;
}

//==============================================

Func_UpdateParticle_Size_ScaleAtCap::Func_UpdateParticle_Size_ScaleAtCap(float seuil, float scale):
    fCap(seuil), fScale(scale)
{}

Particle& Func_UpdateParticle_Size_ScaleAtCap::operator()(Particle& particle)
{
    float ttl=particle.getTtl();
    float timeLived=(float)particle.getElapsedTime();
    float totalTime=ttl+timeLived;
    if((timeLived=(ttl/totalTime)) < fCap)
    {
        particle.setWidth(particle.getWidth()*fScale);
        particle.setHeight(particle.getHeight()*fScale);
    }
    return particle;
}

///Init

Func_UpdateParticle_Init_Default::Func_UpdateParticle_Init_Default()
{

}

Particle& Func_UpdateParticle_Init_Default::operator()(Particle& particle)
{
    particle.setColor(glm::vec4(0,0,0,0));

    particle.setRotation(0);

    particle.setHeight(1.0f);
    particle.setWidth(1.0f);

    particle.setTtl(500);

    //non attribute
    particle.resetElapsedTime();
    particle.setVelocity(glm::vec3(0,0,0));

    return particle;
}

//==============================================

Func_UpdateParticle_Init_Spread::Func_UpdateParticle_Init_Spread()
{}

Func_UpdateParticle_Init_Spread::Func_UpdateParticle_Init_Spread(const glm::vec3& direction):
    afDirection(direction)
{}

Particle& Func_UpdateParticle_Init_Spread::operator()(Particle& particle)
{
    particle.setColor(glm::vec4(0,0,0,0));

    particle.setRotation(0);

    particle.setHeight(1.0f);
    particle.setWidth(1.0f);

    particle.setTtl(500);

    //non attribute
    particle.resetElapsedTime();
    glm::vec3 baseVelocity(afDirection);
    float halfPi=M_PI/2;
    float rx = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/M_PI)) - halfPi;
    float ry = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/M_PI)) - halfPi;
    float rz = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX/M_PI)) - halfPi;
    particle.setVelocity(glm::rotateZ(glm::rotateY(glm::rotateX(baseVelocity, rx), ry), rz));
    return particle;
}
