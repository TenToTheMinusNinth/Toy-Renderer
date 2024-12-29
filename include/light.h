#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>


class DirectLight{
public:
	glm::vec3 position;
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
	 DirectLight(glm::vec3 positioninput,glm::vec3 directioninput,glm::vec3 ambientinput,glm::vec3 diffuseinput,glm::vec3 specularinput) {
		 position= positioninput;
		 direction = directioninput;
		 ambient = ambientinput;
		 diffuse = diffuseinput;
		 specular = specularinput;
	}
	 DirectLight(glm::vec3 directioninput) {
		 position = glm::vec3(-2.0f, 4.0f, -1.0f);
		 direction = directioninput;

		 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		 specular = glm::vec3(0.5f, 0.5f, 0.5f);
	 }
	 DirectLight() {
		 position = glm::vec3(-2.0f, 4.0f, -1.0f);
		 direction = glm::vec3(-1.0f, -5.0f, 1.0f);
		 
		 ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		 diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		 specular = glm::vec3(0.5f, 0.5f, 0.5f);
	 }
	 void SetDirection(glm::vec3 directioninput) {
		 direction = directioninput;
	 }
	 void SetParameter(glm::vec3 ambientinput, glm::vec3 diffuseinput, glm::vec3 specularinput) {
		 ambient = ambientinput;
		 diffuse = diffuseinput;
		 specular = specularinput;
	 }
private:

};

class PointLight {
public:
	glm::vec3 position;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	glm::vec3 color;

	float constant;
	float linear;
	float quadratic;

	
	PointLight(glm::vec3 positioninput, float constantinput,float linearinput,float quadraticinput,glm::vec3 ambientinput, glm::vec3 diffuseinput, glm::vec3 specularinput, glm::vec3 colorinput) {
		position = positioninput;
		color = colorinput;

		constant = constantinput;
		linear = linearinput;
		quadratic = quadraticinput;

		ambient = ambientinput;
		diffuse = diffuseinput;
		specular = specularinput;
	}
	PointLight(glm::vec3 positioninput) {
		position = positioninput;
		color = glm::vec3(1.0, 1.0, 1.0);

		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;
		ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		specular = glm::vec3(0.5f, 0.5f, 0.5f);
	}
	
	PointLight(glm::vec3 positioninput, glm::vec3 colorinput) {
		position = positioninput;
		color = colorinput;

		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;
		ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		specular = glm::vec3(0.5f, 0.5f, 0.5f);
	}
	PointLight() {
		position = glm::vec3(0.0f, 0.0f, 0.0f);
		color = glm::vec3(1.0, 1.0, 1.0);

		constant = 1.0f;
		linear = 0.09f;
		quadratic = 0.032f;
		ambient = glm::vec3(0.05f, 0.05f, 0.05f);
		diffuse = glm::vec3(0.4f, 0.4f, 0.4f);
		specular = glm::vec3(0.5f, 0.5f, 0.5f);
	}
	
	void SetPosition(glm::vec3 positioninput) {
		position = positioninput;
	}
	void SetLightParameter(glm::vec3 ambientinput, glm::vec3 diffuseinput, glm::vec3 specularinput) {
		 ambient = ambientinput;
		 diffuse = diffuseinput;
		 specular = specularinput;
	 }
	void SetAttenuationParameter(float constantinput, float linearinput, float quadraticinput) {
		constant = constantinput;
		linear = linearinput;
		quadratic = quadraticinput;
	}
private:

};


#endif
