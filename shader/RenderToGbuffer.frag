#version 460 core
layout (location = 0) out vec4 gPositionDepth;
layout (location = 1) out vec4 gNormalMetallic;
layout (location = 2) out vec4 gAlbedoRoughness;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

struct Material {
    sampler2D texture_albedo1;
    sampler2D texture_normal1;
    sampler2D texture_metallic1;
    sampler2D texture_roughness1;
}; 

uniform Material material;

const float NEAR = 0.1; // ͶӰ����Ľ�ƽ��
const float FAR = 50.0f; // ͶӰ�����Զƽ��
float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // �ص�NDC
    return (2.0 * NEAR * FAR) / (FAR + NEAR - z * (FAR - NEAR));    
}

void main()
{    
    
    gPositionDepth.rgb = FragPos;
    gPositionDepth.a = LinearizeDepth(gl_FragCoord.z); 
   
    gNormalMetallic.rgb = normalize(Normal);//��ʱʹ��ģ������ķ���
    gNormalMetallic.a = texture(material.texture_metallic1,TexCoords).r;
   
    gAlbedoRoughness.rgb = texture(material.texture_albedo1, TexCoords).rgb;
    gAlbedoRoughness.a = texture(material.texture_roughness1, TexCoords).r;

}