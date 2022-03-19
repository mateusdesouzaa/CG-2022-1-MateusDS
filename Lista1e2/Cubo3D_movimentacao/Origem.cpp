/* Hello Triangle - código adaptado de https://learnopengl.com/#!Getting-started/Hello-Triangle
 *
 * Adaptado por Mateus de Souza - de Rossana Baptista Queiroz
 * para a disciplina de Processamento Gráfico - Jogos Digitais - Unisinos
 * Versão inicial: 7/4/2017
 * Última atualização em 17/03/2022
 *
 */

#include <iostream>
#include <string>
#include <assert.h>

using namespace std;

// GLAD
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

//GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int setupGeometry();

const GLuint WIDTH = 800, HEIGHT = 600;

bool rotateX = false, rotateY = false, rotateZ = false;
int viewID = 1;

glm::vec3 cameraPos, cameraFront, cameraUp;
bool firstMouse = true;
float lastX = WIDTH / 2.0, lastY = HEIGHT / 2.0; 
float yaw = -90.0, pitch = 0.0; 

float fov = 45.0f;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Função MAIN
int main()
{
	// Inicialização da GLFW
	glfwInit();

	// Criação da janela GLFW
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Ola Cubo!", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// GLAD: carrega todos os ponteiros d funções da OpenGL
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;

	}

	// Obtendo as informações de versão
	const GLubyte* renderer = glGetString(GL_RENDERER); /* get renderer string */
	const GLubyte* version = glGetString(GL_VERSION); /* version as a string */
	cout << "Renderer: " << renderer << endl;
	cout << "OpenGL version supported " << version << endl;

	// Definindo as dimensões da viewport com as mesmas dimensões da janela da aplicação
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	// Compilando e buildando o programa de shader
	Shader shader = Shader("../shaders/hello.vs", "../shaders/hello.fs");

	// Gerando um buffer simples, com a geometria de um triângulo
	GLuint VAO = setupGeometry();

	glUseProgram(shader.ID);

	//Inicialização dos parâmetros da câmera
	cameraPos = glm::vec3(0.0, 0.0, 3.0);
	cameraFront = glm::vec3(0.0, 0.0, -1.0);
	cameraUp = glm::vec3(0.0, 1.0, 0.0);

	glm::mat4 model = glm::mat4(1);
	GLint modelLoc = glGetUniformLocation(shader.ID, "model");
	glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

	glm::mat4 view = glm::mat4(1);
	view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), cameraUp);
	GLint viewLoc = glGetUniformLocation(shader.ID, "view");
	glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));

	glm::mat4 projection = glm::mat4(1);
	projection = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);
	GLint projLoc = glGetUniformLocation(shader.ID, "projection");
	glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));

	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glEnable(GL_DEPTH_TEST);


	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f); //cor de fundo
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glLineWidth(10);
		glPointSize(20);

		float angle = (GLfloat)glfwGetTime();

		model = glm::mat4(1);
		glm::mat4 view = glm::mat4(1);

		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		projection = glm::perspective(glm::radians(fov), (GLfloat)WIDTH / (GLfloat)HEIGHT, 0.1f, 100.0f);

		if (rotateX)
		{
			model = glm::rotate(model, angle, glm::vec3(1.0f, 0.0f, 0.0f));

		}
		else if (rotateY)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 1.0f, 0.0f));

		}
		else if (rotateZ)
		{
			model = glm::rotate(model, angle, glm::vec3(0.0f, 0.0f, 1.0f));

		}

		glUniformMatrix4fv(viewLoc, 1, FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(projLoc, 1, FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(modelLoc, 1, FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 14 * 3);

		glDrawArrays(GL_POINTS, 0, 14 * 3);
		glBindVertexArray(0);

		glfwSwapBuffers(window);
	}

	glDeleteVertexArrays(1, &VAO);
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	float cameraSpeed = 0.15f * deltaTime;

	//Pra frente
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraPos += cameraSpeed * cameraFront;
	}

	//Pra trás
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraPos -= cameraSpeed * cameraFront;
	}

	//Pra esquerda
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	//Pra direita
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	}

	//Rota em X
	if (key == GLFW_KEY_X && action == GLFW_PRESS)
	{
		rotateX = true;
		rotateY = false;
		rotateZ = false;
	}

	//Rota em Y
	if (key == GLFW_KEY_Y && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = true;
		rotateZ = false;
	}

	//Rota em Z
	if (key == GLFW_KEY_Z && action == GLFW_PRESS)
	{
		rotateX = false;
		rotateY = false;
		rotateZ = true;
	}

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	//Amortizando, deixando o movimento mais suave
	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

