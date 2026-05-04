#version 410 core

in vec3 v_vertexColors;

uniform float u_Offset; // uniform variable

out vec4 color;

void main()
{
    color = vec4(v_vertexColors.r-u_Offset,v_vertexColors.g,v_vertexColors.b,1.0f);
}
