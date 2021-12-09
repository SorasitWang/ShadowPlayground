#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm/glm.hpp>
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "../src/header/shader_m.h"
#include "../src/header/stb_image.h"
#include "./src/header/camera.h"
#include <iostream>
#include <string>
#include <time.h>

struct ballProperty {
	float velocityLostWall = 0.05f;
	float g = 0.01f;
	float velocityLostBump = 0.0f;
	float lifeTime = 120.0f;
	float friction = 0.01f;
};
struct ColProperty {
	glm::vec3 pos = glm::vec3(4.0, 0.0, 0.0);
	glm::vec3 rotate = glm::vec3(0.0, 0.0, 0.0);
	float countFade = 0.0f;
	bool col = false;
	int axis = -1;
};

const GLfloat PI = 3.14159265358979323846f;
class Ball {
public:
	Ball() {
		//Model ourModel("C:\\Users\\LEGION\\source\\repos\\ShadowPlayground\\Res\\backpack.obj");
		this->radius = 0.5;
	};
	const int Y_SEGMENTS = 10;
	const int X_SEGMENTS = 10;
	unsigned int VAO, VBO, EBO;
	float radius = 0.1;
	std::vector<float> sphereVertices;
	std::vector<int> sphereIndices;
	
	Model ourModel = Model("C:\\Users\\LEGION\\source\\repos\\ShadowPlayground\\Res\\rock.obj");

	void init(Shader shader) {
		
		ourModel.setTexture(shader);
		ourModel.Draw(shader);
		
		/*2-Calculate the vertices of the sphere*/
		//Generate the vertices of the ball
		for (int y = 0; y <= Y_SEGMENTS; y++)
		{
			for (int x = 0; x <= X_SEGMENTS; x++)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				sphereVertices.push_back(xPos);
				sphereVertices.push_back(yPos);
				sphereVertices.push_back(zPos);
			}
		}

		//Indices that generate the ball
		for (int i = 0; i < Y_SEGMENTS; i++)
		{
			for (int j = 0; j < X_SEGMENTS; j++)
			{
				sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
				sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j);
				sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);

				sphereIndices.push_back(i * (X_SEGMENTS + 1) + j);
				sphereIndices.push_back((i + 1) * (X_SEGMENTS + 1) + j + 1);
				sphereIndices.push_back(i * (X_SEGMENTS + 1) + j + 1);
			}
		}
		//setRand(balls);

		glGenVertexArrays(1, &this->VAO);
		glGenBuffers(1, &this->VBO);
		glGenBuffers(1, &this->EBO);

		glBindVertexArray(this->VAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), &sphereVertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphereIndices.size() * sizeof(int), &sphereIndices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		




		/*float colVertices[360 / 5 * 3 + 3];
		int colIndex[73 * 3];
		int idx = 3;
		colVertices[0] = 0.0f; colVertices[1] = 0.0f; colVertices[2] = 0.0f;
		for (int i = 0; i < 360; i += 5) {
			colVertices[idx] = radius * glm::cos(glm::radians((float)i)); colVertices[idx + 1] = radius * glm::sin(glm::radians((float)i)); colVertices[idx + 2] = 0.0f;
			idx += 3;
		}
		idx = 0;
		for (int i = 1; i <= 72; i++) {
			colIndex[idx] = 0; colIndex[idx + 1] = i; colIndex[idx + 2] = (i + 1) > 72 ? 1 : i + 1;
			idx += 3;
		}

		glGenVertexArrays(1, &this->colVAO);
		glGenBuffers(1, &this->colVBO);
		glGenBuffers(1, &this->colEBO);


		glBindVertexArray(this->colVAO);

		glBindBuffer(GL_ARRAY_BUFFER, this->colVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(colVertices), colVertices, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->colEBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(colIndex), colIndex, GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);*/


	}


	/*void draw(Shader shader) {
		// initialize (if necessary)

		shader.use();
		glBindVertexArray(pVAO);

		glBindBuffer(GL_ARRAY_BUFFER, pVBO);
		glBufferData(GL_ARRAY_BUFFER, 4 * vertices.size(), &vertices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, 6/*pow(sub, 2) * 12);
		glBindVertexArray(0);

	}*/
	void draw(Shader shader) {
		// initialize (if necessary)
		//glDisable(GL_CULL_FACE);
		glFrontFace(GL_CW);
		shader.use();
		ourModel.Draw(shader);
		glFrontFace(GL_CCW);
		//glEnable(GL_CULL_FACE);
		/*glBindVertexArray(this->VAO);

		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);*/

	}

	void draw(Shader shader, glm::mat4 projection, glm::mat4 view, glm::vec3 lightPos,Camera cam) {
		
		glDisable(GL_CULL_FACE);
		struct properties {
			glm::vec3 ambient = glm::vec3(0.5f, 0.2f, 0.1f);
			glm::vec3 specular = glm::vec3(0.2f, 0.2f, 0.2f);
			float shininess = 32;
			glm::vec3 diffuse = glm::vec3(1.5f, 1.5f, 1.5f);

		} property, material;
		property.ambient = glm::vec3(0.9f, 0.9f, 0.9f);
		material.ambient = glm::vec3(0.8, 0.5, 0.3);
		material.diffuse = glm::vec3(0.4, 0.8, 0.4);
		shader.use();
		shader.setBool("useNormal", true);
		

		shader.setMat4("projection", projection);
		shader.setMat4("view", view);


		shader.setVec3("viewPos", cam.Position);
		//shader.setMat4("model", model);
		shader.setVec3("lightPos", lightPos);
		shader.setVec3("material.diffuse", material.diffuse);
		shader.setVec3("material.specular", material.specular);
		shader.setFloat("material.shininess", material.shininess);

		shader.setVec3("light.position", lightPos);
		shader.setVec3("light.ambient", property.ambient);
		shader.setVec3("light.specular", property.specular);
		shader.setVec3("light.diffuse", property.diffuse);

		shader.setFloat("light.constant", 1.0f);
		shader.setFloat("light.linear", 0.09f);
		shader.setFloat("light.quadratic", 0.032f);

		shader.setFloat("alpha", 1.0f);
	
		ourModel.Draw(shader);
		glEnable(GL_CULL_FACE);
		/*glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		shader.setBool("useNormal", true);*/
		
	}

	std::vector<float> getAllVertices() {
		return ourModel.getAllVertices();
	}


};