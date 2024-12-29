#version 460 core
layout (location = 0) in vec3 position;

uniform mat4 lightprojection;
uniform mat4 lightview;
uniform mat4 model;

void main()
{
    gl_Position = lightprojection * lightview * model * vec4(position, 1.0f);
}