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
    debug=true;

    vec4 t=texture2D(u_tex,gs_vST);
    if(t.a == 0 || gs_fTtl <= 0)
      discard;
    if(debug)
	out_fragColor = t;
    else
    {
        if(u_bUseColor)
            out_fragColor = t + gs_vColor;
        else
            out_fragColor = t;
    }
}
