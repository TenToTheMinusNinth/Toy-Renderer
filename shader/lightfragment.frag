#version 460 core
out vec4 FragColor;

uniform vec3 lightcolor;

void main()
{
    FragColor = vec4(lightcolor,1.0); // 将向量的四个分量全部设置为1.0
}