#version 330 core
in vec2 uv;
uniform sampler2D colorTex;
uniform sampler2D velocityTex;
out vec4 color;

#define N_SAMPLES (10)
#define BLUR_FACTOR (0.5f)

void main() {
    color = vec4(0.0f);
    vec2 vel = BLUR_FACTOR*texture(velocityTex, uv).xy;

    for (int i = 0; i < N_SAMPLES; ++i) {
        color += texture(colorTex, uv + vel*(float(i)/N_SAMPLES));
    }
    color /= N_SAMPLES;
}
