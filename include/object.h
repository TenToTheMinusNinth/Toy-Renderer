#ifndef OBJECT_H
#define OBJECT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <shader.h>
#include <config.h>
#include <model.h>
#include <string>
class Object {
public:
	glm::vec3 translate;
	glm::vec3 rotate;
	glm::vec3 scale;
	glm::mat4 modelmatrix;
	Model* model;
	Object(Model* modelinput,glm::vec3 translateinput, glm::vec3 rotateinput,glm::vec3 scaleinput):model(modelinput),translate(translateinput),rotate(rotateinput),scale(scaleinput){
		modelmatrix = glm::mat4(1.0);
		modelmatrix = glm::translate(modelmatrix, translate);
		modelmatrix = glm::rotate(modelmatrix, glm::radians(rotate.r), glm::vec3(1.0, 0.0, 0.0));
		modelmatrix = glm::rotate(modelmatrix, glm::radians(rotate.g), glm::vec3(0.0, 1.0, 0.0));
		modelmatrix = glm::rotate(modelmatrix, glm::radians(rotate.b), glm::vec3(0.0, 0.0, 1.0));
		modelmatrix = glm::scale(modelmatrix, scale);
	}
	void render(Shader& shader){
		shader.setMat4("model", modelmatrix);
		model->Draw(shader);
	}
	
private:
	
};
#endif

