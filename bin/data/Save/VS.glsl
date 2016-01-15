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
out float   vs_fWidth;
out float   vs_fHeight;
out float   vs_fTtl;

//debug
bool debug;

void main()
{
    debug=true;


    vec4 vWorldPosition = u_mtxWorld * vec4( vtx_position, 1.0f );

    //out

    if(f_ttl<=10.0f)
    {
	gl_Position = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    else
	gl_Position = u_mtxProjection * u_mtxView * vWorldPosition;
    if(debug)
    {
        vs_vColor=vec4(vtx_position, 1.0f);
        vs_fWidth=0.2f;
        vs_fHeight=0.2f;
        vs_fTtl=0;
    }
    else
    {
        vs_vColor=vtx_color;
        vs_fWidth=f_width;
        vs_fHeight=f_height;
        vs_fTtl=f_ttl;
    }
}
