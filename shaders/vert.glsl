#version 410 core

layout(location=0) in vec3 position;
layout(location=1) in vec3 vetexColors;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in vec3 aNormal;

uniform mat4 u_ModelMatrix;
uniform mat4 u_Projection;
uniform mat4 u_ViewMatrix;

out vec3 v_vertexColors;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main()
{
    Normal = mat3(transpose(inverse(u_ModelMatrix))) * aNormal;
    FragPos=vec3(u_ModelMatrix*vec4(position,1.0));
    v_vertexColors=vetexColors;
    TexCoord=aTexCoord;
    vec4 newPosition=u_Projection*u_ViewMatrix*u_ModelMatrix*vec4(position,1.0f);
    // Don't Forget newPosition.w
    gl_Position = vec4(newPosition.x, newPosition.y, newPosition.z, newPosition.w);
}