#include "xtensor/reducers/xreducer.hpp"
#include <cmath>
#include <cstdio>
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
#include <xtensor/core/xmath.hpp>
#include <xtensor/misc/xmanipulation.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <istream>
#include <fstream>
#include <iostream>
#include <xtensor.hpp>
#include <xtensor/containers/xarray.hpp>
#include <xtensor/io/xnpy.hpp>

#include "gpu_heatmap.h"

#include <gl_lib/texture.h>

class wave_plot
{
public:
	std::shared_ptr<base_window> g;

	wave_plot(std::shared_ptr<base_window>  _wave_plot_window) : g(_wave_plot_window)
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

	//typedef int (*ImPlotFormatter)(double value, char* buff, int size, void* user_data);
	// TODO: make more generic
	static int SI_formatter(double value, char* buff, int size, void* user_data)
	{
		//sig_digits
		int sig_digits = 4;

		// assume user_data is the units
		const char* unit = (const char*)user_data;

		typedef struct {
			double order;
			const char* prefix;
		} si_prefix_t;

		si_prefix_t prefixes[] = {
			{1e-15, "f"},
			{1e-12, "p"},
			{1e-9, "n"},
			{1e-6, "µ"},
			{1e-3, "m"},
			{1e0, ""},
			{1e3,"k"},
			{1e6,"M"},
			{1e9,"G"},
			{1e12,"T"},
			{1e15,"P"}
		};

		// bounds checks
		if (value == 0.0 || !std::isfinite(value)) {
			std::snprintf(buff, size, "%.*g %s", sig_digits, value, unit);
			return 1;
		}

		double mag = std::fabs(value);
		int chosen = 0;

		for (int i = 0; i < (int)(sizeof(prefixes) / sizeof(prefixes[0])); i++)
		{
			if (mag >= prefixes[i].order && mag < prefixes[i].order*1e3)
			{
				chosen = i;
				break;
			}

			if (mag < prefixes[0].order) {chosen=0; break;}
			if (mag >= prefixes[(int)(sizeof(prefixes) / sizeof(prefixes[0])) - 1].order) { chosen = (int)(sizeof(prefixes) / sizeof(prefixes[0])) - 1; break;}
		}

		double scaled = value / prefixes[chosen].order;

		std::snprintf(buff, size, "%.*g %s%s", sig_digits, scaled, prefixes[chosen].prefix, unit);

		return 1;
	}

