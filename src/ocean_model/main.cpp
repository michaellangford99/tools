#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include <implot.h>

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>
#include <iomanip>

#include <gl_lib/base_window.h>
#include <gl_lib/texture.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ocean_model.h"

class ocean_test
{
public:
	std::shared_ptr<base_window> g;

	ocean_test(std::shared_ptr<base_window>  _wave_plot_window) : g(_wave_plot_window)
	{
		
	}

	void generate_imgui_windows()
	{
		// ImGUI window creation
		ImGui::Begin("Settings");
		// Ends the window
		ImGui::End();

		ImGui::ShowDemoWindow();
		ImPlot::ShowDemoWindow();

		// all primary window logic
	}

	void run()
	{
		ocean_spectrum spec = ocean_spectrum(2048, 2048);

		while(!glfwWindowShouldClose(g->window))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			spec.generate_spectrum();

			g->imgui_init();
			generate_imgui_windows();

			ImGui::Begin("test wind");

			spec.ocean_spectrum_shader->generate_imgui_editor();

			ImGui::Image(spec.ocean_spectrum_tex->gl_texture_id, {float(spec.ocean_spectrum_tex->width), float(spec.ocean_spectrum_tex->height)});

			ImGui::End();

			g->imgui_cleanup();

			glfwSwapBuffers(g->window);
			glfwPollEvents();
		}
	}

};

int main()
{
	std::shared_ptr<base_window> g = std::shared_ptr<base_window>(new base_window(800, 600));
	ocean_test o = ocean_test(g);
	o.run();
	return 0;
}