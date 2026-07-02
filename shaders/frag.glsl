#version 410 core

in vec3 v_vertexColors;
in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec3 lightColor;

void main()
{
   float ambientStrength = 0.1f;
   vec3 ambient = ambientStrength*lightColor;
   FragColor = vec4(ambient*v_vertexColors,1.0);
   // FragColor = texture(ourTexture,TexCoord);
}
