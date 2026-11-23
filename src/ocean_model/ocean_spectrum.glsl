#version 330 core

#include "noise.glsl"

out vec4 FragColor;
in vec2 TexCoord;

uniform vec2 k_wind_vector;
uniform float omega_max;

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

    // gravity, [m/sec^2]
    float g = 9.81;

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

float J_omega_to_k()
{
    // use dispersion relation
    
}

void main()
{
    vec2 omega = omega_max * (2.0f*TexCoord-1.0f);

    vec2 u_omega = normalize(omega);
    vec2 u_k_wind_vector = normalize(k_wind_vector);

    float S = S_jonswap(length(omega), length(k_wind_vector))*pow(abs(dot(u_omega, u_k_wind_vector)), 1);

    // I figure it's best to represent a complex spectrum that's a generated
    // realization of a power spectrum  w the assumption that the underlying 
    // statistic that is complex circular gaussian? but I truly have no idea

    vec2 unif_rand = hash22(TexCoord);
    vec2 gaussian_2d = box_muller(unif_rand);

    vec2 amplitude = sqrt(S) * gaussian_2d;

    FragColor = vec4(amplitude, 0, 1);
}