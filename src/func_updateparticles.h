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
    Func_UpdateParticle_Position_Acceleration_RandomSpread(const glm::vec3& acceleration=glm::vec3(0,0,0), float spread=0.1f);
    virtual Particle& operator()(Particle& particle);

    glm::vec3 afAcceleration;
    float fSpread;
};

class Func_UpdateParticle_Position_Direction_Converge : public IFunc_UpdateParticle_Position
{
public:
    Func_UpdateParticle_Position_Direction_Converge();
    Func_UpdateParticle_Position_Direction_Converge(const glm::vec3& direction, float convertIntensity);
    virtual Particle& operator()(Particle& particle);

    glm::vec3 afDirection;
    float fconvergeIntensity;
};


///Color

class Func_UpdateParticle_Color_NoChange : public IFunc_UpdateParticle_Color
{
public:
    Func_UpdateParticle_Color_NoChange();
    virtual Particle& operator()(Particle& particle);
};

class Func_UpdateParticle_Color_ChangeRandom : public IFunc_UpdateParticle_Color
{
public:
    Func_UpdateParticle_Color_ChangeRandom();
    virtual Particle& operator()(Particle& particle);
};

class Func_UpdateParticle_Color_Vanish : public IFunc_UpdateParticle_Color
{
public:
    Func_UpdateParticle_Color_Vanish();
    virtual Particle& operator()(Particle& particle);
};

///Rotation

class Func_UpdateParticle_Rotation_NoChange : public IFunc_UpdateParticle_Rotation
{
public:
    Func_UpdateParticle_Rotation_NoChange();
    virtual Particle& operator()(Particle& particle);
};

class Func_UpdateParticle_Rotation_RotateAngle : public IFunc_UpdateParticle_Rotation
{
public:
    Func_UpdateParticle_Rotation_RotateAngle(float rad=0.05f);
    virtual Particle& operator()(Particle& particle);

    float fAngle;
};

///Size

class Func_UpdateParticle_Size_NoChange : public IFunc_UpdateParticle_Size
{
public:
    Func_UpdateParticle_Size_NoChange();
    virtual Particle& operator()(Particle& particle);
};

class Func_UpdateParticle_Size_ScaleAtCap : public IFunc_UpdateParticle_Size
{
public:
    ///signifie que chaque appel scale par 0.96f si la durée de vie de la particule est < à 30%
    Func_UpdateParticle_Size_ScaleAtCap(float seuil=0.3f, float scale=0.96f);
    virtual Particle& operator()(Particle& particle);

    float fCap;
    float fScale;
};

///Init

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
