#version 410 core

in vec3 v_vertexColors;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;
void main()
{
   FragColor=texture(ourTexture,TexCoord);
}
