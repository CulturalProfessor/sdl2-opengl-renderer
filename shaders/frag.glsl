#version 410 core

struct Material {
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};

struct Light {
   vec3 position;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

in vec3 v_vertexColors;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec3 viewPos;
uniform Material material;
uniform Light light;

void main()
{
   vec3 norm=normalize(Normal);
   vec3 lightDir=normalize(light.position-FragPos);
   float diff=max(dot(norm,lightDir),0.0);
   vec3 viewDir = normalize(viewPos-FragPos);
   vec3 reflectDir = reflect(-lightDir,norm);
   float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

   vec3 ambient = light.ambient*vec3(texture(material.diffuse,TexCoord));
   vec3 diffuse = light.diffuse*diff*vec3(texture(material.diffuse,TexCoord));
   vec3 specular = light.specular*spec*vec3(texture(material.specular,TexCoord).r);
   
   // for color
   // vec3 result=(ambient+diffuse)*v_vertexColors+specular;
   
   // for texture
   vec3 result = ambient + diffuse+specular; 
   FragColor = vec4(result,1.0);
}
