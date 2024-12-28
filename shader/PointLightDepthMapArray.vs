#version 460 core
layout (location = 0) in vec3 position;

uniform mat4 shadowMatrices;
uniform mat4 model;

void main()
{
    gl_Position = shadowMatrices * model * vec4(position, 1.0);
}