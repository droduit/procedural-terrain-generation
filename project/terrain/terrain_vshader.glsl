#version 330
#define M_PI 3.1415926535897932384626433832795

// Source of the Gerstner Waves implementation: http://http.developer.nvidia.com/GPUGems/gpugems_ch01.html
#define ENABLE_GERSTNER_WAVES

in vec2 position;

out vec2 uv;

uniform mat4 MVP;
uniform float time;

void main() {
    uv = (position + vec2(1.0)) * 0.5;

    vec3 pos_3d = vec3(position.x, position.y, 0.0);

#ifndef ENABLE_GERSTNER_WAVES

    pos_3d[2] = 0.2 * sin(uv.x * 3 + time) * cos(uv.y * 3 + time * 0.5);

#else

    #define WAVES_COUNT 3
    struct Wave {
        vec2 direction;
        float frequency;
        float amplitude;
        float speed;
        float steepness;
    } waves[WAVES_COUNT];

    // Settings for the waves
    waves[0].direction = normalize(vec2(3.0, -2.0));
    waves[0].frequency = M_PI;
    waves[0].amplitude = 0.08;
    waves[0].speed = M_PI / 4.0;
    waves[0].steepness = 0.6;

    waves[1].direction = normalize(vec2(3.0, -1.0));
    waves[1].frequency = M_PI / 1.5;
    waves[1].amplitude = 0.06;
    waves[1].speed = M_PI / 3.0;
    waves[1].steepness = 0.4;

    waves[2].direction = normalize(vec2(2.0, -2.0));
    waves[2].frequency = M_PI / 2.0;
    waves[2].amplitude = 0.06;
    waves[2].speed = M_PI / 4.5;
    waves[2].steepness = 1.0;

    // Computing the effects of the waves
    int i = 0;
    for (i = 0; i < WAVES_COUNT; i++) {
        float QA = waves[i].amplitude * waves[i].steepness
                 / (waves[i].frequency * waves[i].amplitude * WAVES_COUNT);

        float sigma = waves[i].frequency * dot(waves[i].direction, position)
                    + waves[i].speed * time;

        pos_3d[0] += waves[i].direction.x * QA * cos(sigma);
        pos_3d[1] += waves[i].direction.y * QA * cos(sigma);
        pos_3d[2] += waves[i].amplitude * sin(sigma);
    }

#endif

    gl_Position = MVP * vec4(pos_3d, 1.0);
}
