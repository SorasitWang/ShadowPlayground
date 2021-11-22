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
const int Y_SEGMENTS = 50;
const int X_SEGMENTS = 50;
const GLfloat PI = 3.14159265358979323846f;
class Ball {
public:
	Ball(float g) {
		this->g = g;
	};
	Ball() {

	};
	std::vector<glm::vec3> allColor = { glm::vec3(0.8f, 0.3f, 0.3f) , glm::vec3(0.3f, 0.8f, 0.3f) , glm::vec3(0.3f, 0.3f, 0.8f),
										glm::vec3(0.6f, 0.6f, 0.2f) ,glm::vec3(0.6f, 0.2f, 0.6f) ,glm::vec3(0.2f, 0.6f, 0.6f) };
	unsigned int VAO, VBO, EBO;
	unsigned int colVAO, colVBO, colEBO;
	float radius = 0.1;
	float lifeTime = 500.0f;
	float curTime = 0;
	float veloLostWall = 0.95;
	float veloLostBump = 0.0f;
	float friction = 0.01;
	float g = 0.01, gVelo = 0.0f;
	int countOnGround = 0;
	int countOnRoof = 0;
	bool isIn = true;
	bool move = true;
	bool col = false;
	int onSurface = -1; // 0 ground , 1 roof
	float colFade = 0.5f;
	std::vector<ColProperty> colMarkers;
	bool threeD = false;
	glm::vec3 velocity = glm::vec3(1, 1, 1);
	glm::vec3 direction = glm::vec3(0.3, 0.5, 0.0f);
	glm::vec3 color = glm::vec3(0.8f, 0.3f, 0.3f);
	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);



	void init(Shader shader, ballProperty prop, bool threeD, std::vector<Ball> balls) {
		this->threeD = threeD;
		lifeTime = prop.lifeTime;
		g = prop.g;
		veloLostWall = 1 - prop.velocityLostWall;
		veloLostBump = 1 - prop.velocityLostBump;
		friction = prop.friction;

		std::vector<float> sphereVertices;
		std::vector<int> sphereIndices;

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

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
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

	void draw(Shader shader, glm::mat4 projection, glm::mat4 view) {
		
		shader.use();
		glm::mat4 model = glm::translate(model, glm::vec3(position.x, position.y, position.z * threeD));
		model = glm::scale(model, glm::vec3(radius));

		shader.setMat4("model", model);
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		shader.setBool("threeD", threeD);
		shader.setVec3("color", color);

		shader.setFloat("alpha", 1.0f - (curTime / lifeTime));
		glBindVertexArray(this->VAO);
		glDrawElements(GL_TRIANGLES, X_SEGMENTS * Y_SEGMENTS * 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		
	}


};