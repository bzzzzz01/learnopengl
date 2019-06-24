#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

glm::vec2 Q(double t);
int C(int c, int k);
int factorial(int n);

vector<glm::vec2> point;

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {

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
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	bool err = gladLoadGL() == 0;
	if (err)
	{
		fprintf(stderr, "Failed to initialize OpenGL loader!\n");
		return -1;
	}


	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(0);

	//shader
	Shader bezierShader("bezier.vs", "bezier.fs");

	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glPointSize(8.0f);

		bezierShader.setVec3("curveColor", glm::vec3(0.8f, 0.8f, 0.8f));
		bezierShader.use();
		
		vector<float> controlPoints;

		for (int i = 0; i < point.size(); ++i) {
			vector<float> renderP = { point[i].x, point[i].y, 0.0f };
			controlPoints.insert(controlPoints.end(), renderP.begin(), renderP.end());
			
			// auxiliary lines
			if (i > 0) {
				float* ptr = controlPoints.data() + (i - 1) * 3;
				glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), ptr, GL_STATIC_DRAW);
				glDrawArrays(GL_LINES, 0, 2);
			}
		}

		// control points
		float *ptr = controlPoints.data();
		glBufferData(GL_ARRAY_BUFFER, controlPoints.size() * sizeof(float), ptr, GL_STATIC_DRAW);
		glDrawArrays(GL_POINTS, 0, point.size());

		// bezier curve
		if (point.size() > 2)
		{
			vector<float> bezierPoints;
			int count = 0;
			for (double i = 0.0f; i < 1.0f; i += 0.001)
			{
				glm::vec2 point(Q(i));
				vector<float> points { point.x, point.y, 0.0f };
				bezierPoints.insert(bezierPoints.end(), points.begin(), points.end());
				if (count > 0)
				{
					float *ptr = bezierPoints.data() + (count - 1) * 3;
					glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), ptr, GL_STATIC_DRAW);
					glDrawArrays(GL_LINES, 0, 2);
				}
				count++;
			}
		}
		
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		point.push_back(glm::vec2((float(xpos) / float(SCR_WIDTH) * 2.0f) - 1, -((float(ypos) / float(SCR_HEIGHT) * 2.0f) - 1)));
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
		if(!point.empty()) point.pop_back();
	}
}

// Q(t)
glm::vec2 Q(double t) {
	int n = point.size() - 1;
	double x = 0, y = 0;
	double u = 1, v = pow((1 - t), n);

	for (int i = 0; i <= n; i++) {
		x += C(n, i) * u * v * point[i].x;
		y += C(n, i) * u * v * point[i].y;
		u *= t;
		v /= (1 - t);
	}

	return glm::vec2(x, y);
}

// C(n, k)
int C(int n, int k) {
	return factorial(n) / (factorial(k) * (factorial(n - k)));
}

// n!
int factorial(int n) {
	if (n <= 1) return 1;
	return n * factorial(n - 1);
}
