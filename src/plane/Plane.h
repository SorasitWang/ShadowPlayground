
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "./Header/shader_m.h"

class Plane {

public:
    Plane() {
    };
    unsigned int pVAO, pVBO;
    float size = 1.0f, posX = 0.0f, posZ = 0.0f, y = 0.0f, bufY = 0.0f, sensitive = 0.002;
    int areaBase = 1, areaHill =0 ;
    float  buffAreaHill = 2.0f;
    int sub = 10, v = 56;
    std::vector<float> vertices;
    std::pair<int, int> vSelect;
    bool selected = false;


    void init(Shader shader, float yy) {
        y = yy;
        /*float interval = size / sub;
        for (int i = 0; i < sub - 1; i++) {
            for (int j = 0; j < sub-1; j++) {
                //left tri
                //0
                //pos
                vertices.push_back(posX + j * interval); vertices.push_back(y ); vertices.push_back(posZ + i * interval);
                //normal
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //1 --> pos(2 + 5) / 2
                vertices.push_back(posX + (j + 0.5) * interval); vertices.push_back(y); vertices.push_back(posZ + (i + 0.5) * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //2
                vertices.push_back(posX + j * interval); vertices.push_back(y); vertices.push_back(posZ + (i + 1) * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //3
                vertices.push_back(posX + j * interval); vertices.push_back(y); vertices.push_back(posZ + i * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //4 --> pos(2 + 5) / 2
                vertices.push_back(posX + (j + 0.5) * interval); vertices.push_back(y); vertices.push_back(posZ + (i + 0.5) * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //5
                vertices.push_back(posX + (j+1) * interval); vertices.push_back(y ); vertices.push_back(posZ + i * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //6
                vertices.push_back(posX + (j + 1) * interval); vertices.push_back(y); vertices.push_back(posZ + (i + 1) * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //7
                vertices.push_back(posX + (j + 0.5) * interval); vertices.push_back(y); vertices.push_back(posZ + (i + 0.5) * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //8
                vertices.push_back(posX + (j + 1) * interval); vertices.push_back(y); vertices.push_back(posZ + i * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //9
                vertices.push_back(posX + (j + 1) * interval); vertices.push_back(y); vertices.push_back(posZ + (i + 1) * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //10
                vertices.push_back(posX + (j + 0.5) * interval); vertices.push_back(y); vertices.push_back(posZ + (i + 0.5) * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);

                //11
                vertices.push_back(posX + j * interval); vertices.push_back(y); vertices.push_back(posZ + (i + 1) * interval);
                vertices.push_back(0); vertices.push_back(1); vertices.push_back(0);                
            }
        }
        */
        /*for (int i = 0; i < vertices.size(); i += 3) {
            std::cout << vertices[i] << " " << vertices[i + 1] << " " << vertices[i + 2] << std::endl;
        }*/
               
        vertices.push_back(-1.0f); vertices.push_back(y); vertices.push_back(-1.0f); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        vertices.push_back(1.0f); vertices.push_back(y); vertices.push_back(-1.0f); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        vertices.push_back(1.0f); vertices.push_back(y); vertices.push_back(1.0f); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        vertices.push_back(1.0f); vertices.push_back(y); vertices.push_back(1.0f); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        vertices.push_back(-1.0f); vertices.push_back(y); vertices.push_back(1.0f); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);
        vertices.push_back(-1.0f); vertices.push_back(y); vertices.push_back(-1.0f); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);


        glGenVertexArrays(1, &pVAO);
        glGenBuffers(1, &pVBO);
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(pVAO);

        glBindBuffer(GL_ARRAY_BUFFER, pVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

        //moveOne(500,18);
        //move(20);
    }

    void draw(Shader shader) {
        // initialize (if necessary)
        
        shader.use();
        glBindVertexArray(pVAO);

        glBindBuffer(GL_ARRAY_BUFFER, pVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glDrawArrays(GL_TRIANGLES, 0, 6/*pow(sub, 2) * 12*/);
        glBindVertexArray(0);
    
    }

    void draw(Shader shader,glm::mat4 proj,glm::mat4 view,glm::vec3 lightPos,Camera cam) {

        struct properties{
            glm::vec3 ambient = glm::vec3(0.5f,0.2f,0.1f);
            glm::vec3 specular = glm::vec3(0.2f, 0.2f, 0.2f);
            float shininess = 32;
            glm::vec3 diffuse = glm::vec3(1.5f, 1.5f,1.5f);
         
        } property , material;
        property.ambient = glm::vec3(0.9f, 0.9f, 0.9f);
        glBindVertexArray(pVAO);

        glBindBuffer(GL_ARRAY_BUFFER, pVBO);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertices.size(), &vertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        material.ambient = glm::vec3(0.8, 0.5, 0.3);
        material.diffuse = glm::vec3(0.4, 0.8, 0.4);
        shader.use();
        glm::mat4 model(1.0f);
       
       // model = glm::scale(model, glm::vec3(2.0f));
        shader.setMat4("model", model);
        shader.setMat4("projection", proj);
        shader.setMat4("view", view);
        shader.setVec3("viewPos", cam.Position);

        shader.setVec3("lightPos", lightPos);
        shader.setVec3("material.diffuse", material.diffuse);
        shader.setVec3("material.specular", material.specular);
        shader.setFloat("material.shininess", material.shininess);

        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.ambient", property.ambient);
        shader.setVec3("light.specular",property.specular);
        shader.setVec3("light.diffuse", property.diffuse);
       
        shader.setFloat("light.constant", 1.0f);
        shader.setFloat("light.linear", 0.09f);
        shader.setFloat("light.quadratic", 0.032f);

        shader.setFloat("alpha", 1.0f);
        glBindVertexArray(pVAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        glDrawArrays(GL_TRIANGLES, 0, 6/*pow(sub,2)*12*/);
        shader.use();
        shader.setVec3("material.diffuse",glm::vec3(0.0f));
        shader.setVec3("material.ambient", glm::vec3(0.0f));
        shader.setFloat("alpha", 0.5f);
        glLineWidth(2.0f);
        glBindVertexArray(pVAO);
        glDrawArrays(GL_LINE_STRIP, 0, pow(sub, 2) * 12);
    }
    void moveOne(float yOffset, float vertex) {
        if (vertex < 0) return;
        std::pair<int, int> idx = std::make_pair(1, 1);
        //vertices[6 * 36 + 1] += 0.1;/*
        std::vector<std::pair<int, int>> allIdx = convert(vertex);
        //change normal vector
        if (vertex == int(vertex)) {
            for (auto tmp : allIdx) {
                int type = tmp.first;
                int idx = tmp.second;
                //std::cout << idx << std::endl;
                vertices[6 * idx + 1] += yOffset * sensitive;   //posY 

                //calculate newNormal
                glm::vec3 newNormal, v2, v1;
                if (type == 0) {
                    v1 = glm::vec3(vertices[6 * (idx - 1)], vertices[6 * (idx - 1) + 1], vertices[6 * (idx - 1) + 2]);
                    v2 = glm::vec3(vertices[6 * (idx - 2)], vertices[6 * (idx - 2) + 1], vertices[6 * (idx - 2) + 2]);
                }
                else if (type == 1) {
                    v1 = glm::vec3(vertices[6 * (idx - 2)], vertices[6 * (idx - 2) + 1], vertices[6 * (idx - 2) + 2]);
                    v2 = glm::vec3(vertices[6 * (idx - 1)], vertices[6 * (idx - 1) + 1], vertices[6 * (idx - 1) + 2]);
                }
                else if (type == 2) {
                    v2 = glm::vec3(vertices[6 * (idx + 2)], vertices[6 * (idx + 2) + 1], vertices[6 * (idx + 2) + 2]);
                    v1 = glm::vec3(vertices[6 * (idx + 1)], vertices[6 * (idx + 1) + 1], vertices[6 * (idx + 1) + 2]);
                }
                else {
                    v1 = glm::vec3(vertices[6 * (idx + 2)], vertices[6 * (idx + 2) + 1], vertices[6 * (idx + 2) + 2]);
                    v2 = glm::vec3(vertices[6 * (idx + 1)], vertices[6 * (idx + 1) + 1], vertices[6 * (idx + 1) + 2]);
                    //continue;
                }
                v1 = v1 - glm::vec3(vertices[6 * (idx)], vertices[6 * (idx)+1], vertices[6 * (idx)+2]);
                v2 = v2 - glm::vec3(vertices[6 * (idx)], vertices[6 * (idx)+1], vertices[6 * (idx)+2]);
                newNormal = glm::cross(v1, v2);

                //change normal
                if (type == 0 || type == 1) {
                    vertices[6 * (idx - 1) + 3] = newNormal.x; vertices[6 * (idx - 1) + 4] = newNormal.y; vertices[6 * (idx - 1) + 5] = newNormal.z;
                    vertices[6 * (idx - 2) + 3] = newNormal.x; vertices[6 * (idx - 2) + 4] = newNormal.y; vertices[6 * (idx - 2) + 5] = newNormal.z;
                }
                else {
                    vertices[6 * (idx + 2) + 3] = newNormal.x; vertices[6 * (idx + 2) + 4] = newNormal.y; vertices[6 * (idx + 2) + 5] = newNormal.z;
                    vertices[6 * (idx + 1) + 3] = newNormal.x; vertices[6 * (idx + 1) + 4] = newNormal.y; vertices[6 * (idx + 1) + 5] = newNormal.z;
                }

                vertices[6 * (idx)+3] = newNormal.x; vertices[6 * (idx)+4] = newNormal.y; vertices[6 * (idx)+5] = newNormal.z;
                //std::cout << newNormal.x << " " << newNormal.y << " " << newNormal.z << std::endl;
            }
        }
        else {
            for (auto tmp : allIdx) {
                int type = tmp.first;
                int idx = tmp.second;
                //std::cout << idx << std::endl;
                vertices[6 * idx + 1] += yOffset * sensitive;   //posY 

                //calculate newNormal
                glm::vec3 newNormal, v2, v1;
                if (type == 4) {
                    v1 = glm::vec3(vertices[6 * (idx - 1)], vertices[6 * (idx - 1) + 1], vertices[6 * (idx - 1) + 2]);
                    v2 = glm::vec3(vertices[6 * (idx + 1)], vertices[6 * (idx + 1) + 1], vertices[6 * (idx + 1) + 2]);
                }
                else {
                    v2 = glm::vec3(vertices[6 * (idx - 1)], vertices[6 * (idx - 1) + 1], vertices[6 * (idx - 1) + 2]);
                    v1 = glm::vec3(vertices[6 * (idx + 1)], vertices[6 * (idx + 1) + 1], vertices[6 * (idx + 1) + 2]);
                }
                v1 = v1 - glm::vec3(vertices[6 * (idx)], vertices[6 * (idx)+1], vertices[6 * (idx)+2]);
                v2 = v2 - glm::vec3(vertices[6 * (idx)], vertices[6 * (idx)+1], vertices[6 * (idx)+2]);
                newNormal = glm::cross(v1, v2);

                //change normal

                vertices[6 * (idx - 1) + 3] = newNormal.x; vertices[6 * (idx - 1) + 4] = newNormal.y; vertices[6 * (idx - 1) + 5] = newNormal.z;
                vertices[6 * (idx + 1) + 3] = newNormal.x; vertices[6 * (idx + 1) + 4] = newNormal.y; vertices[6 * (idx + 1) + 5] = newNormal.z;

                vertices[6 * (idx)+3] = newNormal.x; vertices[6 * (idx)+4] = newNormal.y; vertices[6 * (idx)+5] = newNormal.z;
                //std::cout << newNormal.x << " " << newNormal.y << " " << newNormal.z << std::endl;
            }
        }

    }

    void move(float yOffset) {

        if (!selected) return;
        //find the area --> vertex to move , height?
        // v = anchor / (sub-1)
        int row = v / sub;
        int col = v % sub;
        float vertex;

        vertex = rowToV(row, col);
        //std::cout << vertex << std::endl;
       /* moveOne(yOffset, vertex);
        moveOne(yOffset, vertex + 0.5);*/
        //moveOne(yOffset, vertex - 0.5);
        for (int i = -areaHill; i <= areaHill; i++) {
            for (int j = -areaHill; j <= areaHill;j++) {

                vertex = rowToV(row + i, col + j);
                moveOne(yOffset, vertex);
                if (i != areaHill && j!=areaHill)
                moveOne(yOffset, vertex + 0.5);

                if (i == areaHill && j == areaHill)moveOne(yOffset / 2, vertex + 0.5);
                else if (i == -areaHill && j == -areaHill) {
                    moveOne(yOffset / 2, vertex - sub - 0.5);
                    moveOne(yOffset / 2, vertex - sub + 0.5);
                    moveOne(yOffset / 2, vertex - 0.5);
                }
                else if (i == -areaHill && j == areaHill) {
                    moveOne(yOffset / 2, vertex + 0.5);
                    moveOne(yOffset / 2, vertex - sub + 0.5);
                }
                else if (i == areaHill && j == -areaHill) {
                    moveOne(yOffset / 2, vertex + 0.5);
                    moveOne(yOffset / 2, vertex - 0.5);
                }
                else if (j == -areaHill)moveOne(yOffset / 2, vertex - 0.5);
                else if (i == areaHill)moveOne(yOffset / 2, vertex + 0.5);
                else if (i == -areaHill)moveOne(yOffset / 2, vertex - sub + 0.5);
                else if (j == areaHill)moveOne(yOffset / 2, vertex + 0.5);
             
               /* vertex = rowToV(row + i, col);
                moveOne(yOffset, vertex);
                if (i == areaHill)moveOne(yOffset / 2, vertex + 0.5);
                vertex = rowToV(row - i, col);
                moveOne(yOffset, vertex);
                moveOne(yOffset, vertex + 0.5);
                if (i == areaHill)moveOne(yOffset / 2, vertex - sub + 0.5);

                vertex = rowToV(row + i, col + i);
                moveOne(yOffset, vertex);
                if (i == areaHill)moveOne(yOffset / 2, vertex + 0.5);
                vertex = rowToV(row, col + i);
                moveOne(yOffset, vertex);
                if (i == areaHill)moveOne(yOffset / 2, vertex + 0.5);
                vertex = rowToV(row - i, col + i);
                moveOne(yOffset, vertex);
                if (i == areaHill)moveOne(yOffset / 2, vertex + 0.5);
                if (i == areaHill)moveOne(yOffset / 2, vertex - sub + 0.5);

                vertex = rowToV(row + i, col - i);
                moveOne(yOffset, vertex);
                if (i == areaHill)moveOne(yOffset / 2, vertex + 0.5);
                if (i == areaHill)moveOne(yOffset / 2, vertex - 0.5);
                vertex = rowToV(row, col - i);
                moveOne(yOffset, vertex);
                moveOne(yOffset, vertex + 0.5);
                if (i == areaHill)moveOne(yOffset / 2, vertex - 0.5);
                vertex = rowToV(row - i, col - i);
                moveOne(yOffset, vertex);
                moveOne(yOffset, vertex + 0.5);
                if (i == areaHill)moveOne(yOffset / 2, vertex - 0.5);
                if (i == areaHill)moveOne(yOffset / 2, vertex - sub - 0.5);
                if (i == areaHill)moveOne(yOffset / 2, vertex - sub + 0.5);*/
            }
        }
        


    }

    void adaptAreaHill(float offset) {
        //std::cout << offset << std::endl;
        std::cout << buffAreaHill << std::endl;
        buffAreaHill += offset * 40*sensitive;
        areaHill = std::max(0,int(buffAreaHill));
    }

    std::pair<int,int> findClick(glm::vec3 camPos,glm::vec3 dir) {
        glm::vec3 tmpPos ;
        float interval = size / sub;
        
        selected = false;
        float min=1000,tmp,offset = 0.07;
        for (int i = 0; i < sub; i++) {
            for (int j = 0; j < sub; j++) {
                tmpPos = glm::vec3(i*interval, 0.0f, j*interval);
                tmp = glm::distance(camPos + dir * glm::distance(camPos, tmpPos), tmpPos);
                if (tmp < min && tmp < offset) {
                    min = tmp;
                    selected = true;
                    v = rowToV(i,j);
                }
            }

        }
        return vSelect;
        
    }
    private :
        std::vector<std::pair<int,int>> convert(float x) {
            std::vector<std::pair<int,int>> re;
            int idx = int(x);
            int row = idx / sub;
            int col = idx % sub;
            int anchor = 12 * (sub - 1) * row + col * 12;
            //std::cout << x << " " << anchor << std::endl;
            if (x == int(x)) {
               
                //std::cout << row << "/" << col << " " <<anchor<< std::endl;
                if (row != sub - 1) {
                    if (col != 0) {
                        re.push_back(std::make_pair(0, anchor - 4));  //4
                        re.push_back(std::make_pair(1, anchor - 7));  //5
                    }
                    if (col != sub - 1) {
                        re.push_back(std::make_pair(3, anchor));  //2
                        re.push_back(std::make_pair(2, anchor + 3));  //3
                    }
                }
                if (row != 0) {
                    if (col != 0) {
                        re.push_back(std::make_pair(2, anchor - 12 * (sub - 1) - 3));//6
                        re.push_back(std::make_pair(3, anchor - 12 * (sub - 1) - 6));//7

                    }
                    if (col != sub - 1) {
                        re.push_back(std::make_pair(0, anchor - 12 * (sub - 1) + 2)); //0
                        re.push_back(std::make_pair(1, anchor - 12 * (sub - 1) + 11)); //1
                    }
                }
            }
            else {
                re.push_back(std::make_pair(4, anchor + 1));  //4
                re.push_back(std::make_pair(5, anchor + 4));  //5
                re.push_back(std::make_pair(4, anchor + 7));  //4
                re.push_back(std::make_pair(5, anchor + 10));  //5
                   
                
            }
            
            
            return re;

            

        }
    
        float rowToV(int row,int col) {
            return (row * sub) + col;
        }

       
        
        
};