#version 330

// get the vertex position, and texCoords, as vec3 and vec2 input - sent from the CPU, via VertexBuffer
layout(location=0) in vec3 vtx_position;
layout(location=2) in vec2 vtx_texCoords;

out vec2 vs_texCoords;

void main()
{
    gl_Position     = vec4( vtx_position, 1.0f );
    vs_texCoords    = vtx_texCoords;
}
