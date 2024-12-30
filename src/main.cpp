#include <window.h>
#include <shader.h>
#include <camera.h>
#include <model.h>
#include <skybox.h>
#include <light.h>
#include <shadow.h>
#include <scene.h>

#include <iostream>
#include <vector>
#include <config.h>
#include <defer_render.h>
#include <ssao.h>

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
unsigned int loadTexture(const char* path);

//-----config------------------------------------
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

//-----------------------------------------------
//Bool variable
bool playmode = false;
bool wireframe = false;
bool skyboxenable = false;
bool GammaEnable = false;

int main()
{
    GLFWwindow* window = CreateWindow(SCR_WIDTH, SCR_HEIGHT);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;

    }

    // build and compile shader program
    // ------------------------------------
    Shader test("shader/test.vert", "shader/test.frag");
    //Shader lightingShader("shader/BlingPhong.vert", "shader/BlingPhong.frag");
    //Shader lightingShader_Defer("shader/BlingPhong_Defer.vs","shader/BlingPhong_Defer.fs");
    //Shader PBRshader_Defer("shader/PBR_defer.vert", "shader/PBR_defer.frag");

    // model configuration
    // --------------------
    Model sponza("model/Sponza/sponza.obj");
    
    // scene configuration
    // -------------------- 
    //Scene mainscene(
        //{
        //Object(sponza,glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,0.0f,0.0f), glm::vec3(0.1f))
        //},
        //DirectLight(glm::vec3(1.0f, -3.0f, 1.0f)),
        //{
        //PointLight(glm::vec3(0.5,1.0,0.0),glm::vec3(0.5,0.5,0.0))
        //}
    //);
    Scene::simplelightshader =
        Shader("shader/lightvertexshader.vert", "shader/lightfragment.frag");

    //initialize skybox
    // --------------------
    Skybox skybox("texture/skybox/skybox1/right.jpg",
        "texture/skybox/skybox1/left.jpg",
        "texture/skybox/skybox1/top.jpg",
        "texture/skybox/skybox1/bottom.jpg",
        "texture/skybox/skybox1/front.jpg",
        "texture/skybox/skybox1/back.jpg"
    );
    Skybox::SkyboxShader =
        Shader("shader/skybox.vert", "shader/skybox.frag");

    //shadow map
    //DirLightDepthMap dirlightdepthmap; 
    //DirLightDepthMap::DirLightDepthShader =
        //Shader("shader/DirLightDepthShader.vert", "shader/DirLightDepthShader.frag");
    //DirLightDepthMap::debugDepthQuad =
        //Shader("shader/debugDepthQuad.vert", "shader/debugDepthQuad.frag");

    //PointLightDepthCubeMap pointlightdepthcubemap;
    //PointLightDepthCubeMap::PointLightDepthShader =
        //Shader("shader/PointLightDepthShader.vert", "shader/PointLightDepthShader.fs", "shader/PointLightDepthShader.frag");

    //gbuffer
    //GBuffer gbuffer;
    //GBuffer::RenderToGbuffer =
        //Shader("shader/RenderToGbuffer.vert", "shader/RenderToGbuffer.frag");

    //AO
    //SSAO ssao;
    //SSAO::shaderSSAO =
        //Shader("shader/SSAO.vert", "shader/SSAO.frag");
    //SSAO::shaderSSAOBlur =
        //Shader("shader/SSAOblur.vert", "shader/SSAOblur.frag");

    // configure global opengl state
    // -----------------------------
    glEnable(GL_MULTISAMPLE);//启用MSAA
    //glEnable(GL_FRAMEBUFFER_SRGB);//启用Gamma校正
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    // tick loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // logic loop
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // render loop
        // ------
        //清除深度缓冲、模版缓冲和颜色缓冲
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        //---------------------绘制平行光深度图
        //dirlightdepthmap.RenderSceneToDepthMap(mainscene);

        //----------------------绘制点光源深度图
        //pointlightdepthcubemap.RenderSceneToDepthCubeMap(mainscene,0);

        //------------------------绘制场景
        //pv矩阵
        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

        // don't forget to enable shader before setting uniforms
        test.use();
        test.setMat4("projection", projection);
        test.setMat4("view", view);
        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        test.setMat4("model", model);
        sponza.Draw(test);
        //gbuffer geometry pass
        //gbuffer.GeometryPass(mainscene, projection, view);

        //SSAO
        //ssao.createSSAOtexture(gbuffer, projection);
        //ssao.BlurSSAOTexture();

        //gbuffer lighting pass
        //gbuffer.LightingPass(PBRshader_Defer, mainscene, camera);

        //绘制点光源
        //mainscene.DrawPointLight(projection, view, box, gbuffer.ID);

        //Draw skybox
        //if (skyboxenable == true)
            //skybox.draw(view, projection, camera);

        cout << "render finish" << endl;
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
   
    return 0;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{

    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);

}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}



