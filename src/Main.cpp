#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION


#include "./header/imgui/imgui.h"
#include "./header/imgui/imgui_impl_glfw.h"
#include "./header/imgui/imgui_impl_opengl3.h"
#include <stdio.h>

#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
// About Desktop OpenGL function loaders:
//  Modern desktop OpenGL doesn't have a standard portable header file to load OpenGL function pointers.
//  Helper libraries are often used for this purpose! Here we are supporting a few common ones (gl3w, glew, glad).
//  You may use another loader/header of your choice (glext, glLoadGen, etc.), or chose to manually implement your own.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
#include "./header/imgui/imgui_impl_glfw.h"
#include "./header/model.h"
#include "./header/shader_m.h"
#include "./header/stb_image.h"
#include "./header/camera.h"
#include <iostream>
#include "./plane/Plane.h"
#include "../Ball.h"
#include "./sphere/Sphere.h"
#include <string>
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
void addVertices(Shader& shader,Ball b);
unsigned int loadTexture(const char* path);
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;
const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
const float SIZE = 6;
const int numObj = 3;
glm::vec3 lightPos(1.0f, 1.6f, 1.0f);
glm::vec3 lightPos2(1.5f, 0.1f, 0.0f);
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
float x, y;
// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
bool edit = false;
Camera cam = Camera();
Plane p1 = Plane();
Plane p2 = Plane();
Plane p3 = Plane();

float angle = 0.0;
int depthMode = 1;
float setBias = 0.0f;
unsigned int show = 1;

float vertices[360]= {
    // back face
-0.3f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // bottom-left
  0.5f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // bottom-right    
  0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // top-right              
  0.5f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // top-right
 -0.3f,  0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // top-left
 -0.3f, -0.5f, -0.5f,  0.0f, 0.0f,-1.0f, // bottom-left           
 // front face
 -1.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f, // bottom-left
  0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f, // top-right
  0.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f, // bottom-right        
  0.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f, // top-right
 -1.5f, -0.5f,  0.5f,  0.0f, 0.0f,1.0f, // bottom-left
 -1.5f,  0.5f,  0.5f,  0.0f, 0.0f,1.0f, // top-left        
 // left face
 -1.5f,  0.5f,  0.5f,  -1.0f, 0.0f,0.0f, // top-right
 -1.5f, -0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // bottom-left
 -1.5f,  0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // top-left       
 -1.5f, -0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // bottom-left
 -1.5f,  0.5f,  0.5f,  -1.0f, 0.0f,0.0f, // top-right
 -1.5f, -0.5f,  0.5f,  -1.0f, 0.0f,0.0f, // bottom-right
   // left face 2
 -0.3f,  0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // top-right
 -0.3f, -0.5f,  -0.5f,  -1.0f, 0.0f,0.0f, // bottom-left
 -0.3f,  0.5f,  -0.5f,  -1.0f, 0.0f,0.0f, // top-left       
 -0.3f, -0.5f,  -0.5f,  -1.0f, 0.0f,0.0f, // bottom-left
 -0.3f,  0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // top-right
 -0.3f, -0.5f,  0.0f,  -1.0f, 0.0f,0.0f, // bottom-right
 // back face 2
  -1.5f, -0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // bottom-left
  -0.3f, -0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // bottom-right    
  -0.3f,  0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // top-right              
  -0.3f,  0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // top-right
 -1.5f,  0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // top-left
 -1.5f, -0.5f, -0.0f,  0.0f, 0.0f,-1.0f, // bottom-left       
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

  -1.5f,  0.5f, -0.0f,  0.0f, 1.0f, 0.0f,// top-left
  0.3f,  0.5f, -0.0f,  0.0f, 1.0f, 0.0f, // top-right
  0.3f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right                 
  0.3f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right
 -1.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-left  
 -1.5f,  0.5f, -0.0f,  0.0f, 1.0f, 0.0f,  // top-left   

  //bottomface
  0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right 
  0.5f,  -0.5f, -0.5f,  0.0f, 1.0f, 0.0f, // top-right
  -0.3f,  -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,// top-left
  
      
  -0.3f,  -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  // top-left   
  -0.3f,  -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-left  
  0.5f,  -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right
 
 
  0.3f,  -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right  
  0.3f,  -0.5f, -0.0f,  0.0f, 1.0f, 0.0f, // top-right
  -1.5f, - 0.5f, -0.0f,  0.0f, 1.0f, 0.0f,// top-left

   
  -1.5f, -0.5f, -0.0f,  0.0f, 1.0f, 0.0f,  // top-left   
  -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-left  
  0.3f,  -0.5f,  0.5f,  0.0f, 1.0f, 0.0f, // bottom-right
 
 
};;


