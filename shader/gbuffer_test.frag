#version 460 core

//gBuffer
uniform sampler2D gPositionDepth;//position and depth
uniform sampler2D gNormalMetallic;//normal and metallic
uniform sampler2D gAlbedoRoughness;//albedo and roughness

in vec2 TexCoords;
out vec4 Lo;

void main(){
	
	Lo=texture(gAlbedoRoughness,TexCoords);

}
