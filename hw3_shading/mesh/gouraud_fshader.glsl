#version 330

in vec3 vcolor;
in vec3 light_dir;
in vec3 view_dir;

out vec3 color;

void main() {
    color = vec3(0.0,0.0,0.0);
    color = vcolor;
}
