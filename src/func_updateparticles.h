#ifndef IFUNC_UPDATEPARTICLE_H
#define IFUNC_UPDATEPARTICLE_H

#include "particle.h"
#include "common.h"

///Interfaces

class IFunc_UpdateParticle
{
public:
    virtual Particle& operator()(Particle& particle)=0;
};

class IFunc_UpdateParticle_Position : public IFunc_UpdateParticle
{};

class IFunc_UpdateParticle_Color : public IFunc_UpdateParticle
{};

class IFunc_UpdateParticle_Rotation : public IFunc_UpdateParticle
{};

class IFunc_UpdateParticle_Size : public IFunc_UpdateParticle
{};

class IFunc_UpdateParticle_Init : public IFunc_UpdateParticle
{};

///Position

class Func_UpdateParticle_Position_NoChange : public IFunc_UpdateParticle_Position
{
public:
    Func_UpdateParticle_Position_NoChange();
    virtual Particle& operator()(Particle& particle);
};

class Func_UpdateParticle_Position_Acceleration_RandomSpread : public IFunc_UpdateParticle_Position
{
public:
    Func_UpdateParticle_Position_Acceleration_RandomSpread();
    Func_UpdateParticle_Position_Acceleration_RandomSpread(const glm::vec3& acceleration, float spread);
    virtual Particle& operator()(Particle& particle);

    glm::vec3 afAcceleration;
    float fSpread;
};

class Func_UpdateParticle_Position_Direction_Converge : public IFunc_UpdateParticle_Position
{
public:
    Func_UpdateParticle_Position_Direction_Converge();
    Func_UpdateParticle_Position_Direction_Converge(const glm::vec3& direction);
    virtual Particle& operator()(Particle& particle);

    glm::vec3 afDirection;
};


///Color

class Func_UpdateParticle_Color_NoChange : public IFunc_UpdateParticle_Color
{
public:
    Func_UpdateParticle_Color_NoChange();
    virtual Particle& operator()(Particle& particle);
};

///Rotation

class Func_UpdateParticle_Rotation_NoChange : public IFunc_UpdateParticle_Rotation
{
public:
    Func_UpdateParticle_Rotation_NoChange();
    virtual Particle& operator()(Particle& particle);
};

///Size

class Func_UpdateParticle_Size_NoChange : public IFunc_UpdateParticle_Size
{
public:
    Func_UpdateParticle_Size_NoChange();
    virtual Particle& operator()(Particle& particle);
};

class Func_UpdateParticle_Init_Default : public IFunc_UpdateParticle_Init
{
public:
    Func_UpdateParticle_Init_Default();
    virtual Particle& operator()(Particle& particle);
};

class Func_UpdateParticle_Init_Spread : public IFunc_UpdateParticle_Init
{
public:
    Func_UpdateParticle_Init_Spread();
    Func_UpdateParticle_Init_Spread(const glm::vec3& direction);
    virtual Particle& operator()(Particle& particle);

    glm::vec3 afDirection;
};

#endif // IFUNC_UPDATEPARTICLE_H
