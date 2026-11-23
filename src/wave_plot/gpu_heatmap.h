#pragma once

#include <gl_lib/primitives.h>
#include <gl_lib/triangles.h>
#include <gl_lib/framebuffer.h>
#include <memory>

class gpu_heatmap
{
public:
	std::unique_ptr<shader> colormap_shader;

//need an output texture instance for the fb to bind to

	// user must call reprocess I guess??
	//
public:
	gpu_heatmap();
	void reprocess(std::shared_ptr<texture> data_in, std::shared_ptr<framebuffer> fb_out, float min, float max);
};