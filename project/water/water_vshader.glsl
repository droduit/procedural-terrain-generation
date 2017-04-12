#version 330 core

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform float tesselation, area;
uniform sampler2D heightmap;

in vec2 position;

out vec2 uv;
out float height;

void main() {
    gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
    uv = (position + vec2(area / 2)) / area;
    height = texture(heightmap, uv).r;
}