bool samePlane(float x1, float y1, float z1, float x2, float y2, float z2,
    float x3, float y3, float z3, float x, float y, float z)
{
    float error = 0.0001;
    float a1 = x2 - x1;
    float b1 = y2 - y1;
    float c1 = z2 - z1;
    float a2 = x3 - x1;
    float b2 = y3 - y1;
    float c2 = z3 - z1;
    float a = b1 * c2 - b2 * c1;
    float b = a2 * c1 - a1 * c2;
    float c = a1 * b2 - b1 * a2;
    float d = (-a * x1 - b * y1 - c * z1);

    // equation of plane is: a*x + b*y + c*z = 0 #

    // checking if the 4th point satisfies
    // the above equation
    //if (a==0 && b==0 && c==0) return true;
    if (abs(a * x + b * y + c * z + d) == 0.0)
        return true;
    return false;
}

int main()
{
    const char* glsl_version = "#version 130";
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
    Shader modelShader("src/shader/model.vs", "src/plane/plane.fs");
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

    Ball b = Ball();

    p1.init(planeShader, -0.5f, true);
    p2.init(planeShader, -0.7f, true);
    p3.init(planeShader, -0.5f, true);
    b.init(modelShader);
    // s.init(pointShader);

    unsigned int depthMapFBO, depthMap, depthMapFBO2, depthMap2, depthMapFBO3, depthMap3;
    unsigned int posMapFBO, posMap, posMapFBO2, posMap2;

    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture


    initDepthMap(depthMapFBO, depthMap);
    initDepthMap(depthMapFBO2, depthMap2);
    initDepthMap(depthMapFBO3, depthMap3);

    initPosMap(posMapFBO, posMap);
    initPosMap(posMapFBO2, posMap2);

    planeShader.use();

    planeShader.setInt("shadowMapFar", 3);
    planeShader.setInt("shadowMapBack", 2);
    planeShader.setInt("shadowMapNear", 1);
    planeShader.setInt("posMapFar", 5);
    planeShader.setInt("posMapNear", 4);
    simpleDepthShader.use();
    simpleDepthShader.setInt("shadowMapNear", 1);
    modelShader.use();

    modelShader.setInt("shadowMapFar", 3);
    modelShader.setInt("shadowMapBack", 2);
    modelShader.setInt("shadowMapNear", 1);
    modelShader.setInt("posMapFar", 5);
    modelShader.setInt("posMapNear", 4);
    //star.init(rockShader, wall);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glFrontFace(GL_CCW);


    float orth = 2.0f;
    float near_plane = 0.1f;
    float far_plane = 15.0f;
    glm::mat4 lightProjection, lightView;
    glm::mat4 lightSpaceMatrix, lightSpaceMatrix2;
    //float near_plane = 0.1f, far_plane = 7.5f;
    lightProjection = glm::ortho(-orth, orth, -orth, orth, near_plane, far_plane);
    //glm::perspective(glm::radians(45.0f), (GLfloat)SHADOW_WIDTH / (GLfloat)SHADOW_HEIGHT, near_plane, far_plane);

    simpleDepthShader.use();
    simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);


    glm::mat4 planeModel(1.0f);
    planeModel = glm::translate(planeModel, glm::vec3(angle, 0.0, 0.0));
    planeModel = glm::scale(planeModel, glm::vec3(0.2f, 2.0f, 9.0f));
    planeModel = glm::rotate(planeModel, glm::radians(90.0f), glm::vec3(0.0, 0.0, -1.0));
    //planeModel = glm::rotate(planeModel, glm::radians(30.0f), glm::vec3(1.0, 1.0, 0.0));
    //planeModel = glm::translate(planeModel, glm::vec3(0.8, 0.0, 0.0));
    glm::mat4 pieceModel(1.0f);
    pieceModel = glm::translate(pieceModel, glm::vec3(0.3, 0.015, -0.9));
    pieceModel = glm::scale(pieceModel, glm::vec3(0.02f, 0.5f, 0.5f));
    pieceModel = glm::rotate(pieceModel, glm::radians(90.0f), glm::vec3(0.0, 0.0, -1.0));

    glm::mat4 groundModel(1.0f);
    groundModel = glm::translate(groundModel, glm::vec3(-0.3, -0.735, -0.7));
    groundModel = glm::scale(groundModel, glm::vec3(2.5f, 1.0f, 2.0f));
    groundModel = glm::rotate(groundModel, glm::radians(00.0f), glm::vec3(0.0, 0.0, -1.0));

    //pieceModel = glm::rotate(pieceModel, glm::radians(50.0f), glm::vec3(1.0, 1.0, 0.0));
    glm::mat4 ballModel(1.0f);
    ballModel = glm::translate(ballModel, glm::vec3(0.3, 0.0, 0.0));
    ballModel = glm::rotate(ballModel, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ballModel = glm::scale(ballModel, glm::vec3(0.1, 0.1, 0.1));

    glm::mat4 boxModel = glm::mat4(1.0f);
   
    


    addVertices(planeShader,b);
    addVertices(modelShader, b);
    //planeShader.setInt("allObj[4].num", b.X_SEGMENTS* b.Y_SEGMENTS * 6);
    /*planeShader.setInt("test.num", b.X_SEGMENTS* b.Y_SEGMENTS * 6);
    int idx = 0,c=0;
    for (int i = 0; i < b.X_SEGMENTS * b.Y_SEGMENTS; i+=1) {
        for (int j = 0; j < 6; j++) {
            idx = 3 * b.sphereIndices[6*i + j];
            //planeShader.setVec3(std::string("allObj[4].vertex[") + std::to_string(i / 6) + std::string("]"),
                //glm::vec3(b.sphereVertices[idx], b.sphereVertices[idx + 1], b.sphereVertices[idx + 2]));
            planeShader.setVec3(std::string("test.vertex[") + std::to_string(c) + std::string("]"),
                glm::vec3(b.sphereVertices[idx], b.sphereVertices[idx + 1], b.sphereVertices[idx + 2]));
            c += 1;
        }

    }*/
  

   
    //sboxModel = glm::rotate(boxModel, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    modelShader.use();
    modelShader.setBool("isModel", true);
    
    const char* showMode[] = { "Normal use bias" , "Back face" ,"Middepth front-back face" , "Middepth 1st-2nd front face" , "Position front" , "Positon back" };
   
    /*for (int i = 0; i < showV.size(); i += 3) {
       std::cout << showV[i] << " " << showV[i+1] << " " << showV[i+2] << " " << std::endl;
    }*/
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


        boxModel = glm::mat4(1.0f);
        boxModel = glm::translate(boxModel, glm::vec3(-0.5, 0.0, 0.0));
        boxModel = glm::rotate(boxModel, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        boxModel = glm::scale(boxModel, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube


        planeModel = glm::translate(glm::mat4(1.0f), glm::vec3(angle, 0.0, 0.0));
        planeModel = glm::scale(planeModel, glm::vec3(0.2f, 2.0f, 9.0f));
        planeModel = glm::rotate(planeModel, glm::radians(90.0f), glm::vec3(0.0, 0.0, -1.0));


        lightView = glm::lookAt(lightPos2, glm::vec3(0, 0, 0), glm::vec3(0.0f, 1.0f, 0.0f));
        lightSpaceMatrix2 = lightProjection * lightView;



        //--------------------- pos ---------------------------------------------//
        glEnable(GL_CULL_FACE);
        glBindFramebuffer(GL_FRAMEBUFFER, posMapFBO);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        simplePosShader.use();
        simplePosShader.setMat4("lightSpaceMatrix", lightSpaceMatrix2);
       

        simplePosShader.setMat4("model", planeModel);
        //simpleDepthShader.setBool("isSecond", true);
        p1.draw(simplePosShader);
        simplePosShader.setMat4("model", pieceModel);
        p2.draw(simplePosShader);
        simplePosShader.setMat4("model", groundModel);
        p3.draw(simplePosShader);
        simplePosShader.setMat4("model", ballModel);
        b.draw(simplePosShader);
        simplePosShader.setMat4("model", boxModel);
        glBindVertexArray(lightCubeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 60);

        
        //model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 60);


        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_2D, posMap);

        glBindFramebuffer(GL_FRAMEBUFFER, posMapFBO2);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 60);

     


        simplePosShader.setMat4("model", planeModel);
        p1.draw(simplePosShader);
        simplePosShader.setMat4("model", pieceModel);
        p2.draw(simplePosShader);
        simplePosShader.setMat4("model", groundModel);
        p3.draw(simplePosShader);
        simplePosShader.setMat4("model", ballModel);
        b.draw(simplePosShader);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, posMap2);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        ///---------shadow------------------

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix2);
        //simplePosShader.setMat4("lightSpaceMatrix", lightSpaceMatrix2);


        simpleDepthShader.setMat4("model", planeModel);
        simpleDepthShader.setBool("isSecond", false);
        p1.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", pieceModel);
        p2.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", groundModel);
        p3.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", ballModel);
        b.draw(simpleDepthShader);


     
        simpleDepthShader.setMat4("model", boxModel);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 60);

        //drawFrame(simpleDepthShader, lightCubeVAO, lightProjection, p1, p2);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);

        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO2);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_BACK);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        simpleDepthShader.setMat4("model", boxModel);

        simpleDepthShader.setInt("posMapNear", 4);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 60);

      

        simpleDepthShader.setMat4("model", planeModel);
        p1.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", pieceModel);
        p2.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", groundModel);
        p3.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", ballModel);
        b.draw(simpleDepthShader);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, depthMap2);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO3);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        simpleDepthShader.setMat4("model", boxModel);
        simpleDepthShader.setBool("isSecond", true);

        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 60);


        simpleDepthShader.setMat4("model", planeModel);
        p1.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", pieceModel);
        p2.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", groundModel);
        p3.draw(simpleDepthShader);
        simpleDepthShader.setMat4("model", ballModel);
        b.draw(simpleDepthShader);

        simpleDepthShader.setMat4("model", boxModel);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 60);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, depthMap3);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);


        // reset viewport
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        /*

        draw


        */




        glDisable(GL_CULL_FACE);
        //glCullFace(GL_FRONT);

        glm::mat4 view = cam.GetViewMatrix();
        glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)SCR_HEIGHT / (float)SCR_WIDTH, 0.01f, 100.0f);
        planeShader.use();
        //planeShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        planeShader.setInt("numObj", numObj);
        planeShader.setInt("depthMode", show);
        planeShader.setFloat("setBias", setBias);
        planeShader.setBool("useNormal", true);
        //plane


        planeShader.setMat4("model", planeModel);

        planeShader.setMat4("lightSpaceMatrix2", lightSpaceMatrix2);
        planeShader.setFloat("far_plane", far_plane);
        planeShader.setBool("drawShadow", false);
        planeShader.setMat4("allObj[0].model", planeModel);
        planeShader.setMat4("allObj[1].model", pieceModel);
        planeShader.setMat4("allObj[2].model", groundModel);
        planeShader.setMat4("test.model", ballModel);
        //planeShader.setMat4("allObj[4].model", ballModel);
        p1.draw(planeShader, proj, view, lightPos2, cam, glm::vec3(0.4, 0.8, 0.4));
        planeShader.setMat4("model", pieceModel);
        p2.draw(planeShader, proj, view, lightPos2, cam, glm::vec3(0.8, 0.4, 0.4));
        planeShader.setMat4("model", groundModel);
        p3.draw(planeShader, proj, view, lightPos2, cam, glm::vec3(0.4, 0.4, 0.8));

        modelShader.use();
        modelShader.setInt("numObj", numObj);
        modelShader.setInt("depthMode", show);
        modelShader.setFloat("setBias", setBias);
        modelShader.setBool("useNormal", true);
        modelShader.setMat4("lightSpaceMatrix2", lightSpaceMatrix2);
        modelShader.setMat4("allObj[0].model", planeModel);
        modelShader.setMat4("allObj[1].model", pieceModel);
        modelShader.setMat4("allObj[2].model", groundModel);
        modelShader.setMat4("allObj[3].model", boxModel);
        modelShader.setMat4("test.model", ballModel);
        modelShader.setBool("isModel", false);

        //plane

        modelShader.setMat4("model", ballModel);
        b.draw(modelShader, proj, view, lightPos2, cam);

        //Box
        planeShader.use();

        planeShader.setBool("drawShadow", true);
        planeShader.setVec3("material.diffuse", glm::vec3(0.8, 0.5, 0.3));
        planeShader.setFloat("alpha", 1.0f);



 
        planeShader.setMat4("allObj[3].model", boxModel);
        planeShader.setMat4("modelObj", glm::mat4(0.0f));
        planeShader.setMat4("model", boxModel);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 60);

        //light source
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", proj);
        lightCubeShader.setMat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.02f));

        lightCubeShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        //glDrawArrays(GL_TRIANGLES, 0, 36);


        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos2);
        model = glm::scale(model, glm::vec3(0.01));
        lightCubeShader.setMat4("model", model);
        glBindVertexArray(lightCubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        // render show framebuffer
        shader.use();
        shader.setInt("texture1", show);
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-0.6f, 0.7f, 0.0f));
        model = glm::scale(model, glm::vec3(0.8));
        shader.setMat4("model", model);
        glBindVertexArray(VAO1);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        bool d = false;
        float bias = 0.0f;
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Settings!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("Movement Property");
            ImGui::Text("TAB to change Edit Mode");
            ImGui::Button(showMode[show - 1]);

            //ImGui::Checkbox("Open Edit mode", &_e);// Edit bools storing our window open/close state
            //ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("Bias", &setBias, 0.0f, 0.5f);
            // Display some text (you can use a format strings too)



            ImGui::End();
        }
        ImGui::Render();
        int display_w, display_h;
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


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
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        show = 5;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (show <= 4) depthMode = show;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        setBias += 0.0001;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        angle += 0.005;
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        angle -= 0.005;
    //p1.move(0.01);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        setBias -= 0.0001;
        //angle -= 0.01;
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

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
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
    // a smaller cube
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    //glDrawArrays(GL_TRIANGLES, 0, 36);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.2, 0.0, 0.43));
    model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.9f)); // a smaller cube
    model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0, 0.0, -1.0));
    shader.setMat4("model", model);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 60);

}

