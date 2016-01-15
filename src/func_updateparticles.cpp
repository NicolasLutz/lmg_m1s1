#include "func_updateparticles.h"

///Position

Func_UpdateParticle_Position_NoChange::Func_UpdateParticle_Position_NoChange()
{}

Particle& Func_UpdateParticle_Position_NoChange::operator()(Particle& particle)
{
    return particle;
}

Func_UpdateParticle_Position_Acceleration_RandomSpread::Func_UpdateParticle_Position_Acceleration_RandomSpread():
    fSpread(0)
{}

Func_UpdateParticle_Position_Acceleration_RandomSpread::Func_UpdateParticle_Position_Acceleration_RandomSpread(const glm::vec3& acceleration, float spread):
    afAcceleration(acceleration), fSpread(spread)
{}

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
                         + particle.getVelocity()*(float)particle.getElapsedTime()
                         + afAcceleration*(float)(particle.getElapsedTime()*particle.getElapsedTime()));
    particle.setVelocity(baseVelocity
                         + afAcceleration*(float)particle.getElapsedTime());
    return particle;
}

Func_UpdateParticle_Position_Direction_Converge::Func_UpdateParticle_Position_Direction_Converge()
{}

Func_UpdateParticle_Position_Direction_Converge::Func_UpdateParticle_Position_Direction_Converge(const glm::vec3& direction)
{}

Particle& Func_UpdateParticle_Position_Direction_Converge::operator()(Particle& particle)
{

}

///Color

Func_UpdateParticle_Color_NoChange::Func_UpdateParticle_Color_NoChange()
{}

Particle& Func_UpdateParticle_Color_NoChange::operator()(Particle& particle)
{
    return particle;
}

///Rotation

Func_UpdateParticle_Rotation_NoChange::Func_UpdateParticle_Rotation_NoChange()
{}

Particle& Func_UpdateParticle_Rotation_NoChange::operator()(Particle& particle)
{
    return particle;
}

///Size

Func_UpdateParticle_Size_NoChange::Func_UpdateParticle_Size_NoChange()
{}

Particle& Func_UpdateParticle_Size_NoChange::operator()(Particle& particle)
{
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

    particle.setTtl(5000);

    //non attribute
    particle.resetElapsedTime();
    particle.setVelocity(glm::vec3(0,0,0));

    return particle;
}

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

    particle.setTtl(5000);

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
