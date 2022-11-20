#include <iostream>
#include <array>
#include <fstream>
#include <vector>
#include <cassert>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int Width = 800;
int Heigth = 600;

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	const GLfloat rotationSpeed = 10;

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch (key)
		{
		case GLFW_KEY_W:
			rotationX -= rotationSpeed;
			break;
		case GLFW_KEY_S:
			rotationX += rotationSpeed;
			break;
		case GLFW_KEY_D:
			rotationY += rotationSpeed;
			break;
		case GLFW_KEY_A:
			rotationY -= rotationSpeed;
			break;
		}
	}
}

void DrawCube(GLfloat centerPosX, GLfloat centerPosY, GLfloat centerPosZ, GLfloat edgeLength) {
	GLfloat halfSideLength = edgeLength * 0.5f;

	GLfloat vertices[] = {

		// face da frente
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // cima esquerda
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // cima direita
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // baixo direita
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // baixo esquerda

		// face de trás
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // cima esquerda
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // cima direita
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // baixo direita
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // baixo esquerda

		// face esquerda
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // cima esquerda
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // cima direita
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // baixo direita
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // baixo esquerda

		// face direita
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // cima esquerda
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // cima direita
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // baixo direita
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // baixo esquerda

		// face de cima
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // cima esquerda
		centerPosX - halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // cima direita
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ - halfSideLength, // baixo direita
		centerPosX + halfSideLength, centerPosY + halfSideLength, centerPosZ + halfSideLength, // baixo esquerda

		// face de baixo
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength, // cima esquerda
		centerPosX - halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // cima direita
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ - halfSideLength, // baixo direita
		centerPosX + halfSideLength, centerPosY - halfSideLength, centerPosZ + halfSideLength  // baixo esquerda
	};

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glEnableClientState(GL_VERTEX_ARRAY);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glVertexPointer(3, GL_FLOAT, 0, vertices);

	glDrawArrays(GL_QUADS, 0, 24);

	glDisableClientState(GL_VERTEX_ARRAY);
}

GLfloat rotationX = 0.0f;
GLfloat rotationY = 0.0f;

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
	{
		return -1;
	}

	window = glfwCreateWindow(Width, Heigth, "GC com OpenGL", NULL, NULL);

	glfwSetKeyCallback(window, keyCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);


	int screenWidth, screenHeight;
	glfwGetFramebufferSize(window, &screenWidth, &screenHeight);

	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	glViewport(0.0f, 0.0f, screenWidth, screenHeight);
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glOrtho(0, Width, 0, Heigth, 0, 1000); 
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity(); 

	GLfloat halfScreenWidth = Width / 2;
	GLfloat halfScreenHeight = Heigth / 2;


	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);

		glPushMatrix();
		glTranslatef(halfScreenWidth, halfScreenHeight, -500);
		glRotatef(rotationX, 1, 0, 0);
		glRotatef(rotationY, 0, 1, 0);
		glTranslatef(-halfScreenWidth, -halfScreenHeight, 500);

		DrawCube(halfScreenWidth, halfScreenHeight, -500, 200);

		glPopMatrix();

		glfwPollEvents();

		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}