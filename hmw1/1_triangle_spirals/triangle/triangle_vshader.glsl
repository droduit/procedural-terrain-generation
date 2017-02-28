#version 330

in vec3 vpoint;

void main() {
    gl_Position = vec4(vpoint, 1.0);
}