void addVertices(Shader& shader,Ball b) {

    shader.use();
    //p1 + p2
    shader.setInt("xxx", 3);
    shader.setBool("isModel", false);
    shader.setInt("allObj[0].num", p1.ver.size() / 6);
    shader.setInt("allObj[1].num", p1.ver.size() / 6);
    shader.setInt("allObj[2].num", p1.ver.size() / 6);

    for (int i = 0; i < p1.ver.size(); i += 6) {
        shader.setVec3(std::string("allObj[0].vertex[") + std::to_string(i / 6) + std::string("]"), glm::vec3(p1.ver[i], p1.ver[i + 1], p1.ver[i + 2]));
        shader.setVec3(std::string("allObj[1].vertex[") + std::to_string(i / 6) + std::string("]"), glm::vec3(p1.ver[i], p1.ver[i + 1], p1.ver[i + 2]));
        shader.setVec3(std::string("allObj[2].vertex[") + std::to_string(i / 6) + std::string("]"), glm::vec3(p1.ver[i], p1.ver[i + 1], p1.ver[i + 2]));
        // planeShader.setVec3(std::string("allObj[4].vertex[") + std::to_string(i / 6) + std::string("]"), glm::vec3(p1.ver[i], p1.ver[i + 1], p1.ver[i + 2]));

    }

    shader.setInt("allObj[3].num", 6 * 8);
    for (int i = 0; i < 6 * 8 * 6; i += 6) {
        shader.setVec3(std::string("allObj[3].vertex[") + std::to_string(i / 6)
            + std::string("]"), glm::vec3(vertices[i], vertices[i + 1], vertices[i + 2]));
    }

    auto modelV = b.getAllVertices();
    std::cout <<"modelV" << modelV.size() << std::endl;
    shader.setInt("test.num", modelV.size() / 3);

    for (int i = 0; i < modelV.size(); i += 3) {
        shader.setVec3(std::string("test.vertex[") + std::to_string(i / 3)
            + std::string("]"), glm::vec3(modelV[i], modelV[i + 1], modelV[i + 2]));
        //glm::vec4 tmp = ballModel * glm::vec4(modelV[i], modelV[i + 1], modelV[i + 2], 1.0);
        //std::cout << modelV.size() << " " << tmp.x << " " << tmp.y << " " << tmp.z << " " << std::endl;
    }
}