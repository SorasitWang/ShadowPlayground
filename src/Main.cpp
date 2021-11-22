#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "./header/shader_m.h"
#include "./header/stb_image.h"
#include "./header/camera.h"
#include <iostream>
#include "./plane/Plane.h"
#include "../Ball.h"
#include "./sphere/Sphere.h"
//#include "../Character.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void initDepthMap(unsigned int& depthFBO, unsigned int& depthMap);
void renderDepth(unsigned int depthFBO, unsigned int depthMap, Shader& depthShader, glm::mat4 projection
    , glm::mat4 view, Shader& wallShader, Shader& rockShader, Shader& planeShader);
void initPosMap(unsigned int& posFBO, unsigned int& posMap);
void initDepthMap(unsigned int& depthFBO, unsigned int& depthMap);
void drawFrame(Shader& shader, unsigned& VAO, glm::mat4 lightProjection, Plane p1, Plane p2);
unsigned int loadTexture(const char* path);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const float SIZE = 6;
const int numObj = 3;
glm::vec3 lightPos(1.0f, 0.6f, 1.0f);
glm::vec3 lightPos2(1.0f, 0.6f, 0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
int winScore = 5;
float x, y;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
bool edit = false;
Camera cam = Camera();
Plane p1 = Plane();
Plane p2 = Plane();
float angle = 0.0;
unsigned int show = 2;
int main()
{

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }




    glEnable(GL_DEPTH_TEST);
    // Shader pointShader("point.vs", "ourShader.fs");
    Shader planeShader("src/plane/plane.vs", "src/plane/plane.fs");
    //Shader planeShader("ourShader.vs", "ourshader.fs");
    Shader simpleDepthShader("src/shader/ourShader.vs", "src/shader/ourshader.fs");
    Shader lightCubeShader("src/shader/lightcube.vs", "src/shader/lightcube.fs");
    Shader shader("src/shader/texture.vs", "src/shader/texture.fs");
    Shader simplePosShader("src/shader/pos.vs", "src/shader/pos.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices1[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO1, VAO1, EBO;
    glGenVertexArrays(1, &VAO1);
    glGenBuffers(1, &VBO1);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    float vertices[] = {
        // back face
  -0.3f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // bottom-left
    0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // bottom-right    
    0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // top-right              
    0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // top-right
   -0.3f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // top-left
   -0.3f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // bottom-left           
   // front face
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f, // bottom-left
    0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f, // top-right
    0.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f, // bottom-right        
    0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f, // top-right
   -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f, // bottom-left
   -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f, // top-left        
   // left face
   -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f,0.0f, // top-right
   -0.5f, -0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // bottom-left
   -0.5f,  0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // top-left       
   -0.5f, -0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // bottom-left
   -0.5f,  0.5f,  0.5f,  -1.0f, 0.0f,0.0f, // top-right
   -0.5f, -0.5f,  0.5f,  -1.0f, 0.0f,0.0f, // bottom-right
     // left face 2
   -0.3f,  0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // top-right
   -0.3f, -0.5f,  -0.5f,  -1.0f, 0.0f,0.0f, // bottom-left
   -0.3f,  0.5f,  -0.5f,  -1.0f, 0.0f,0.0f, // top-left       
   -0.3f, -0.5f,  -0.5f,  -1.0f, 0.0f,0.0f, // bottom-left
   -0.3f,  0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // top-right
   -0.3f, -0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // bottom-right
   // back face 2
    -0.5f, -0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // bottom-left
    -0.3f, -0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // bottom-right    
    -0.3f,  0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // top-right              
    -0.3f,  0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // top-right
   -0.5f,  0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // top-left
   -0.5f, -0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // bottom-left       
   // right face
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f, // top-left
    0.5f,  0.5f, -0.5f,  1.0f, 0.0f,0.0f, // top-right      
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f, // bottom-right          
    0.5f, -0.5f, -0.5f,  1.0f, 0.0f,0.0f, // bottom-right
    0.5f, -0.5f,  0.5f,  1.0f, 0.0f,0.0f, // bottom-left
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f,0.0f, // top-left  

    //top face
     -0.3f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,// top-left
    0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // top-right
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right                 
    0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right
   -0.3f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-left  
   -0.3f,  0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // top-left   

    -0.5f,  0.5f, -0.0f,  0.0f, 1.0f, 0.0f,// top-left
    0.3f,  0.5f, -0.0f,  0.0f, 1.0f, 0.0f, // top-right
    0.3f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right                 
    0.3f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right
   -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-left  
   -0.5f,  0.5f, -0.0f,  0.0f, 1.0f, 0.0f,  // top-left   
    };

    // first, configure the cube's VAO (and VBO)

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO, VBO;
    glGenVertexArrays(1, &lightCubeVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    p1.init(planeShader, -0.5f);
    p2.init(planeShader, -0.2f);
    // s.init(pointShader);

    unsigned int depthMapFBO, depthMap, depthMapFBO2, depthMap2;
    unsigned int posMapFBO, posMap, posMapFBO2, posMap2;

    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture


    initDepthMap(depthMapFBO, depthMap);
    initDepthMap(depthMapFBO2, depthMap2);

    initPosMap(posMapFBO, posMap);
    initPosMap(posMapFBO2, posMap2);

    planeShader.use();

    planeShader.setInt("shadowMapFar", 2);
    planeShader.setInt("shadowMapNear", 1);
    planeShader.setInt("posMapFar", 4);
    planeShader.setInt("posMapNear", 3);
    simpleDepthShader.use();
    simpleDepthShader.setInt("shadowMapNear", 1);
    //star.init(rockShader, wall);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);

    
    float orth = 1.0f;
    float near_plane = 0.001f;
    float far_plane = 10.0f;
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix, lightSpaceMatrix2;
    //float near_plane = 0.1f, far_plane = 7.5f;
    lightProjection = glm::ortho(-orth, orth, -orth, orth, near_plane, far_plane);
    //glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);

    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);


    planeShader.use();
    //p1 + p2
    planeShader.setInt("allPlate[0].num", p1.vertices.size());
    planeShader.setInt("allPlate[1].num", p1.vertices.size());
    for (int i = 0; i < p1.vertices.size(); i +=3) {
        planeShader.setInt("allPlate[0].vertex", p1.vertices[i]);
        planeShader.setInt("allPlate[0].vertex", p1.vertices[i+1]);
        planeShader.setInt("allPlate[0].vertex", p1.vertices[i+2]);
        planeShader.setInt("allPlate[1].vertex", p2.vertices[i]);
        planeShader.setInt("allPlate[1].vertex", p2.vertices[i+1]);
        planeShader.setInt("allPlate[1].vertex", p2.vertices[i+2]);
    }
    planeShader.setInt("allPlate[2].num", 6*8*6);
    planeShader.setInt("allPlate[3].num", 6 * 8 * 6);
    for (int i = 0; i < p1.vertices.size(); i += 1) {
        planeShader.setInt("allPlate[2].vertex", vertices[i]);
        planeShader.setInt("allPlate[2].vertex", vertices[i + 1]);
        planeShader.setInt("allPlate[2].vertex", vertices[i + 2]);
        planeShader.setInt("allPlate[3].vertex", vertices[i]);
        planeShader.setInt("allPlate[3].vertex", vertices[i + 1]);
        planeShader.setInt("allPlate[3].vertex", vertices[i + 2]);
    }

    
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        //s.bounce(deltaTime);
        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        float boxX = 0.345;


        ///////


        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);



       

       

        lightView = glm::lookAt(lightPos2, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix2 = lightProjection * lightView;



        //--------------------- pos ---------------------------------------------//

        glBindFramebuffer(GL_FRAMEBUFFER, posMapFBO);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        simplePosShader.use();
        simplePosShader.setMat4("lightSpaceMatrix", lightSpaceMatrix2);
        glm::mat4 model = glm::mat4(1.0f);
        simplePosShader.setMat4("model", model);
        //simpleDepthShader.setBool("isSecond", true);
        p1.draw(simplePosShader);
        p2.draw(simplePosShader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.2, 0.0, 0.43));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
       
        simplePosShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.4, 0.0, 0.43));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        simplePosShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

      
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, posMap);

        glBindFramebuffer(GL_FRAMEBUFFER, posMapFBO2);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.2, 0.0, 0.43));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));

        simplePosShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        model = glm::mat4(1.0f);
        simplePosShader.setMat4("model", model);
        p1.draw(simplePosShader);
        p2.draw(simplePosShader);

        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, posMap2);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        ///---------shadow------------------

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix2);
        simpleDepthShader.setBool("isSecond", false);
        //simplePosShader.setMat4("lightSpaceMatrix", lightSpaceMatrix2);
        model = glm::mat4(1.0f);
        simpleDepthShader.setMat4("model", model);
        //simpleDepthShader.setBool("isSecond", true);
        p1.draw(simpleDepthShader);
        p2.draw(simpleDepthShader);


        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.2, 0.0, 0.43));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        simpleDepthShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.4, 0.0, 0.43));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        simpleDepthShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        //drawFrame(simpleDepthShader, lightCubeVAO, lightProjection, p1, p2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO2);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        simpleDepthShader.setMat4("model", model);
        simpleDepthShader.setBool("isSecond", true);
        simpleDepthShader.setInt("posMapNear", 1);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.2, 0.0, 0.43));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        simpleDepthShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        model = glm::mat4(1.0f);
        simpleDepthShader.setMat4("model", model);
        p1.draw(simpleDepthShader);
        p2.draw(simpleDepthShader);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap2);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        /*
        
        draw
        
        
        */


        planeShader.use();


        glCullFace(GL_FRONT);

        glm::mat4 view = cam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_HEIGHT / (float)SCR_WIDTH, 0.01f, 100.0f);
        planeShader.use();
        //planeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        planeShader.setInt("numObj", numObj);
        //plane
        
       
        planeShader.setMat4("lightSpaceMatrix2", lightSpaceMatrix2);
        planeShader.setFloat("far_plane", far_plane);
        planeShader.setBool("drawShadow", false);
        planeShader.setMat4("allPlate[0].modelViewProj", model* view* proj);
        planeShader.setMat4("allPlate[1].modelViewProj", model* view* proj);

        p1.draw(planeShader, proj, view, lightPos2, cam);
        p2.draw(planeShader, proj, view, lightPos, cam);

        //Box
        planeShader.use();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(boxX, 0.0, 0.5));
        model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube

        planeShader.setMat4("model", model);
        planeShader.setBool("drawShadow", true);
        planeShader.setVec3("material.diffuse", glm::vec3(0.8, 0.5, 0.3));
        planeShader.setFloat("alpha", 1.0f);



        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.2, 0.0, 0.43));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        planeShader.setMat4("allPlate[2].modelViewProj", model* view* proj);
        
        planeShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.4, 0.0, 0.43));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        planeShader.setMat4("allPlate[3].modelViewProj", model* view* proj);
        planeShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 48);

        //light source
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", proj);
        lightCubeShader.setMat4("view", view);
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.02f));
        lightCubeShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos2);
        model = glm::scale(model, glm::vec3(0.02f));
        lightCubeShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // render show framebuffer
        shader.use();
        shader.setInt("texture1", show);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.7f, 0.7f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8));
        shader.setMat4("model", model);
        glBindVertexArray(VAO1);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        glfwSwapBuffers(window);
        glfwPollEvents();


    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
        show = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
        show = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS)
        show = 3;
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        show = 4;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        angle += 0.01;
    //p1.move(0.01);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        angle -= 0.01;
    //p1.move(-0.01);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cam.ProcessKeyboard(FORWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cam.ProcessKeyboard(BACKWARD, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cam.ProcessKeyboard(LEFT, deltaTime);

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cam.ProcessKeyboard(RIGHT, deltaTime);




}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_TAB && action == GLFW_PRESS)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        //p.move();
        //s.move(2, -0.39);
    }
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
    {

        // s.move(2, 0.0);
    }

}
// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
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
    x = 2 * xpos / SCR_WIDTH - 1;
    y = 2 * (-ypos / SCR_HEIGHT + 0.5);

    //p1.move(yoffset);

    cam.ProcessMouseMovement(xoffset, yoffset);
}


