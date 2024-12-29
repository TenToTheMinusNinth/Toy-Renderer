#ifndef SHADOW_H
#define SHADOW_H

#include <glad/glad.h>
#include <glm/ext.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <shader.h>
#include <scene.h>
#include <light.h>
#include <config.h>

const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

class DirLightDepthMap {
public:
    GLuint depthMapFBO;
    GLuint depthMap;
    static Shader DirLightDepthShader;
    static Shader debugDepthQuad;
    DirLightDepthMap(){
        glGenFramebuffers(1, &depthMapFBO);
        //创建一个2D纹理，提供给帧缓冲的深度缓冲使用
        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        //把生成的深度纹理作为帧缓冲的深度缓冲
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);//告诉OpenGL我们不适用任何颜色数据进行渲染
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderSceneToDepthMap(Scene& mainscene) {
        //---------------------绘制平行光深度图
        // 从光源的位置渲染场景到帧缓冲上
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        DirLightDepthShader.use();
        GLfloat near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);//正交投影矩阵(平行光）
        glm::mat4 lightView = glm::lookAt(-mainscene.directlight.direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        DirLightDepthShader.setMat4("lightprojection", lightProjection);
        DirLightDepthShader.setMat4("lightview", lightView);
        mainscene.render(DirLightDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    }

    void DrawDepthMap() {
        debugDepthQuad.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        unsigned int quadVAO;
        unsigned int quadVBO;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        glBindVertexArray(0);
    }

private:
    
};
Shader DirLightDepthMap::DirLightDepthShader =
Shader();
Shader DirLightDepthMap::debugDepthQuad =
Shader();

class PointLightDepthCubeMap {
public:
    GLuint depthCubeMapFBO;
    GLuint depthCubemap;
    static Shader PointLightDepthShader;
    PointLightDepthCubeMap(){
        glGenFramebuffers(1, &depthCubeMapFBO);
        glGenTextures(1, &depthCubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
        for (GLuint i = 0; i < 6; ++i)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        // Attach cubemap as depth map FBO's color buffer
        glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);//绑定到深度缓冲
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void RenderSceneToDepthCubeMap(Scene mainscene,int i){
        // 0. Create depth cubemap transformation matrices
        GLfloat aspect = (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT;
        GLfloat near = 1.0f;
        GLfloat far = 25.0f;
        glm::mat4 shadowProj = glm::perspective(90.0f, aspect, near, far);
        std::vector<glm::mat4> shadowTransforms;

        shadowTransforms.push_back(shadowProj* glm::lookAt(mainscene.pointlight[i].position, mainscene.pointlight[i].position + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj* glm::lookAt(mainscene.pointlight[i].position, mainscene.pointlight[i].position + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj* glm::lookAt(mainscene.pointlight[i].position, mainscene.pointlight[i].position + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
        shadowTransforms.push_back(shadowProj* glm::lookAt(mainscene.pointlight[i].position, mainscene.pointlight[i].position + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
        shadowTransforms.push_back(shadowProj* glm::lookAt(mainscene.pointlight[i].position, mainscene.pointlight[i].position + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
        shadowTransforms.push_back(shadowProj* glm::lookAt(mainscene.pointlight[i].position, mainscene.pointlight[i].position + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)));

        //  Render scene to depth cubemap
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthCubeMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        PointLightDepthShader.use();
        PointLightDepthShader.setFloat("far_plane", far);
        for (GLuint i = 0; i < 6; ++i)
            glUniformMatrix4fv(glGetUniformLocation(PointLightDepthShader.ID, ("shadowMatrices[" + std::to_string(i) + "]").c_str()), 1, GL_FALSE, glm::value_ptr(shadowTransforms[i]));
        PointLightDepthShader.setFloat("far_plane", far);
        PointLightDepthShader.setVec3("lightPos", mainscene.pointlight[i].position);
        mainscene.render(PointLightDepthShader);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    }

private:
    
};
Shader PointLightDepthCubeMap::PointLightDepthShader =
Shader();


#endif

