#version 330 core
in vec2 uv;
uniform sampler2D tex;
out vec3 color;


void main() {
    color = texture(tex, uv).rgb;
}
