#include "gpu_heatmap.h"
#include <gl_lib/primitives.h>
#include <gl_lib/triangles.h>
#include <gl_lib/framebuffer.h>
#include <memory>

gpu_heatmap::gpu_heatmap()
{
    // (init geometry and shaders here)
    colormap_shader = std::unique_ptr<shader>(new shader("build/src/wave_plot/vertex.glsl", "build/src/wave_plot/heatmap.glsl"));
}

void gpu_heatmap::reprocess(std::shared_ptr<texture> data_in, std::shared_ptr<framebuffer> fb_out, float min, float max)
{
    auto& fullscreen_quad = primitives::get().fullscreen_quad;

    // //height_map = std::unique_ptr<texture>(new texture(terrain_file, 3601, 3601, 1, 2, GL_RED, GL_R32F, GL_SHORT, true));

    // fb_out->fb_texture = std::shared_ptr<texture>(new texture(nullptr, x, y, 4, 2, GL_RED, GL_R32F, GL_FLOAT));
    // //---- generate normal map from heightmap ----

    // framebuffer fb = framebuffer(x, y, colormap_output);

    colormap_shader->set_uniform("pmin", min);
    colormap_shader->set_uniform("pmax", max);
    colormap_shader->set_uniform("tex0", data_in.get());
    colormap_shader->set_uniform("model", glm::mat4(1.0));
    colormap_shader->set_uniform("view", glm::mat4(1.0));
    colormap_shader->set_uniform("projection", glm::mat4(1.0));

    fb_out->bind_and_predraw();

    colormap_shader->bind();
    colormap_shader->set_imgui_uniforms();
    fullscreen_quad->draw();

    fb_out->unbind();
    fb_out->fb_texture->generate_mipmaps();
}
