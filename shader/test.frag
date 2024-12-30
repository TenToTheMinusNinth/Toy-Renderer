#version 460 core
out vec4 FragColor;

in vec2 TexCoords;
struct Matrial{
 sampler2D texture_diffuse1;
 sampler2D texture_specular1;
};
uniform Matrial material;
void main()
{    
    FragColor = texture(material.texture_specular1, TexCoords);
}