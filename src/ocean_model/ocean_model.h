#pragma once

#include <memory>

#include <gl_lib/texture.h>
#include <gl_lib/shader.h>
#include "gl_lib/framebuffer.h"

class ocean_spectrum
{
    public:

    std::shared_ptr<texture> ocean_spectrum_tex;
    std::shared_ptr<shader> ocean_spectrum_shader;
    std::shared_ptr<framebuffer> ocean_spectrum_fb;

    ocean_spectrum(int Nx, int Ny);
    void generate_spectrum();
};

class ocean_time_domain
{
    public:

    std::shared_ptr<shader> ocean_time_domain_shader;
    std::shared_ptr<framebuffer> ocean_time_domain_fb;
    std::shared_ptr<framebuffer> ocean_time_domain_tex;

    ocean_spectrum();
    void compute_spectrum_realization(std::shared_ptr<texture> ocean_spectrum_realization_tex,
                                      float time_s);
}