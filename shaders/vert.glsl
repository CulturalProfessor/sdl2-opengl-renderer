#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vetexColors;
layout(location=2) in vec2 aTexCoord;

uniform mat4 u_ModelMatrix;
uniform mat4 u_Projection;
uniform mat4 u_ViewMatrix;

out vec3 v_vertexColors;
out vec2 TexCoord;

void main()
{
    v_vertexColors=vetexColors;
    TexCoord=aTexCoord;
    vec4 newPosition=u_Projection*u_ViewMatrix*u_ModelMatrix*vec4(position,1.0f);
    // Don't Forget newPosition.w
    gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
}