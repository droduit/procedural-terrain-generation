#version 330 core
in vec3 texCoord;
uniform samplerCube skybox;
out vec4 color;

void main() {
    color = texture(skybox, texCoord);
}
