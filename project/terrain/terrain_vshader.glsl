#version 330

in vec2 position;

out vec2 uv;

uniform mat4 MVP;

void main() {
    uv = (position + vec2(10.0)) / 20.0;
    gl_Position = MVP * vec4(position.x, position.y, 0.0, 1.0);
}
