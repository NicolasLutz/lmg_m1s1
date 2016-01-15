#ifndef __MAIN_H__
#define __MAIN_H__

#include "openglwindow.h"

#include "common.h"
#include "GPUProgram.h"


#include <QTime>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtGui/QKeyEvent>
#include "particlesystem.h"

//------------------------------------------------------------------------------------


class TPGLWindow : public OpenGLWindow
{
public:
    TPGLWindow();
    virtual ~TPGLWindow();

    void initialize();

    void render();

    void update();

private:

    /// Handles key press event, and move the 3D object
    void keyPressEvent(QKeyEvent *);

    /// Handles resize event
    void resizeEvent(QResizeEvent*);

    //------------------------------------------------------------------------------------
    /// Get the GLSL uniform locations
    void getUniformLocations();

    /// Send the uniform vars from the CPU to the GPU
    void sendUniformsToGPU();

    //------------------------------------------------------------------------------------
    /// Update the CameraProjection, CameraView, ObjectWorld matrices
    void updateMatrices();

    //------------------------------------------------------------------------------------
    /// Creates the Texture object from a texture file
    void createTextures();

    /// Destroys all Texture objects
    void destroyTextures();

    /// Bind the textures to their texture unit
    void setupTexturesInUnit();

    //------------------------------------------------------------------------------------
    /// Destroys all RenderTarget related data
    void destroyRenderTarget();

    /// Bind the FBO RenderTarget so that we can draw into it
    void bindRenderTarget();

    /// Unbind the FBO RenderTarget so that we can draw in the default Frame Buffer again
    void unbindRenderTarget();

    /// Writes the FBO color attachment to the disk
    void writeFBOToFile( const std::string& _rstrFilePath );


    GPUProgram              m_GPUProgram;

    //------------------------------------------------------------------------------------
    glm::vec3   			m_vObjectTranslate;         ///< Store the 3D object translate component
    glm::vec3   			m_vObjectEulerAngles;       ///< Store the 3D object orientation as euler angles

    glm::mat4   			m_mtxObjectWorld;           ///< Matrix transform, for object -> world coordinates
    glm::mat4   			m_mtxCameraView;            ///< Matrix transform, for world -> camera view coordinates
    glm::mat4   			m_mtxCameraProjection;      ///< Matrix transform, for camera view -> camera projection coordinates

    GLuint      			m_iUniformWorld;            ///< GLSL uniform location for World matrix
    GLuint      			m_iUniformView;             ///< GLSL uniform location for View matrix
    GLuint      			m_iUniformProjection;       ///< GLSL uniform location for Projection matrix

    //------------------------------------------------------------------------------------
    QTime       			m_timer;                    ///< Time used to get elapsed time between 2 frames

    glm::vec3               m_vCameraPosition;          ///< Camera position - in World Space

    GLboolean               m_bUseColor;                ///< Whether or not additional color is used on particles
    GLuint                  m_iUniformUseColor;         ///< GLSL uniform location for particles' color usage

    //------------------------------------------------------------------------------------

    GLuint                  m_iTexture;                 ///< OpenGL ID for TextureObject - used for diffuse texture
    GLuint                  m_iUniformTextureUnit;      ///< GLSL uniform location for the sampler "u_tex"

    bool                    m_bAlphaBlend;              ///< Use Alpha Blending ?

    //------------------------------------------------------------------------------------
    GLuint                  m_iFBO;                     ///< OpenGL ID for Frame Buffer Object
    GLuint                  m_iColorRenderTexture;      ///< OpenGL ID for Texture Object - used as Color Buffer on the FBO

    ParticleSystem          m_ParticleSystem;

    Func_UpdateParticle_Init_Spread                         m_funcInit;
    Func_UpdateParticle_Position_Direction_Converge         m_funcPosition;
    Func_UpdateParticle_Color_Vanish                        m_funcColor;
    Func_UpdateParticle_Rotation_RotateAngle                m_funcRotation;
    Func_UpdateParticle_Size_ScaleAtCap                     m_funcSize;

    bool                                                    m_bSpreadFire;
    Func_UpdateParticle_Position_Acceleration_RandomSpread  m_funcSpreadFire;

    GLint                   m_status;

};
//====================================================================================================================================



#endif // __MAIN_H__

