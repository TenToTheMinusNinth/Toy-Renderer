#ifndef DEFER_H
#define DEFER_H

#include <glad/glad.h>
#include <glm-0.9.8.5/glm/glm.hpp>
#include <glm-0.9.8.5/glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <shader.h>
#include <config.h>
void RenderQuad();
class GBuffer {
public:
	GLuint ID;
	GLuint gPositionDepth, gNormalMetallic, gAlbedoRoughness;
	GLuint rboDepth;
	static Shader RenderToGbuffer;
	GBuffer(){
		glGenFramebuffers(1, &ID);
		glBindFramebuffer(GL_FRAMEBUFFER, ID);

		// - position and depth
		glGenTextures(1, &gPositionDepth);
		glBindTexture(GL_TEXTURE_2D, gPositionDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);

		// - normal and metallic
		glGenTextures(1, &gNormalMetallic);
		glBindTexture(GL_TEXTURE_2D, gNormalMetallic);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormalMetallic, 0);

		// - albedo and roughness
		glGenTextures(1, &gAlbedoRoughness);
		glBindTexture(GL_TEXTURE_2D, gAlbedoRoughness);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoRoughness, 0);

		// - 告诉OpenGL我们将要使用(帧缓冲的)哪种颜色附件来进行渲染
		GLuint attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
		glDrawBuffers(3, attachments);

		//深度缓冲
		glGenRenderbuffers(1, &rboDepth);
		glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
		// - Finally check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//render scene's geometry/color data into gbuffer
	void GeometryPass(Scene mainscene, glm::mat4 projection, glm::mat4 view) const {
		glBindFramebuffer(GL_FRAMEBUFFER, ID);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderToGbuffer.use();
		RenderToGbuffer.setMat4("projection", projection);
		RenderToGbuffer.setMat4("view", view);
		mainscene.render(RenderToGbuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	//calculate lighting by iterating over a screen filled quad pixel - by - pixel using the gbuffer's content.
	void LightingPass(Shader shader, Scene scene,Camera camera) const {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		shader.use();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, gPositionDepth);
		shader.setInt("gPositionDepth", 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, gNormalMetallic);
		shader.setInt("gNormalMetallic", 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, gAlbedoRoughness);
		shader.setInt("gAlbedoRoughness", 2);

		// Also send light relevant uniforms
		for (GLuint i = 0; i < scene.PointLightNumber; i++)
		{
			shader.setVec3("lights[" + std::to_string(i) + "].Position", scene.pointlight[i].position);
			shader.setVec3("lights[" + std::to_string(i) + "].Color", scene.pointlight[i].color);

			shader.setFloat("lights[" + std::to_string(i) + "].Linear", scene.pointlight[i].linear);
			shader.setFloat("lights[" + std::to_string(i) + "].Quadratic", scene.pointlight[i].quadratic);
		}
		shader.setVec3("viewPos", camera.Position);
		RenderQuad();
	}
private:

};
Shader GBuffer::RenderToGbuffer=
	Shader();

void RenderQuad()
{
	GLuint quadVAO = 0;
	GLuint quadVBO;
	GLfloat quadVertices[] = {
		// Positions        // Texture Coords
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
	// Setup plane VAO
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}


#endif