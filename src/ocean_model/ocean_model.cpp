#include "ocean_model.h"

#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <gl_lib/texture.h>
#include <gl_lib/shader.h>
#include "gl_lib/framebuffer.h"
#include "gl_lib/primitives.h"

ocean_spectrum::ocean_spectrum(int Nx, int Ny)
{
    // load shader.
    ocean_spectrum_shader = std::unique_ptr<shader>(new shader("build/src/gl_lib/vertex.glsl", "build/src/ocean_model/ocean_spectrum.glsl"));

    // init empty texture
    ocean_spectrum_tex = std::shared_ptr<texture>(new texture(nullptr, Nx, Ny, 4, 4, GL_RGBA, GL_RGBA32F, GL_FLOAT));

    // init framebuffer
    ocean_spectrum_fb = std::shared_ptr<framebuffer>(new framebuffer(Nx, Ny, ocean_spectrum_tex));
}

void ocean_spectrum::generate_spectrum()
{
    auto& fullscreen_quad = primitives::get().fullscreen_quad;

    ocean_spectrum_shader->set_uniform("k_max", 100.0f*2.0f*3.14f*0.7f);
    ocean_spectrum_shader->set_uniform("k_wind_vector", glm::vec2(31,0));        

    ocean_spectrum_shader->set_uniform("model", glm::mat4(1.0));
    ocean_spectrum_shader->set_uniform("view", glm::mat4(1.0));
    ocean_spectrum_shader->set_uniform("projection", glm::mat4(1.0));

    ocean_spectrum_fb->bind_and_predraw();

    ocean_spectrum_shader->bind();
    ocean_spectrum_shader->set_imgui_uniforms();
    fullscreen_quad->draw();

    ocean_spectrum_fb->unbind();
    ocean_spectrum_fb->fb_texture->generate_mipmaps();
}

ocean_time_domain::ocean_time_domain(int Nx, int Ny)
{
    // load shader.
    ocean_time_domain_shader = std::unique_ptr<shader>(new shader("build/src/gl_lib/vertex.glsl", "build/src/ocean_model/ocean_time_domain.glsl"));

    // init empty texture
    ocean_time_domain_tex = std::shared_ptr<texture>(new texture(nullptr, Nx, Ny, 4, 4, GL_RGBA, GL_RGBA32F, GL_FLOAT));

    // init framebuffer
    ocean_time_domain_fb = std::shared_ptr<framebuffer>(new framebuffer(Nx, Ny, ocean_time_domain_tex));
}

void ocean_time_domain::compute_spectrum_realization(std::shared_ptr<texture> ocean_spectrum_realization_tex,
                                      float time_s)
{
    auto& fullscreen_quad = primitives::get().fullscreen_quad;

    ocean_time_domain_shader->set_uniform("ocean_spectrum_complex_realization", ocean_spectrum_realization_tex.get());
    ocean_time_domain_shader->set_uniform("k_max", 100.0f*2.0f*3.14f*0.7f);

    ocean_time_domain_shader->set_uniform("t", time_s/10.0f);
    
    ocean_time_domain_shader->set_uniform("model", glm::mat4(1.0));
    ocean_time_domain_shader->set_uniform("view", glm::mat4(1.0));
    ocean_time_domain_shader->set_uniform("projection", glm::mat4(1.0));

    ocean_time_domain_fb->bind_and_predraw();

    ocean_time_domain_shader->bind();
    ocean_time_domain_shader->set_imgui_uniforms();
    fullscreen_quad->draw();

    ocean_time_domain_fb->unbind();
    ocean_time_domain_fb->fb_texture->generate_mipmaps();
}