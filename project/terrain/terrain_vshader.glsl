#version 330

in vec2 position;

out vec2 uv;
out vec4 fragPos;

uniform mat4 MVP;
uniform sampler2D heightmap;

void main() {
    uv = (position + vec2(2.0)) / 4.0;
    fragPos = vec4(position.x, position.y, max(0.0, texture(heightmap, uv).r - 0.4), 1.0);
    gl_Position = MVP * fragPos;
}
