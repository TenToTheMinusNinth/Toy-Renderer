#version 460 core
out vec4 FragColor;

uniform vec3 lightcolor;

void main()
{
    FragColor = vec4(lightcolor,1.0); // ���������ĸ�����ȫ������Ϊ1.0
}