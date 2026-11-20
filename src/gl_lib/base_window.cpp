#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <implot.h>
#include <spdlog/spdlog.h>

#define IMGUI_USER_CONFIG
#define IMGUI_ENABLE_FREETYPE

#include <vector>
#include <map>
#include <algorithm>
#include <memory>
#include <stdexcept>

#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include <iostream>

#include "base_window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


GLFWwindow* base_window::create_glfw_window(int width, int height)
{
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//glfwWindowHint(GLFW_SAMPLES, 4);

	GLFWwindow* window = glfwCreateWindow(width, height, "Glacier", NULL, NULL);
	if (window == NULL)
	{
		spdlog::error("Failed to create GLFW window");
		glfwTerminate();
		return NULL;
	}
	glfwMakeContextCurrent(window);

	glfwSwapInterval(1); 

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		spdlog::error("Failed to initialize GLAD");
		return NULL;
	}

	glViewport(0, 0, width, height);
	//glEnable(GL_MULTISAMPLE);
	return window;
}

void base_window::setup_callbacks(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);

	glfwSetWindowUserPointer(window, this);
}

void base_window::setup_imgui(GLFWwindow* window)
{
	//
	//Initialize ImGUI
	//
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	// Font: oversampling raises the per-pixel sharpness for slightly better
	// readability at small sizes. PixelSnap keeps glyph baselines on integer
	// pixel rows, which avoids subpixel jitter while panning ImPlot.
	ImFontConfig imf = ImFontConfig();
	imf.OversampleH = 3;
	imf.OversampleV = 3;
	imf.PixelSnapH = true;


	// Glyph range: ImGui's default range only covers basic Latin. We use Δ,
	// 1/Δt, etc. throughout the markers/measure overlays, so we bolt on the
	// Latin-1 supplement, Greek, math operators, arrows, and general
	// punctuation. (`ranges` must outlive the font atlas — making it a
	// `static` keeps it valid for the program's lifetime.)
	ImFontGlyphRangesBuilder builder;
	builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
	static const ImWchar extra_ranges[] = {
		0x00A0, 0x00FF, // Latin-1 supplement
		0x0370, 0x03FF, // Greek (Δ = 0x0394, π = 0x03C0, μ = 0x03BC, …)
		0x2000, 0x206F, // General punctuation
		0x2190, 0x21FF, // Arrows
		0x2200, 0x22FF, // Mathematical operators
		0,
	};
	builder.AddRanges(extra_ranges);
	static ImVector<ImWchar> ranges;
	builder.BuildRanges(&ranges);

	//io.Fonts->AddFontFromFileTTF("Ubuntu-R.ttf", 13, &imf, ranges.Data);

	// Default to the dark palette; the app can flip via apply_theme(true).
	apply_theme(false);

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void base_window::apply_theme(bool light)
{
	// Geometry/rounding is identical between themes — only colors change.
	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha             = 1.0f;
	style.FrameRounding     = 2.0f;
	style.WindowRounding    = 3.0f;
	style.ChildRounding     = 2.0f;
	style.GrabRounding      = 2.0f;
	style.PopupRounding     = 3.0f;
	style.TabRounding       = 2.0f;
	style.ScrollbarRounding = 2.0f;


	// Both palettes use four background shades + three highlight shades, so
	// every UI surface gets a consistent step in tone whether dark or light.
	// Light theme uses ImPlot::StyleColorsLight so plot text/grid invert too.
	ImVec4 bg0, bg1, bg2, bg3, hi1, hi2, hi3, txt, txtD, line, plotline;
	if (light) {
		ImGui::StyleColorsLight();
		ImPlot::StyleColorsLight(&ImPlot::GetStyle());
		bg0      = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
		bg1      = ImVec4(0.93f, 0.93f, 0.93f, 1.00f);
		bg2      = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
		bg3      = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
		hi1      = ImVec4(0.74f, 0.74f, 0.74f, 1.00f);
		hi2      = ImVec4(0.62f, 0.62f, 0.62f, 1.00f);
		hi3      = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		txt      = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
		txtD     = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		line     = ImVec4(0.65f, 0.65f, 0.65f, 1.00f);
		plotline = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
	} else {
		ImGui::StyleColorsDark();
		ImPlot::StyleColorsDark(&ImPlot::GetStyle());
		bg0      = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
		bg1      = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		bg2      = ImVec4(0.12f, 0.12f, 0.12f, 1.00f);
		bg3      = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
		hi1      = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
		hi2      = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
		hi3      = ImVec4(0.45f, 0.45f, 0.45f, 1.00f);
		txt      = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		txtD     = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		line     = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		plotline = ImVec4(0.85f, 0.85f, 0.85f, 1.00f);
	}


	style.Colors[ImGuiCol_Text]                  = txt;
	style.Colors[ImGuiCol_TextDisabled]          = txtD;
	style.Colors[ImGuiCol_WindowBg]              = bg1;
	style.Colors[ImGuiCol_ChildBg]               = bg1;
	style.Colors[ImGuiCol_PopupBg]               = bg2;
	style.Colors[ImGuiCol_Border]                = line;
	style.Colors[ImGuiCol_BorderShadow]          = ImVec4(0,0,0,0);
	style.Colors[ImGuiCol_FrameBg]               = bg3;
	style.Colors[ImGuiCol_FrameBgHovered]        = hi1;
	style.Colors[ImGuiCol_FrameBgActive]         = hi2;
	style.Colors[ImGuiCol_TitleBg]               = bg0;
	style.Colors[ImGuiCol_TitleBgActive]         = bg2;
	style.Colors[ImGuiCol_TitleBgCollapsed]      = bg0;
	style.Colors[ImGuiCol_MenuBarBg]             = bg2;
	style.Colors[ImGuiCol_ScrollbarBg]           = bg0;
	style.Colors[ImGuiCol_ScrollbarGrab]         = hi1;
	style.Colors[ImGuiCol_ScrollbarGrabHovered]  = hi2;
	style.Colors[ImGuiCol_ScrollbarGrabActive]   = hi3;
	style.Colors[ImGuiCol_CheckMark]             = light
	    ? ImVec4(0.15f, 0.15f, 0.15f, 1.0f)
	    : ImVec4(0.85f, 0.85f, 0.85f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab]            = hi2;
	style.Colors[ImGuiCol_SliderGrabActive]      = hi3;
	style.Colors[ImGuiCol_Button]                = bg3;
	style.Colors[ImGuiCol_ButtonHovered]         = hi1;
	style.Colors[ImGuiCol_ButtonActive]          = hi2;
	style.Colors[ImGuiCol_Header]                = hi1;
	style.Colors[ImGuiCol_HeaderHovered]         = hi2;
	style.Colors[ImGuiCol_HeaderActive]          = hi3;
	style.Colors[ImGuiCol_Separator]             = line;
	style.Colors[ImGuiCol_SeparatorHovered]      = hi2;
	style.Colors[ImGuiCol_SeparatorActive]       = hi3;
	style.Colors[ImGuiCol_ResizeGrip]            = hi1;
	style.Colors[ImGuiCol_ResizeGripHovered]     = hi2;
	style.Colors[ImGuiCol_ResizeGripActive]      = hi3;
	style.Colors[ImGuiCol_Tab]                   = bg2;
	style.Colors[ImGuiCol_TabHovered]            = hi2;
	style.Colors[ImGuiCol_TabActive]             = hi1;
	style.Colors[ImGuiCol_TabUnfocused]          = bg1;
	style.Colors[ImGuiCol_TabUnfocusedActive]    = bg3;
	style.Colors[ImGuiCol_DockingPreview]        = ImVec4(0.55f, 0.55f, 0.55f, 0.6f);
	style.Colors[ImGuiCol_DockingEmptyBg]        = bg0;
	style.Colors[ImGuiCol_TableHeaderBg]         = bg3;
	style.Colors[ImGuiCol_TableBorderStrong]     = line;
	style.Colors[ImGuiCol_TableBorderLight]      = light
	    ? ImVec4(0.78f, 0.78f, 0.78f, 1.00f)
	    : ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	style.Colors[ImGuiCol_TableRowBg]            = ImVec4(0,0,0,0);
	style.Colors[ImGuiCol_TableRowBgAlt]         = light
	    ? ImVec4(0,0,0,0.03f)
	    : ImVec4(1,1,1,0.03f);
	style.Colors[ImGuiCol_TextSelectedBg]        = hi3;
	style.Colors[ImGuiCol_NavHighlight]          = hi3;


	// Nudge ImPlot so axis text and frame contrast against the new bg.
	ImPlotStyle& ps = ImPlot::GetStyle();
	ps.Colors[ImPlotCol_FrameBg]   = bg1;
	ps.Colors[ImPlotCol_PlotBg]    = bg0;
	ps.Colors[ImPlotCol_PlotBorder]= line;
	ps.Colors[ImPlotCol_AxisText]  = txt;
	ps.Colors[ImPlotCol_AxisGrid]  = light
	    ? ImVec4(0.0f, 0.0f, 0.0f, 0.18f)
	    : ImVec4(1.0f, 1.0f, 1.0f, 0.16f);
	ps.Colors[ImPlotCol_AxisTick]  = line;
	(void)plotline;
}

void base_window::imgui_init()
{
	// Tell OpenGL a new frame is about to begin
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiID id = ImGui::DockSpaceOverViewport(0, nullptr, ImGuiDockNodeFlags_NoDockingInCentralNode | ImGuiDockNodeFlags_PassthruCentralNode, nullptr);
	node = ImGui::DockBuilderGetCentralNode(id);

	// user overriden function
	//TODO: bc what if they dont want to override. this is a lib class not a main class
	//generate_imgui_windows();
}

void base_window::imgui_cleanup()
{	
	if (node)
		set_render_area({node->Pos.x, node->Pos.y}, {node->Size.x, node->Size.y}); //maybe there's an imgui_handler class that I poll??? or there's some callback??? IDK

	// Renders the ImGUI elements
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

/*void base_window::generate_imgui_windows()
{

}*/

base_window::base_window() : base_window(800, 600)
{

}

base_window::base_window(int width, int height)
{
	spdlog::set_level(spdlog::level::debug); // Set *global* log level to debug
	
	window_size = {width, height};

	GLFWwindow* _window = create_glfw_window(window_size.x, window_size.y); //move this intp base_window constructor
	if (_window == NULL) throw std::runtime_error("Unable to initiaize GLFW Window");
	window = _window;

	setup_callbacks(window);
	setup_imgui(window);

	int vp[4];
	glGetIntegerv(GL_VIEWPORT, vp);

	//TODO: this should be initialized from parameter and this is dumb
	viewport_pos = {vp[0], vp[1]};
	viewport_size = {vp[2], vp[3]};
	window_size = viewport_size;
}

base_window::~base_window()
{
	spdlog::info("Shutting Down GLacier");

	// Deletes all ImGUI instances
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

	glfwTerminate();
}

void base_window::set_render_area(glm::vec2 pos, glm::vec2 size)
{
	viewport_pos = pos;
	viewport_size = size;
}

//TODO: pull these into the camera itself and set whether the camera is the 'active camera' somehow. idk.

void base_window::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	base_window* g_ptr = reinterpret_cast<base_window*>(glfwGetWindowUserPointer(window));
	g_ptr->window_size = {width, height};
}

void base_window::mouse_callback(GLFWwindow*, double, double)
{
}

void base_window::mouse_button_callback(GLFWwindow*, int, int, int)
{
}

void base_window::scroll_callback(GLFWwindow*, double, double)
{
}