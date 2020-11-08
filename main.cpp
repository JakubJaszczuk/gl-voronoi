#include <math.h>
#include <random>
#include <algorithm>
#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include "utils.h"

struct alignas(2 * sizeof(glm::vec4)) Point
{
	glm::vec4 color;
	glm::vec2 position;

	Point(float r, float g, float b, float x, float y) : color{r, g, b, 1}, position{x, y} {}
};

int main()
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	std::uniform_real_distribution<> dis_norm(-1.0, 1.0);

	if(!glfwInit())
	{
		return 1;
	}

	int32_t res_width = 512;
	int32_t res_height = 512;

	glfwSetErrorCallback(utils::error_callback);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	GLFWwindow* window = glfwCreateWindow(res_width, res_height, "Okno", NULL, NULL);

	if(!window)
	{
		glfwTerminate();
	}

	glfwSetFramebufferSizeCallback(window, utils::framebuffer_size_callback);
	glfwMakeContextCurrent(window);
	glDebugMessageCallback(utils::gl_message_callback, 0);
	glfwSwapInterval(1);

	const auto vs = utils::loadShader("vert.vert", GL_VERTEX_SHADER);
	const auto fs = utils::loadShader("frag.frag", GL_FRAGMENT_SHADER);
	const auto program = utils::createProgram({vs, fs});

	GLuint vao;
	glCreateVertexArrays(1, &vao);

	// Shader Storage buffer
	constexpr uint32_t num_points = 50;
	GLuint storage_buffer;
	glCreateBuffers(1, &storage_buffer);
	glNamedBufferStorage(storage_buffer, sizeof(Point) * num_points, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
	auto points = static_cast<Point*>(glMapNamedBufferRange(storage_buffer, 0, sizeof(Point) * num_points, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT));
	for(size_t i = 0; i < num_points; ++i)
	{
		points[i] = Point(dis(gen), dis(gen), dis(gen), dis(gen), dis(gen));
	}

	// Display data buffer
	GLuint display_buffer;
	glCreateBuffers(1, &display_buffer);
	glNamedBufferStorage(display_buffer, sizeof(GLuint) * 2 * 2, nullptr, GL_DYNAMIC_STORAGE_BIT);
	int32_t data[] = {res_width, res_height};
	glNamedBufferSubData(display_buffer, 0, sizeof(GLuint) * 2, data);
	
	// Prepare for MainLoop --> Bind everything
	glBindVertexArray(vao);
	glUseProgram(program);
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, display_buffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, storage_buffer);

	int32_t w, h;
	while(!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		glfwGetFramebufferSize(window, &w, &h);
		if(w != res_width || h != res_height)
		{
			res_width = w;
			res_height = h;
			int32_t data[] = {w, h};
			glNamedBufferSubData(display_buffer, 0, sizeof(GLuint) * 2, data);
		}

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glfwSwapBuffers(window);

		//for(size_t i = 0; i < num_points; ++i)
		//{
		//	points[i].position += glm::vec2(dis_norm(gen), dis_norm(gen)) * 0.002f;
		//}
		for(size_t i = 0; i < num_points; ++i)
		{
			glm::vec2 r = glm::vec2(0);
			for(size_t j = 0; j < num_points; ++j)
			{
				if(i != j)
				{
					glm::vec2 d = points[i].position - points[j].position;
					r += glm::normalize(d) / glm::dot(d, d);
				}
			}
			points[i].position += r * 0.00001f;
			if(points[i].position.x < 0)
			{
				points[i].position.x = 0;
			}
			else if(points[i].position.x > 1)
			{
				points[i].position.x = 1;
			}
			if(points[i].position.y < 0)
			{
				points[i].position.y = 0;
			}
			else if(points[i].position.y > 1)
			{
				points[i].position.y = 1;
			}
		}
	}

	glDeleteProgram(program);
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &storage_buffer);
	glDeleteBuffers(1, &display_buffer);
	glDeleteProgram(program);

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
