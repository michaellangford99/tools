#define PI 3.1415926535897932f

uint pcg_hash(uint input_val)
{
    uint state = input_val * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float uintToFloat(uint x) {
    return uintBitsToFloat((x >> 9u) | 0x3F800000u) - 1.0;
}

// 1D float input -> uniform float in [0.0, 1.0)
float hash11(float p) {
    return uintToFloat(pcg_hash(floatBitsToUint(p)));
}

// 2D vec2 input -> uniform float in [0.0, 1.0)
float hash12(vec2 p) {
    uvec2 u = floatBitsToUint(p);
    return uintToFloat(pcg_hash(u.x + pcg_hash(u.y)));
}

// 3D vec3 input -> uniform float in [0.0, 1.0)
float hash13(vec3 p) {
    uvec3 u = floatBitsToUint(p);
    return uintToFloat(pcg_hash(u.x + pcg_hash(u.y + pcg_hash(u.z))));
}

// 4D vec4 input -> uniform float in [0.0, 1.0)
float hash14(vec4 p) {
    uvec4 u = floatBitsToUint(p);
    return uintToFloat(pcg_hash(u.x + pcg_hash(u.y + pcg_hash(u.z + pcg_hash(u.w)))));
}

vec2 hash22(vec2 p) {
    uvec2 u = floatBitsToUint(p + 0.0);
    uint h1 = pcg_hash(u.x + pcg_hash(u.y));
    uint h2 = pcg_hash(u.y + pcg_hash(u.x));
    return vec2(uintToFloat(h1), uintToFloat(h2));
}

vec2 box_muller(vec2 unif_rand)
{
    if (unif_rand.x <= 0.0f)
        unif_rand.x = 1e-10;

    float mag = sqrt(-2.0f*log(unif_rand.x));
    float z0 = mag * cos(2.0f*PI*unif_rand.y);
    float z1 = mag * sin(2.0f*PI*unif_rand.y);

    return vec2(z0, z1);
}