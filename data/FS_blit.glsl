#version 330

// get these data from the previous Shader Unit in the pipeline
in vec2 vs_texCoords;

uniform sampler2D u_tex;

// we should use this, instead of gl_FragColor which is deprecated since GL version 3.0
out vec4 out_fragColor;

const bool bSepiaColor      = false;
const bool bGreyScaleColor  = true;
const bool bInvertColor     = false;

void main()
{
    vec4 color = texture( u_tex, vs_texCoords );

    if( bSepiaColor )
    {
        // kinda like a poor man sepia filtering
        out_fragColor = color.ggga;
    }

    if( bGreyScaleColor )
    {
        vec3 greyscale  = vec3( dot( color.rgb, vec3( 0.3, 0.59, 0.11 )));
        out_fragColor   = vec4( greyscale, 1.0 );
    }

    if( bInvertColor )
    {
        out_fragColor = vec4( vec3( 1, 1, 1 ) - color.rgb, 1 );
    }

//    out_fragColor = color;
}
