
#include <cmath>
#include <vector>
#include <algorithm>
#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "shader.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// window size
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// point structure
struct point {
	int x;
	int y;
	point(int x0, int y0) :x(x0), y(y0) {}
};


vector<point> bresenham_line(point p1, point p2);
vector<point> draw_triangle(point p1, point p2, point p3);
void bresenham_circle(vector<point> &points, point p0, point p1);
vector<point> draw_circle(point p0, int r);
void swap(int *a, int *b);
float* get_vectices(vector<point> points);


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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	Shader ourShader("./shader.vs", "./shader.fs");

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	bool ifCircle = false;
	int r = 100;
	ImVec4 color = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);


	// Imgui setting
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsLight();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	// render loop
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		// Imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		{
			ImGui::Begin("Imgui", false, ImGuiWindowFlags_MenuBar);
			ImGui::SetWindowSize(ImVec2(400, 100));
			if (ImGui::BeginMenuBar())
			{
				if (ImGui::BeginMenu("Select shape"))
				{
					if (ImGui::MenuItem("Circle")) {
						ifCircle = true;
					}
					if (ImGui::MenuItem("Triangle")) {
						ifCircle = false;
					}
					ImGui::EndMenu();
				}
				ImGui::EndMenuBar();
			}
			if (ifCircle)
			{
				ImGui::SliderInt("radius", &r, 0, 200);
				ImGui::ColorEdit3("select color", (float*)&color);
			}
			else
			{
				ImGui::ColorEdit3("select color", (float*)&color);
			}
			ImGui::End();
		}

		vector<point> points;
		if (ifCircle)
		{
			points = draw_circle(point(0, 0), r);
		}
		else
		{
			points = draw_triangle(point(0, 200), point(200, -100), point(-200, -100));
		}

		float *vertices = get_vectices(points);
		for (int k = 0; k < points.size(); k++)
		{
			vertices[6 * k + 3] = color.x;
			vertices[6 * k + 4] = color.y;
			vertices[6 * k + 5] = color.z;
		}

		// render
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		ourShader.use();
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, (points.size() * 6) * sizeof(float), vertices, GL_STREAM_DRAW);
		
		// position
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		// Unbind
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(VAO);
		delete vertices;

		glDrawArrays(GL_POINTS, 0, points.size());

		// Imgui render
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
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


// Bresenham line
vector<point> bresenham_line(point p1, point p2)
{
	vector<point> tmp;

	bool k = 1;
	int dx = abs(p2.x - p1.x);
	int dy = abs(p2.y - p1.y);
	if (dx < dy)
	{
		k = 0;
		swap(&p1.x, &p1.y);
		swap(&p2.x, &p2.y);
		swap(&dx, &dy);
	}

	int ix = (p2.x - p1.x) > 0 ? 1 : -1;
	int iy = (p2.y - p1.y) > 0 ? 1 : -1;
	int d = dy * 2 - dx;
	int x = p1.x;
	int y = p1.y;
	if (k)
	{
		while (x != p2.x)
		{
			if (d < 0)
			{
				d += dy * 2;
			}
			else
			{
				d += (dy - dx) * 2;
				y += iy;
			}
			tmp.push_back(point(x, y));
			x += ix;
		}
	}
	else
	{
		while (x != p2.x)
		{
			if (d < 0)
			{
				d += dy * 2;
			}
			else
			{
				d += (dy - dx) * 2;
				y += iy;
			}
			tmp.push_back(point(y, x));
			x += ix;
		}
	}
	return tmp;
}

// Draw triangle
vector<point> draw_triangle(point p1, point p2, point p3)
{
	vector<point> tri, line1, line2, line3;
	line1 = bresenham_line(p1, p2);
	line2 = bresenham_line(p1, p3);
	line3 = bresenham_line(p2, p3);
	tri.insert(tri.end(), line1.begin(), line1.end());
	tri.insert(tri.end(), line2.begin(), line2.end());
	tri.insert(tri.end(), line3.begin(), line3.end());
	line1.clear();
	line2.clear();
	line3.clear();
	tri.push_back(p1);
	tri.push_back(p2);
	tri.push_back(p3);
	return tri;
}

// Bresenham circle
void bresenham_circle(vector<point> &points, point p0, point p1)
{
	points.push_back(point(p0.x + p1.x, p0.y + p1.y));
	points.push_back(point(p0.x - p1.x, p0.y + p1.y));
	points.push_back(point(p0.x + p1.x, p0.y - p1.y));
	points.push_back(point(p0.x - p1.x, p0.y - p1.y));
	points.push_back(point(p0.x + p1.y, p0.y + p1.x));
	points.push_back(point(p0.x - p1.y, p0.y + p1.x));
	points.push_back(point(p0.x + p1.y, p0.y - p1.x));
	points.push_back(point(p0.x - p1.y, p0.y - p1.x));
}

// Draw circle
vector<point> draw_circle(point p0, int r)
{
	vector<point> points;
	int x, y, yi, d;
	x = 0;
	y = r;
	d = 3 - 2 * r;
	while (x <= y)
	{
		bresenham_circle(points, p0, point(x, y));
		if (d < 0)
		{
			d = d + 4 * x + 6;
		}
		else
		{
			d = d + 4 * (x - y) + 10;
			y--;
		}
		x++;
	}
	return points;
}

// swap
void swap(int *a, int *b)
{
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}


// Get vectices
float* get_vectices(vector<point> points)
{
	int n = points.size();
	float *vertices = new float[n * 6];
	for (int i = 0; i < n; i++) {
		// position
		vertices[i * 6 + 0] = (float)points[i].x / ((float)SCR_WIDTH / 2);
		vertices[i * 6 + 1] = (float)points[i].y / ((float)SCR_HEIGHT / 2);
		vertices[i * 6 + 2] = 0.0f;
		// color
		vertices[i * 6 + 3] = 1.0f;
		vertices[i * 6 + 4] = 0.0f;
		vertices[i * 6 + 5] = 0.0f;
	}
	return vertices;
}