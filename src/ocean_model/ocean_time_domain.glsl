#version 330 core

#include "complex.glsl"
#include "noise.glsl"

out vec4 FragColor;
in vec2 TexCoord;

uniform float k_max;
uniform sampler2D ocean_spectrum_complex_realization;
uniform float t;

float deep_water_dispersion_omega(float k)
{
    float g = 9.81f;
    return sqrt(g*k);
}

float deep_water_dispersion_k(float omega)
{
    float g = 9.81f;
    return omega*omega / g;
}

void main()
{
    vec2 h_k = texture(ocean_spectrum_complex_realization, TexCoord).xy;
    vec2 k = k_max * (2.0f*TexCoord-1.0f);

    // complex conjugate of h(-k)
    vec2 h_hermite_k = texture(ocean_spectrum_complex_realization, vec2(1,1) - TexCoord).xy;
    h_hermite_k.y = -h_hermite_k.y;

    float omega = deep_water_dispersion_omega(length(k));

    vec2 e_neg_jwt = cx_exp_i(-omega * t);

    vec2 h_total = (cx_mul(h_k, e_neg_jwt) + cx_mul(h_hermite_k, conj(e_neg_jwt))) / sqrt(2.0f);

    FragColor = vec4(h_total, 0, 1);
}