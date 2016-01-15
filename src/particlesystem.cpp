#include "particlesystem.h"

#include <QOpenGLFunctions_3_3_Core>

ParticleSystem::ParticleSystem() : QOpenGLFunctions_3_3_Core()
  , m_particles()
  , m_afOrigin(0,0,0)
  , m_status(GL_NO_ERROR)
  , m_maxSize(32)
  , m_funcCompare()
{
    TP_ASSERT(std::is_standard_layout<Particle>::value, "Particle is not a %s\n", "standard layout type");
}

ParticleSystem::~ParticleSystem()
{
    destroy();
}

bool ParticleSystem::update(bool newParticle, IFunc_UpdateParticle_Init& ui,
            IFunc_UpdateParticle_Position& up, IFunc_UpdateParticle_Color& uc,
            IFunc_UpdateParticle_Rotation& ur, IFunc_UpdateParticle_Size& us,
            const glm::mat4& objectToWorld, const glm::vec3 &cameraWPos)
{
    if(newParticle)
    {
        m_particles.push_back(Particle());
        setParameters(m_particles[m_particles.size()-1], ui);
        m_particles[m_particles.size()-1].updateDistanceToCamera(objectToWorld, cameraWPos);
        if(m_particles.size()>m_maxSize)
        {
            m_maxSize=m_maxSize*2; //used for buffer orphaning
        }
    }
    unsigned int pos=0;
    std::vector<Particle>::iterator it=m_particles.begin();
    for(; it!=m_particles.end(); ++it, ++pos)
    {
        up((*it));
        uc((*it));
        ur((*it));
        us((*it));
        (*it).update(); //apply every function and diminuish ttl
        (*it).updateDistanceToCamera(objectToWorld, cameraWPos);
    }
    //Sort the particles in function of their distance to the camera
    std::sort(m_particles.begin(), m_particles.end(), m_funcCompare);

    while(pos>0 && (*(--it)).getTtl()<=0)
    {//diminuish vector size if the last particles are dead
        m_particles.erase(it);
        --pos;
    }
    if(m_particles.size() < (m_maxSize/2) && m_maxSize>32) //Shrink future vbo size
        m_maxSize=m_maxSize/2;

    // VBO UPDATE: our particle system isn't static and both its size and content change
    glBindBuffer(GL_ARRAY_BUFFER, m_iVBOParticle);
    {
        // Buffer orphaning: enhances performances
        glBufferData(GL_ARRAY_BUFFER, m_maxSize*sizeof(Particle), NULL, GL_STREAM_DRAW);

        //Data filling using glBufferSubData
        glBufferSubData(GL_ARRAY_BUFFER, 0, m_particles.size()*sizeof(Particle), &m_particles[0]);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return true;
}

void ParticleSystem::setParameters(Particle& p, IFunc_UpdateParticle_Init &ui) const
{
    p.setPosition(m_afOrigin);
    ui(p);
}


//====================================================================================================================================
void ParticleSystem::draw()
{
    //--------------------------------------------------------------------------------------------------------------------
    // precondition
    TP_ASSERT( 0 != m_iVAO ,           "m_iVAO should not be 0 (here it is '%d')      - Did you call createFrom(...) ?.\n", m_iVAO );
    //--------------------------------------------------------------------------------------------------------------------

    //DRAW
    glBindVertexArray( m_iVAO );
    glDrawArrays( m_eDrawMode, 0, m_particles.size() );
    glBindVertexArray( 0 );


    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}
//====================================================================================================================================
bool ParticleSystem::create( GLenum _eDrawMode)
{
    //--------------------------------------------------------------------------------------------------------------------
    // precondition
    TP_ASSERT(      GL_POINTS           == _eDrawMode
                ||  GL_LINES            == _eDrawMode
                ||  GL_LINE_LOOP        == _eDrawMode
                ||  GL_LINE_STRIP       == _eDrawMode
                ||  GL_TRIANGLES        == _eDrawMode
                ||  GL_TRIANGLE_STRIP   == _eDrawMode
                ||  GL_TRIANGLE_FAN     == _eDrawMode
                , "_eDrawMode is invalid (here it is '%d').\n", _eDrawMode );
    //--------------------------------------------------------------------------------------------------------------------

    // required to have the OpenGL functions working - this is because of our use of OpenGL with Qt
    initializeOpenGLFunctions();

    m_eDrawMode         = _eDrawMode;

    // Creates 1 Buffer object for ELEMENTS ------------------------------------------------------------------------
    glGenBuffers( 1, &m_iVBOParticle );

    // Binds the Buffer, to say "we will work on this one from now"
    glBindBuffer( GL_ARRAY_BUFFER, m_iVBOParticle );
    {
        glBufferData( GL_ARRAY_BUFFER, m_maxSize * sizeof(Particle), NULL, GL_STREAM_DRAW );
    }
    // Unbinds the Buffer, we are done working on it !
    glBindBuffer( GL_ARRAY_BUFFER, 0 );

    // Creates 1 VAO
    glGenVertexArrays( 1, &m_iVAO );

    //Attrib pointers
    glBindVertexArray( m_iVAO );
    {
        if( 0 != m_iVBOParticle )
        {
            glBindBuffer( GL_ARRAY_BUFFER, m_iVBOParticle );
/*position*/glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), NULL); //takes 12 bytes
            glEnableVertexAttribArray(0);
/*color*/   glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)12); //+16 bytes
            glEnableVertexAttribArray(1);
/*rotation*/glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)28); //+4 bytes
            glEnableVertexAttribArray(2);
/*width*/   glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)32); //+4 bytes
            glEnableVertexAttribArray(3);
/*height*/  glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)36); //+4 bytes
            glEnableVertexAttribArray(4);
/*ttl*/     glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)40); //+4 bytes
            glEnableVertexAttribArray(5);
            glBindBuffer( GL_ARRAY_BUFFER, 0 );
        }
    }
     // Stops using the VAO
    glBindVertexArray( 0 );


    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
    return true;
}
//====================================================================================================================================
void ParticleSystem::destroy()
{
    //--------------------------------------------------------------------------------------------------------------------
    // precondition
    TP_ASSERT( 0 != m_iVAO ,            "m_iVAO should not be 0 (here it is '%d') - have you called createFrom(...) ?.\n", m_iVAO );
    //--------------------------------------------------------------------------------------------------------------------

    if( 0 != m_iVBOParticle)
    {
        glDeleteBuffers( 1, & m_iVBOParticle );
        m_iVBOParticle = 0;
    }

    glDeleteVertexArrays( 1, & m_iVAO );
    m_iVAO = 0;

    TP_ASSERT((m_status=glGetError())==GL_NO_ERROR,"error: %s\n",gluErrorString(m_status));
}
