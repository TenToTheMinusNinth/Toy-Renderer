#version 460 core
out vec4 FragColor;


in VS_OUT{
    vec3 FragPos;
    vec3 Normal;
}fs_in;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{
    vec3 I=normalize(fs_in.FragPos-cameraPos);
    vec3 R=reflect(I,normalize(fs_in.Normal));
    FragColor = vec4(texture(skybox,R).rgb,1.0);
}