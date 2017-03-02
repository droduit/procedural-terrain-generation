#version 330

in vec2 uv;

out vec3 color;

uniform sampler1D colormap;

void main() {
    color = vec3(1.0, 0.0, 0.0);
}


