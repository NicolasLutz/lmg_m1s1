#version 330

bool debug;

// sampler for the RGBA texture used by the billboard
uniform sampler2D u_tex;
uniform bool u_bUseColor;

// we should use this, instead of gl_FragColor which is deprecated since GL version 3.0
//in

in vec4    gs_vColor;
in float   gs_fTtl;
in vec2    gs_vST;

//out

out vec4 out_fragColor;

void main (void)
{
    vec4 t;

    if(u_bUseColor)
    {
	t=texture2D(u_tex,gs_vST)*gs_vColor;
    }
    else
    {
	t=texture2D(u_tex,gs_vST);
    }
    if(t.a <= 0.005f)
      discard;
    out_fragColor = t;
}
