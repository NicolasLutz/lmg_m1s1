#version 330

//in

layout (points) in ;
in vec4    vs_vColor[];
in float   vs_fWidth[];
in float   vs_fHeight[];
in float   vs_fTtl[];

//out

layout (triangle_strip, max_vertices=4) out;
out vec4    gs_vColor;
out float   gs_fTtl;
out vec2    gs_vST;

void main()
{
    float wDiv2=vs_fWidth[0]*0.5f;
    float hDiv2=vs_fHeight[0]*0.5f;

    gl_Position = gl_in[0].gl_Position+vec4(-wDiv2,-hDiv2,0,0);
    gs_vColor=vs_vColor[0];
    gs_fTtl=vs_fTtl[0];
    gs_vST=vec2(0,0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position+vec4(wDiv2,-hDiv2,0,0);
    gs_vColor=vs_vColor[0];
    gs_fTtl=vs_fTtl[0];
    gs_vST=vec2(1,0);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position+vec4(-wDiv2,hDiv2,0,0);
    gs_vColor=vs_vColor[0];
    gs_fTtl=vs_fTtl[0];
    gs_vST=vec2(0,1);
    EmitVertex();

    gl_Position = gl_in[0].gl_Position+vec4(wDiv2,hDiv2,0,0);
    gs_vColor=vs_vColor[0];
    gs_fTtl=vs_fTtl[0];
    gs_vST=vec2(1,1);
    EmitVertex();
    EndPrimitive();
}
