#ifndef GLACIER_H
#define GLACIER_H

#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <stdexcept>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct ImGuiDockNode;

class base_window 
{
public:
	GLFWwindow* window;

	//viewport parameters
	glm::vec2 viewport_pos = {0,0};
	glm::vec2 viewport_size;
	glm::vec2 window_size;
    ImGuiDockNode* node;

	base_window();
	base_window(int width, int height);
	~base_window();
	
	GLFWwindow* create_glfw_window(int width, int height);
	void set_render_area(glm::vec2 pos, glm::vec2 size);
	void setup_callbacks(GLFWwindow* window);
	void setup_imgui(GLFWwindow* window);
	void apply_theme(bool light);
	void imgui_init();
    void imgui_cleanup();

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	static void processInput(GLFWwindow *window);
};

#endif