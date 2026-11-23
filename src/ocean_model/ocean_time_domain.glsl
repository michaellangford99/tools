#version 330 core

#include "noise.glsl"

out vec4 FragColor;
in vec2 TexCoord;

uniform vec2 k_wind_vector;
uniform float omega_max;

uniform sampler2D ocean_spectrum_complex_realization;

float deep_water_dispersion__omega(float k)
{
    float g = 9.81f;
    return sqrt(g*k);
}

float deep_water_dispersion__k(float omega)
{
    float g = 9.81f;
    return omega*omega / g;
}



void main()
{
    vec4 h_omega = texture(ocean_spectrum_complex_realization, TexCoord);
    vec2 omega = omega_max * (2.0f*TexCoord-1.0f);

    vec2 k = normalize(omega) * deep_water_dispersion__k(length(omega))


}