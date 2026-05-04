#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vetexColors;

uniform float u_Offset; // uniform variable

out vec3 v_vertexColors;

void main()
{
    v_vertexColors=vetexColors;
    gl_Position = vec4(position.x, position.y+u_Offset, position.z, 1.0f);
}