void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    /*if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        std::cout << p1.selected << std::endl;
        if (!p1.selected) {
            glm::mat4 view = cam.GetViewMatrix();// glm::lookAt(glm::vec3(0.0f), cam.Front, cam.Up);

            glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_HEIGHT / (float)SCR_WIDTH, 0.01f, 100.0f);

            float mouseX = lastX / (SCR_WIDTH * 0.5f) - 1.0f;
            float mouseY = lastY / (SCR_HEIGHT * 0.5f) - 1.0f;

            glm::mat4 invVP = glm::inverse(proj * view);
            glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
            glm::vec4 worldPos = invVP * screenPos;

            glm::vec3 dir = glm::normalize(glm::vec3(worldPos.x, worldPos.y, worldPos.z));
            glm::vec3 world = cam.Position + dir * glm::distance(cam.Position, glm::vec3(1.0f, 0.0f, 0.0f));
            float rat = (0 - cam.Position.y) / world.y;
            glm::vec3 pos = cam.Position + world * rat;
            std::cout << "pos " << pos.x << " " << pos.y << " " << pos.z << std::endl;
            std::cout << "dir " << dir.x << " " << dir.y << " " << dir.z << std::endl;
            std::cout << "cam " << cam.Position.x << " " << cam.Position.y << " " << cam.Position.z << std::endl;
            std::pair<int, int> vertex = p1.findClick(cam.Position, dir);
            std::cout << vertex.first << " " << vertex.second << std::endl;
        }
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        p1.selected = false;
    }*/



}
// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    //std::cout << yoffset << std::endl;
    //p1.adaptAreaHill(yoffset);
    //p.move(yoffset);
    //cam.ProcessMouseScroll(yoffset);
}

