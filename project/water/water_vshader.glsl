#version 330 core

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

in vec3 vpoint;
in vec2 vtexcoord;
out vec2 uv;

void main() {
    gl_Position = projection * view * model * vec4(vpoint, 1.0);
    uv = vtexcoord;
}
