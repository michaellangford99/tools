#version 330 core

#include "noise.glsl"

out vec4 FragColor;
in vec2 TexCoord;

uniform vec2 k_wind_vector;
uniform float k_max;

// gravity, [m/sec^2]
#define g 9.81f

//TODO add noise texture input,
// or generate it ourself

//read: https://agupubs.onlinelibrary.wiley.com/doi/full/10.1029/2018JC014804
// https://www.oceanopticsbook.info/view/surfaces/level-2/wave-variance-spectra-examples
// https://jtessen.people.clemson.edu/reports/papers_files/simdoc.pdf

// what are omega's units?
float S_jonswap(float omega, float k_wind_speed)
{
    // wind speed at 10m
    float U_10 = k_wind_speed;//0;

    // 'Fetch' or distance from lee shore [m]
    float F = 10000;

    // coefficients
    float alpha = 0.076*pow(U_10*U_10/(F*g), 0.22);
    float omega_p = 22.0*pow(g*g / (U_10*F), 1.0/3.0);// peak wave frequency (? in cycles/sec?)
    float gamma = 3.3;
    float sigma_1 = 0.07;
    float sigma_2 = 0.09;

    float sigma = (omega <= omega_p) ? sigma_1 : sigma_2;

    float r = exp(-pow(omega-omega_p, 2.0) / (2.0*sigma*sigma*omega_p*omega_p));

    // spectrum in m^2 / Hz
    // since power spectrum is units^2
    float S = (alpha*g*g / pow(omega, 5.0)) * exp((-5.0/4.0)*pow(omega_p/omega, 4.0))*pow(gamma, r);

    return S;
}

// float S_phillips(float omega, float k_wind_speed)
// {
//     float L = 1000;
//     float S = exp(-1.0f / (k_wind_speed*k_wind_speed*L*L)) /  pow(k_wind_speed, 4.0f);
//     return S;
// }

// S(w)dw = S(k)dk
// S(k) = S(w)dw / dk
// w^2 = gk
// 2w = g
float dw_over_dk(float omega)
{
    return 2.0f*omega/g;
}

float k_to_omega(float k)
{
    // use dispersion relation
    float omega = sqrt(g*k);
    return omega;
}

void main()
{
    vec2 k = k_max * (2.0f*TexCoord-1.0f);
    vec2 u_k = normalize(k);

    vec2 omega = u_k*k_to_omega(length(k));

    vec2 u_omega = u_k;
    vec2 u_k_wind_vector = normalize(k_wind_vector);

    float S_omega = S_jonswap(length(omega), length(k_wind_vector))*pow(abs(dot(u_omega, u_k_wind_vector)), 1);
    float S_k = S_omega * dw_over_dk(length(omega));

    // I believe S_w is a one-sided spectrum...
    // need to handle the nyquist and 0 frequency correctly though.

    // I figure it's best to represent a complex spectrum that's a generated
    // realization of a power spectrum  w the assumption that the underlying 
    // statistic that is complex circular gaussian? but I truly have no idea

    vec2 unif_rand = hash22(TexCoord);
    vec2 gaussian_2d = box_muller(unif_rand);

    vec2 cx_circ_gaussian = gaussian_2d / sqrt(2.0f);

    vec2 amplitude = sqrt(S_k / 2.0f) * cx_circ_gaussian;

    FragColor = vec4(amplitude*1000.0f, 0, 1);
}