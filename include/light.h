#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

class Light {
public:
	glm::vec3 position;
	glm::vec3 color;
	double radientflux = 1000.0;//辐射通量，单位W
	float density=1.0;//范围[0,1]，不是物理量
	Light() {

	}
	void SetPosition(glm::vec3 positioninput) {
		position = positioninput;
	}
	void SetColor(glm::vec3 colorinput) {
		color = colorinput;
	}
	void SetRadientflux(double radientfluxinput) {
		radientflux = radientfluxinput;
	}
	void SetDensity(float densityinput) {
		density = densityinput;
	}
private:

};
class DirectLight : public Light{
public:
	glm::vec3 direction;
	 DirectLight(glm::vec3 directioninput,glm::vec3 colorinput){
		position = glm::vec3(0.0f, 10.0f, 0.0f);
		color = colorinput;
		direction = directioninput;
	 }
	 DirectLight() {
		 position = glm::vec3(0.0f, 10.0f, 0.0f);
		 color = glm::vec3(1.0, 1.0, 1.0);
		 direction = glm::vec3(0.0f, -1.0f, 0.0f);
	 }
	 void SetDirection(glm::vec3 directioninput) {
		 direction = directioninput;
	 }
private:

};

class PointLight : public Light{ 
public:
	//光源的衰减系数
	float constant = 1.0;
	float linear = 0.045;
	float quadratic = 0.0075;
	PointLight(glm::vec3 positioninput ,glm::vec3 colorinput) {
		position = positioninput;
		color = colorinput;
	}
	PointLight(){
		position = glm::vec3(0.0f, 10.0f, 0.0f);
		color = glm::vec3(1.0, 1.0, 1.0);
	}		
	void SetAttenuationParameter(float constantinput, float linearinput, float quadraticinput) {
		constant = constantinput;
		linear = linearinput;
		quadratic = quadraticinput;
	}
private:

};


#endif
