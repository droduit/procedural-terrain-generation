#version 330

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

void main() {
    gl_Position = vec4(vpoint, 1.0);
    uv = vtexcoord;
}