int setupGeometry()
{
	GLfloat vertices[] = {

		//Base
		-0.5, -0.5, -0.5, 0.0, 1.0, 1.0,
		-0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 0.0, 1.0, 1.0,

		-0.5, -0.5, 0.5,  0.0, 1.0, 1.0,
		 0.5, -0.5,  0.5, 0.0, 1.0, 1.0,
		 0.5, -0.5, -0.5, 0.0, 1.0, 1.0,

		 //Frente
		 -0.5, -0.5, -0.5, 1.0, 0.0, 0.0,
		 -0.5,  0.5, -0.5, 1.0, 0.0, 0.0,
		  0.5, -0.5, -0.5, 1.0, 0.0, 0.0,

		  0.5, -0.5, -0.5, 1.0, 0.0, 0.0,
		 -0.5,  0.5, -0.5, 1.0, 0.0, 0.0,
		  0.5,  0.5, -0.5, 1.0, 0.0, 0.0,

		  //Trás
		  -0.5,  0.5,  0.5, 0.0, 1.0, 0.0,
		  -0.5, -0.5,  0.5, 0.0, 1.0, 0.0,
		   0.5,  0.5,  0.5, 0.0, 1.0, 0.0,

		   0.5,  0.5,  0.5, 0.0, 1.0, 0.0,
		  -0.5, -0.5,  0.5, 0.0, 1.0, 0.0,
		   0.5, -0.5,  0.5, 0.0, 1.0, 0.0,

		   //Esquerda
		   -0.5,  0.5, -0.5, 0.0, 0.0, 1.0,
		   -0.5, -0.5, -0.5, 0.0, 0.0, 1.0,
		   -0.5, -0.5,  0.5, 0.0, 0.0, 1.0,

		   -0.5, -0.5,  0.5, 0.0, 0.0, 1.0,
		   -0.5,  0.5,  0.5, 0.0, 0.0, 1.0,
		   -0.5,  0.5, -0.5, 0.0, 0.0, 1.0,

		   //Direita
			0.5,  0.5,  0.5, 1.0, 0.5, 0.0,
			0.5, -0.5,  0.5, 1.0, 0.5, 0.0,
			0.5, -0.5, -0.5, 1.0, 0.5, 0.0,

			0.5, -0.5, -0.5, 1.0, 0.5, 0.0,
			0.5,  0.5, -0.5, 1.0, 0.5, 0.0,
			0.5,  0.5,  0.5, 1.0, 0.5, 0.0,

			//Topo
			-0.5, 0.5,  0.5, 1.0, 0.0, 1.0,
			 0.5, 0.5,  0.5, 1.0, 0.0, 1.0,
			-0.5, 0.5, -0.5, 1.0, 0.0, 1.0,

			 0.5, 0.5,  0.5, 1.0, 0.0, 1.0,
			 0.5, 0.5, -0.5, 1.0, 0.0, 1.0,
			-0.5, 0.5, -0.5, 1.0, 0.0, 1.0,


			//Chão
			-1.0, -0.5, -1.0, 0.5, 0.5, 0.5,
			-1.0, -0.5,  1.0, 0.5, 0.5, 0.5,
			 1.0, -0.5, -1.0, 0.5, 0.5, 0.5,

			-1.0, -0.5,  1.0,  0.5, 0.5, 0.5,
			 1.0, -0.5,  1.0, 0.5, 0.5, 0.5,
			 1.0, -0.5, -1.0, 0.5, 0.5, 0.5,

	};

	GLuint VBO, VAO;

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return VAO;
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;
}