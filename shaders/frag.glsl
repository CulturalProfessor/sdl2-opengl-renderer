#version 410 core

struct Material {
   sampler2D diffuse;
   sampler2D specular;
   float shininess;
};
 
struct DirLight {
   vec3 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;
};

struct PointLight {
   vec3 position;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float constant;
   float linear;
   float quadratic;
};

struct SpotLight {
   vec3 position;
   vec3 direction;

   vec3 ambient;
   vec3 diffuse;
   vec3 specular;

   float cutOff;
   float outerCutOff;
};

in vec3 v_vertexColors;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D ourTexture;
uniform vec3 viewPos;
uniform Material material;
uniform DirLight dirlight;
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform SpotLight spotlight;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);  
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
   vec3 norm=normalize(Normal);
   vec3 viewDir = normalize(viewPos-FragPos);

   // Directional lighting
   vec3 result = CalcDirLight(dirlight,norm,viewDir);

   // point lights 
   for (int i=0;i<NR_POINT_LIGHTS;i++){
      result += CalcPointLight(pointLights[i],norm,FragPos,viewDir);
   }

   // Spot Light
   result += CalcSpotLight(spotlight,norm,FragPos,viewDir);

   FragColor = vec4(result,1.0);
}


vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir){
   vec3 lightDir = normalize(-light.direction);

   // diffuse shading
   float diff=max(dot(normal,lightDir),0.0);

   // specular shading
   vec3 reflectDir = reflect(-lightDir,normal);
   float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

   vec3 ambient = light.ambient*vec3(texture(material.diffuse,TexCoord));
   vec3 diffuse = light.diffuse*diff*vec3(texture(material.diffuse,TexCoord));
   vec3 specular = light.specular*spec*vec3(texture(material.specular,TexCoord).r);

   return (ambient+diffuse+specular);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
   vec3 lightDir=normalize(light.position-FragPos);

   // diffuse shading
   float diff=max(dot(normal,lightDir),0.0);

   // specular shading
   vec3 reflectDir = reflect(-lightDir,normal);
   float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

   // attenuation
   float distance = length(light.position-FragPos);
   float attenuation = 1.0/(light.constant+light.linear*distance +light.quadratic*(distance*distance));

   vec3 ambient = light.ambient*vec3(texture(material.diffuse,TexCoord));
   vec3 diffuse = light.diffuse*diff*vec3(texture(material.diffuse,TexCoord));
   vec3 specular = light.specular*spec*vec3(texture(material.specular,TexCoord).r);

   ambient *= attenuation;
   diffuse *= attenuation;
   specular *= attenuation;
   
   return (ambient+diffuse+specular);
}

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir){
   vec3 lightDir=normalize(light.position-FragPos);

   float theta = dot(lightDir,normalize(-light.direction));
   float epsilon = light.cutOff - light.outerCutOff;
   float intensity = clamp((theta - light.outerCutOff)/epsilon,0.0,1.0);
      
   // diffuse shading
   float diff=max(dot(normal,lightDir),0.0);

   // specular shading
   vec3 reflectDir = reflect(-lightDir,normal);
   float spec = pow(max(dot(viewDir,reflectDir),0.0),material.shininess);

   vec3 diffuse = light.diffuse*diff*vec3(texture(material.diffuse,TexCoord));
   vec3 specular = light.specular*spec*vec3(texture(material.specular,TexCoord).r);

   diffuse *= intensity;
   specular *= intensity;

   return (diffuse+specular);
}