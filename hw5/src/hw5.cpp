#include <iostream>
#include "camera.h"
#include "shader.h"
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 10.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;


int main()
{

	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, cursorPosCallback);

	bool err = gladLoadGL() == 0;
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return -1;
	}

	// Imgui setting
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsLight();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	// cube
	float vertices[] = {

		-2.0f, -2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
		2.0f, -2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
		2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
		2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
		-2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 0.0f,
		-2.0f, -2.0f, -2.0f,  1.0f, 0.0f, 0.0f,

		-2.0f, -2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
		2.0f, -2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
		2.0f,  2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
		2.0f,  2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
		-2.0f,  2.0f,  2.0f,  1.0f, 1.0f, 0.0f,
		-2.0f, -2.0f,  2.0f,  1.0f, 1.0f, 0.0f,

		-2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f,  2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f, -2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,

		2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 1.0f,
		2.0f,  2.0f, -2.0f,  0.0f, 1.0f, 1.0f,
		2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 1.0f,
		2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 1.0f,
		2.0f, -2.0f,  2.0f,  0.0f, 1.0f, 1.0f,
		2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 1.0f,

		-2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
		2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
		2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		-2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		-2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,

		-2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 1.0f,
		2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 1.0f,
		2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 1.0f,
		2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 1.0f,
		-2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 1.0f,
		-2.0f,  2.0f, -2.0f,  1.0f, 0.0f, 1.0f
	};

	float ortho[4] = { -10.0f, 10.0f, -10.0f, 10.0f };
	float distance[2] = { 0.1f, 100.0f };

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glEnable(GL_DEPTH_TEST);

	Shader ourShader("shader.vs", "shader.fs");

	int mode = 0;

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		glfwPollEvents();
		processInput(window);

		// imgui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (mode != 3) {
			ImGui::Begin("Imgui");
			
			ImGui::RadioButton("Orthographic", &mode, 0);
			ImGui::RadioButton("Perspective", &mode, 1);
			ImGui::RadioButton("View changing", &mode, 2);
			ImGui::RadioButton("Free Camera", &mode, 3);

			ImGui::InputFloat4("left, right, bottom, top", ortho, 2);
			ImGui::InputFloat2("near, far", distance, 2);

			ImGui::End();
		}

		// render
		ImGui::Render();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		ourShader.use();

		// transformation
		glm::mat4 model = glm::mat4(1.0f); 
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		float Radius = 10.0f;
		float camPosX = sin(glfwGetTime()) * Radius;
		float camPosZ = cos(glfwGetTime()) * Radius;


		switch (mode)
		{
		case 0:
			model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
			view = camera.GetViewMatrix();
			projection = glm::ortho(ortho[0], ortho[1], ortho[2], ortho[3], distance[0], distance[1]);
			break;
		case 1:
			model = glm::translate(model, glm::vec3(-1.5f, 0.5f, -1.5f));
			view = camera.GetViewMatrix();
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, distance[0], distance[1]);
			break;
		case 2:
			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -0.0f));
			view = glm::lookAt(glm::vec3(camPosX, 0.0f, camPosZ), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 100.0f);
			break;
		case 3:
			model = glm::mat4(1.0f);
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			view = camera.GetViewMatrix();
			projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_WIDTH, 0.1f, 100.0f);
			break;
		}

		unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		ourShader.setMat4("projection", projection);

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(window);
		glfwSwapBuffers(window);
	}

	// de-allocate
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);


	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);

}


void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
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

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}