	void run()
	{
		//locals:
		//...

		// will probably have to replace with another loader
		auto data = xt::load_npy<std::complex<double>>("test_data.npy");
		
		auto data_dB = xt::eval(xt::cast<float>(20*xt::log10(xt::abs(data))));
		data_dB = xt::where(xt::isnan(data_dB), -100.0f, data_dB);

		xt::view(data_dB, 10, xt::all()) = 90;

		auto x = xt::flatten(data);

		auto shape = data.shape();

		std::cout << data << std::endl;

		for (auto& s : shape)
			std::cout << s << std::endl;

		float data_min = xt::amin(data_dB)();
		float data_max = xt::amax(data_dB)();

		gpu_heatmap* gpu_h = new gpu_heatmap();

		// put these into a helper function
		std::shared_ptr<texture> data_tex = std::shared_ptr<texture>(new texture(data_dB.data(), shape[1], shape[0], 1, 4, GL_RED, GL_R32F, GL_FLOAT));
		std::shared_ptr<texture> heatmap_tex_out = std::shared_ptr<texture>(new texture(nullptr, shape[1], shape[0], 4, 2, GL_RGBA, GL_RGBA, GL_SHORT));
		std::shared_ptr<framebuffer> heatmap_fb_out = std::shared_ptr<framebuffer>(new framebuffer(shape[1], shape[0], heatmap_tex_out));

		while(!glfwWindowShouldClose(g->window))
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			g->imgui_init();
			generate_imgui_windows();


			ImGui::Begin("plot settings");

			gpu_h->colormap_shader->generate_imgui_editor();

			ImGui::LabelText("RDM Settings", "");

			static double heatmap_min=0.0f;
			static double heatmap_max=1.0f;

			
			static int num_bins=100;
			ImGui::InputScalar("Histogram Bin Count", ImGuiDataType_S32, &num_bins);

			ImPlotSpec hist_spec;
			//hist_spec.Flags |= ImPlotHistogramFlags_Horizontal;
			//ImGui::SameLine();
			ImPlot::SetNextAxisLinks(ImAxis_X1, nullptr, nullptr);
			ImPlot::SetNextAxisLinks(ImAxis_Y1, nullptr, nullptr);
			if (ImPlot::BeginPlot("histogram", ImVec2(-1, 0), ImPlotFlags_Crosshairs))
			{
				ImPlot::PlotHistogram("histo test", data_dB.data(), shape[1]*shape[0], num_bins, 1.0, ImPlotRange());
				
				ImPlot::DragLineX(99990, &heatmap_min, ImVec4(1,0,0,1));
				ImPlot::DragLineX(99991, &heatmap_max, ImVec4(0,1,0,1));
				
				ImPlot::EndPlot();
			}
			ImGui::End();

			ImGui::Begin("test");

			static double drag_y_val = 0;	
			static double drag_x_val = 0;
			
			bool heatmap_hovered = false;
			static ImPlotPoint heatmap_hover_pos = {0,0};

			static float row_ratios[] = {0.8, 0.2};
			static float col_ratios[] = {0.8, 0.2};

			ImPlotSubplotFlags flags = ImPlotSubplotFlags_LinkRows | ImPlotSubplotFlags_LinkCols;
			if (ImPlot::BeginSubplots("linked rdm", 2, 2, {-1,1000}, flags, row_ratios, col_ratios))
			{
				ImPlot::PushColormap(ImPlotColormap_Viridis);

				if (ImPlot::BeginPlot("2D heatmap", ImVec2(-1, 0), ImPlotFlags_Crosshairs))
				{
					ImPlot::SetupAxes("X", "Y");

					ImPlot::SetupAxis(ImAxis_X1, "Bin index");
					// ImPlot::SetupAxis(ImAxis_X2, "Time [sec]", ImPlotAxisFlags_Lock);
					// ImPlot::SetupAxis(ImAxis_X3, "Range [m]");
					// ImPlot::SetupAxis(ImAxis_Y1, "Doppler");

					// Same horizontal span, expressed in each axis's units:
					// ImPlot::SetupAxisLimits(ImAxis_X1, 0, shape[1]);
					// ImPlot::SetupAxisLimits(ImAxis_X2, 0, 1/3e8);
					// ImPlot::SetupAxisLimits(ImAxis_X3, 0, 1000);

					// link all
					static double x_min = 0;
					static double x_max = 100.0;

					//note - setting the axis links somehow needs to work differently so that the
					//data stays in sync - the axis values will be different since the scales are different.
					// ImPlot::SetupAxisLinks(ImAxis_X3, &x_min, &x_max);
					// ImPlot::SetupAxisLinks(ImAxis_X2, &x_min, &x_max);
					// ImPlot::SetupAxisLinks(ImAxis_X1, &x_min, &x_max);

					// ImPlot::SetupAxisFormat(ImAxis_X3, (ImPlotFormatter)SI_formatter, (void*)"m");
					// ImPlot::SetupAxisFormat(ImAxis_X2, (ImPlotFormatter)SI_formatter, (void*)"s");

					gpu_h->reprocess(data_tex, heatmap_fb_out, heatmap_min, heatmap_max);

					// image generator currently upside-downs the image
					// this is because the data is loaded in opengl's bottom to top order -> flipping it.
					// either flip it again here or in the generator, or at load time.
					ImPlot::PlotImage("tex", heatmap_tex_out->gl_texture_id, ImPlotPoint(0, shape[0]), ImPlotPoint(shape[1], 0));
					//TODO: add no legend

					heatmap_hovered = ImPlot::IsPlotHovered();
					if (heatmap_hovered)
						heatmap_hover_pos = ImPlot::GetPlotMousePos();

					ImPlot::EndPlot();
				}

				ImPlot::PopColormap();

				// ImPlotSpec hist_spec;
				// hist_spec.Flags |= ImPlotHistogramFlags_Horizontal;
				// ImGui::SameLine();
				// if (ImPlot::BeginPlot("histogram", ImVec2(ImGui::GetWindowWidth()*0.3, ImGui::GetWindowHeight()), ImPlotFlags_Crosshairs))
				// {
				// 	ImPlot::PlotHistogram("histo test", data_dB.data(), shape[1]*shape[0], num_bins, 1.0, ImPlotRange(), hist_spec);
				// 	ImPlot::EndPlot();
				// }
				static size_t x_idx = 0;
				static size_t y_idx = 0;

				if (heatmap_hovered)
				{
					x_idx = heatmap_hover_pos.x;
					y_idx = heatmap_hover_pos.y;
				}

				//TODO probably use subplots here...
				// Marginal plots with selectable reduction operator:
				if (ImPlot::BeginPlot("Marginal Y", ImVec2(-1,100), ImPlotFlags_Crosshairs))
				{
					ImPlot::SetupAxisLimits(ImAxis_X1, data_min, data_max);
					
					if (x_idx >= 0 && x_idx < data_dB.shape()[1])
					{
						auto slice_1d = xt::view(data_dB, xt::all(), x_idx);
						auto y_axis = xt::arange<float>(slice_1d.size());
						
						// ImPlotSpec y_plot_spec;
						// y_plot_spec.Stride = int(slice_1d.strides()[0] * sizeof(float));
						// ImPlot::PlotLine("marginal y", slice_1d.data()+slice_1d.data_offset(), xs.data(), slice_1d.size(), 1, 0, y_plot_spec);
					
						struct vert_slice {
							const xt::xarray<float>& values;  // Plotted along the horizontal X-axis
							const xt::xarray<float>& y_coords; // Plotted along the vertical Y-axis
						};

						vert_slice plot_data{ slice_1d, y_axis };

						auto getter = [](int idx, void* user_data) -> ImPlotPoint {
							vert_slice* d = static_cast<vert_slice*>(user_data);
							return ImPlotPoint(d->values(idx), d->y_coords(idx));
						};

						ImPlot::PlotLineG("marginal y", getter, &plot_data, static_cast<int>(slice_1d.size()));
					
						float h_lines[] = {float(y_idx)};
						ImPlotSpec h_line_spec;
						h_line_spec.Flags |= ImPlotInfLinesFlags_Horizontal;
						h_line_spec.Flags |= ImPlotItemFlags_NoLegend;
						h_line_spec.LineColor = {0.8,0.8,0.8,1};
						h_line_spec.LineWeight = 0.1;
						ImPlot::PlotInfLines("crosshairs", h_lines, 1, h_line_spec);
					}
					ImPlot::EndPlot();
				}

				if (ImPlot::BeginPlot("Marginal X", ImVec2(-1,100), ImPlotFlags_Crosshairs))
				{
					// may want to set 'always' later
					ImPlot::SetupAxisLimits(ImAxis_Y1, data_min, data_max);

					if (y_idx >= 0 && y_idx < data_dB.shape()[0])
					{
						auto slice_1d = xt::view(data_dB, y_idx, xt::all());
						
						ImPlotSpec x_plot_spec;
						x_plot_spec.Stride = static_cast<int>(slice_1d.strides()[0] * sizeof(float));
						x_plot_spec.LineColor = {0.8,0.2,0.1,0.6};
						ImPlot::PlotLine("marginal x", slice_1d.data()+slice_1d.data_offset(), slice_1d.size(), 1, 0, x_plot_spec);
						
						float v_lines[] = {float(x_idx)};
						ImPlotSpec v_line_spec;
						v_line_spec.Flags |= ImPlotItemFlags_NoLegend;
						v_line_spec.LineColor = {0.8,0.8,0.8,1};
						v_line_spec.LineWeight = 0.1;
						ImPlot::PlotInfLines("crosshairs", v_lines, 1, v_line_spec);
					}
					ImPlot::EndPlot();
				}
				ImPlot::EndSubplots();
				
				ImPlot::SetNextAxisLinks(ImAxis_X1, nullptr, nullptr);
				ImPlot::SetNextAxisLinks(ImAxis_Y1, nullptr, nullptr);

				ImGui::SameLine();
				ImGui::LabelText("test text", "");
			}

			//TODO after plotting marginals, optionally plot slices from the H/V bars
			//Should be able to add a list of slices.

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
	wave_plot w = wave_plot(g);
	w.run();
	return 0;
}