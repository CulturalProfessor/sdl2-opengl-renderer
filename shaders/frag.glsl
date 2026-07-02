#version 410 core

in vec3 v_vertexColors;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec3 lightColor;
uniform vec3 lightPos;

void main()
{
   vec3 norm=normalize(Normal);
   vec3 lightDir=normalize(lightPos-FragPos);
   float diff=max(dot(norm,lightDir),0.0);
   vec3 diffuse=diff*lightColor;

   float ambientStrength = 0.1f;
   vec3 ambient = ambientStrength*lightColor;
   vec3 texColor = texture(ourTexture,TexCoord).rgb;
   
   // for color
   // vec3 result=(ambient+diffuse)*v_vertexColors;
   
   // for texture
   vec3 result = (ambient + diffuse) * texColor; 
   FragColor = vec4(result,1.0);
}
