#ifndef SSAO_H
#define SSAO_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>
#include <config.h>
#include <iostream>

#include <random> 
#include <defer_render.h>
#include <shader.h>

class SSAO {
public:
    static Shader shaderSSAO;
    static Shader shaderSSAOBlur;
    GLuint ssaoFBO, ssaoBlurFBO;
    GLuint ssaoColorBuffer, ssaoColorBufferBlur;
    GLuint noiseTexture;
    std::vector<glm::vec3> ssaoKernel;

    SSAO(){
        //SSAO FBO
        glGenTextures(1, &ssaoColorBuffer);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenFramebuffers(1, &ssaoFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //SSAO blur FBO
        glGenTextures(1, &ssaoColorBufferBlur);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glGenFramebuffers(1, &ssaoBlurFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Sample kernel
        std::default_random_engine generator;//Ĭ�����������
        std::uniform_real_distribution<GLfloat> randomFloats(0.0, 1.0); //������0.0��1.0��Χ�ڵľ��ȷֲ�
        //���ɷ������
        for (GLuint i = 0; i < 64; ++i)
        {   
            //�����ڷ�������ھ��ȷֲ�������
            //randomFloats(generator) * 2.0 - 1.0��Ϊ������-1.0��1.0��Χ�ڵ������
            glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
            sample = glm::normalize(sample);
            sample *= randomFloats(generator);

            //ʹ���ɵ�����������ԭ�㣨�������ڵ�λ�ã�
            GLfloat scale = GLfloat(i) / 64.0;
            scale = lerp(0.1f, 1.0f, scale * scale);
            sample *= scale;
            ssaoKernel.push_back(sample);
        }

        //��������Ե�����������
        //4*4�����������ظ���ƽ������Ļ��
        std::vector<glm::vec3> ssaoNoise;
        for (GLuint i = 0; i < 16; i++)
        {
            glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
            ssaoNoise.push_back(noise);
        }
        glGenTextures(1, &noiseTexture);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }

    void createSSAOtexture(GBuffer gbuffer,glm::mat4 projection) {
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAO.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gbuffer.gPositionDepth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gbuffer.gNormalMetallic);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        shaderSSAO.setInt("gPositionDepth", 0);
        shaderSSAO.setInt("gNormal", 1);
        shaderSSAO.setInt("texNoise", 2);
        // Send kernel + rotation 
        for (GLuint i = 0; i < 64; ++i)
            shaderSSAO.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
        shaderSSAO.setMat4("projection", projection);
        RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void BlurSSAOTexture(){
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        shaderSSAOBlur.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        RenderQuad();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


private:
    GLfloat lerp(GLfloat a, GLfloat b, GLfloat f)
    {
        return a + f * (b - a);
    }

};
Shader SSAO::shaderSSAO=
    Shader();
Shader SSAO::shaderSSAOBlur=
    Shader();
// Also create framebuffer to hold SSAO processing stage 


#endif
