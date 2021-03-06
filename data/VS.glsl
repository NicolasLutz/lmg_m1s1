#version 330

//uniform

uniform mat4 u_mtxWorld, u_mtxView, u_mtxProjection;

//in

layout(location=0)  in vec3 vtx_position; //position of the particle
layout(location=1)  in vec4 vtx_color;  //color of the particle
layout(location=2)  in float f_rotation; //rotation of the particle
layout(location=3)  in float f_width;    //current width of the particle
layout(location=4)  in float f_height;   //current height of the particle
layout(location=5)  in float f_ttl;      //time to live: discard if <=0

//out

out vec4    vs_vColor;

out float   vs_fRotation;

out float   vs_fWidth;
out float   vs_fHeight;

out float   vs_fTtl;

//debug
bool debug;

void main()
{

    vec4 vWorldPosition = u_mtxWorld * vec4( vtx_position, 1.0f );

    //out

    gl_Position = u_mtxProjection * u_mtxView * vWorldPosition;

    vs_vColor=vtx_color;
    vs_fRotation=f_rotation;
    vs_fWidth=f_width;
    vs_fHeight=f_height;
    vs_fTtl=f_ttl;
}