void initDepthMap(unsigned int& depthFBO, unsigned int& depthMap) {

    glGenFramebuffers(1, &depthFBO);
    //glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SHADOW_WIDTH,
        SHADOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        depthMap, 0);
    //glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void initPosMap(unsigned int& posFBO, unsigned int& posMap) {

    glGenFramebuffers(1, &posFBO);
    //glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glGenTextures(1, &posMap);
    glBindTexture(GL_TEXTURE_2D, posMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SHADOW_WIDTH,
        SHADOW_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, posFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
        posMap, 0);
    //glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo); // now actually attach it
    // now that we actually created the framebuffer and added all attachments we want to check if it is actually complete now
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void drawFrame(Shader& shader, unsigned& VAO, glm::mat4 lightProjection, Plane p1, Plane p2) {

    float boxX = 0.345;
    //glm::mat4 model(1.0f);
    glm::mat4 lightView = glm::lookAt(lightPos2, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix2 = lightProjection * lightView;
    shader.setMat4("lightSpaceMatrix", lightSpaceMatrix2);
    glm::mat4 model = glm::mat4(1.0f);
    shader.setMat4("model", model);
    shader.setBool("isSecond", true);
    p1.draw(shader);
    //p2.draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.5, 0.0, 0.5));
    model = glm::scale(model, glm::vec3(0.4f)); // a smaller cube
    shader.setMat4("model", model);

    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(boxX, 0.0, 0.5));
    model = glm::scale(model, glm::vec3(0.1f)); // a smaller cube
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.2, 0.0, 0.43));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 48);

}