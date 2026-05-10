#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vetexColors;

uniform mat4 u_ModelMatrix;
uniform mat4 u_Projection;
out vec3 v_vertexColors;

void main()
{
    v_vertexColors=vetexColors;
    vec4 newPosition=u_Projection*u_ModelMatrix*vec4(position,1.0f);
    // Don't Forget newPosition.w
    gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
}