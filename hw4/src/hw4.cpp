
#include <vector>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
using namespace std;


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

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

	bool err = gladLoadGL() == 0;
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return 1;
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

		-2.0f, -2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
		2.0f, -2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
		2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
		2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
		-2.0f,  2.0f,  2.0f,  1.0f, 0.0f, 0.0f,
		-2.0f, -2.0f,  2.0f,  1.0f, 0.0f, 0.0f,

		-2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f,  2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f, -2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
		-2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,

		2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
		2.0f,  2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		2.0f, -2.0f, -2.0f,  0.0f, 1.0f, 0.0f,
		2.0f, -2.0f,  2.0f,  0.0f, 1.0f, 0.0f,
		2.0f,  2.0f,  2.0f,  0.0f, 1.0f, 0.0f,

		-2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
		2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
		2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		-2.0f, -2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		-2.0f, -2.0f, -2.0f,  0.0f, 0.0f, 1.0f,

		-2.0f,  2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
		2.0f,  2.0f, -2.0f,  0.0f, 0.0f, 1.0f,
		2.0f,  2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		2.0f,  2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		-2.0f,  2.0f,  2.0f,  0.0f, 0.0f, 1.0f,
		-2.0f,  2.0f, -2.0f,  0.0f, 0.0f, 1.0f
	};


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

	glDisable(GL_DEPTH_TEST);


	Shader ourShader("shader.vs", "shader.fs");

	int mode = 0;
	bool  is_depth_test = false;
	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		glfwPollEvents();

		// ImGui new frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			ImGui::Begin("Imgui");	

			if (ImGui::Button("Depth Test On"))
				is_depth_test = true;
			ImGui::SameLine();
			if (ImGui::Button("Depth Test Off"))
				is_depth_test = false;


			if (ImGui::Button("Translation x"))
				mode = 1;
			ImGui::SameLine();
			if (ImGui::Button("Translation y"))
				mode = 2;

			if (ImGui::Button("Stop"))
				mode = 0;
			ImGui::SameLine();
			if (ImGui::Button("Rotation"))
				mode = 3;
			ImGui::SameLine();
			if (ImGui::Button("Scaling"))
				mode = 4;

			ImGui::End();
		}

		// render
		ImGui::Render();
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);						
		ourShader.use();

		// create transformations
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 model = glm::mat4(1.0f); 
		glm::mat4 projection = glm::mat4(1.0f);

		vector<glm::mat4> ourModel;
		ourModel.assign(2, glm::mat4(1.0f));

		view = glm::lookAt( glm::vec3(0.0f, 1.0f, 3.0f),
							glm::vec3(0.0f, 0.0f, 0.0f),
							glm::vec3(0.0f, 1.0f, 1.0f));

		projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		unsigned int modelLoc = glGetUniformLocation(ourShader.ID, "model");
		unsigned int viewLoc = glGetUniformLocation(ourShader.ID, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		ourShader.setMat4("projection", projection);

		switch (mode)
		{
			case 0:
				break;
			case 1:
				model = glm::translate(model, (float)sin(glfwGetTime()) * glm::vec3(1.0f, 0.0f, 0.0f));
				break;
			case 2:
				model = glm::translate(model, (float)sin(glfwGetTime()) * glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			case 3:
				model = glm::rotate(model, (float)glfwGetTime() * 2.0f, glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 4:
				model = glm::scale(model, (float)abs(sin(glfwGetTime())) * glm::vec3(2.0f, 2.0f, 2.0f));
				break;
		}
		switch (is_depth_test)
		{
			case 0:
				glDisable(GL_DEPTH_TEST);
				break;
			case 1:
				glEnable(GL_DEPTH_TEST);
				break;
		}

		// render cube
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
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
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}