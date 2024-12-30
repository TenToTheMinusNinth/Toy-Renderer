#version 460 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec4 gNormalMetallic;
layout (location = 2) out vec4 gAlbedoRoughness;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

struct Material{
 sampler2D texture_diffuse1;
 sampler2D texture_specular1;
};

uniform Material material;

const float NEAR = 0.1; // 投影矩阵的近平面
const float FAR = 50.0f; // 投影矩阵的远平面
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // 回到NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
}

void main()
{    
    
    gPositionDepth.rgb = FragPos;
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z); 
   
    gNormalMetallic.rgb = normalize(Normal);//暂时使用模型自身的法线
    gNormalMetallic.a = texture(material.texture_specular1,TexCoords).b;
   
    gAlbedoRoughness.rgb = texture(material.texture_diffuse1, TexCoords).rgb;
    gAlbedoRoughness.a = texture(material.texture_specular1, TexCoords).g;

}