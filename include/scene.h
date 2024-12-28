#ifndef SCENE_H
#define SCENE_H

#include <object.h>
#include <light.h>
#include <vector>

class Scene {
public:
	vector <Object> objects;
	DirectLight directlight;
	vector<PointLight> pointlight;
	int PointLightNumber;
	static Shader simplelightshader;
	Scene(vector <Object> objectsinput, DirectLight directlightinput,vector<PointLight> pointlightinput):
		objects(objectsinput), directlight(directlightinput), pointlight(pointlightinput)
	{
		PointLightNumber = pointlight.size();
	}
	void addobject(Object& object) {
		objects.push_back(object);
	}

	void render(Shader& shader) {
		for (int i = 0; i < objects.size(); i++) {
			objects[i].render(shader);
		}
	}

	//draw point light
	void DrawPointLight(glm::mat4 projection,glm::mat4 view,Model box, GLuint ID){
		glClear(GL_DEPTH_BUFFER_BIT);
		//将Gbuffer中的深度缓冲复制到默认帧缓冲中
		glBindFramebuffer(GL_READ_FRAMEBUFFER, ID);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入到默认帧缓冲
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		simplelightshader.use();
		simplelightshader.setMat4("projection", projection);
		simplelightshader.setMat4("view", view);
		for (int i = 0; i < PointLightNumber; i++) {
			glm::mat4 model = glm::mat4(1.0);
			model = glm::translate(model, pointlight[i].position);
			model = glm::scale(model, glm::vec3(0.02f));
			simplelightshader.setMat4("model", model);
			simplelightshader.setVec3("lightcolor",pointlight[i].color);
			box.Draw(simplelightshader);
		}
	}
private:

};
Shader Scene::simplelightshader =
	Shader();
#endif
