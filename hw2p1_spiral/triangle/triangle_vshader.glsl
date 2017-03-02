#version 330

in vec3 vpoint;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vpoint, 1.0);
}
