#version 330

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vpoint, 1.0);
    uv = vtexcoord;
}
