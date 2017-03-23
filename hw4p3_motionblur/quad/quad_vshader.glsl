#version 330 core
in vec3 vpoint;
in vec2 vtexcoord;

uniform mat4 currentMVP;
uniform mat4 previousMVP;

out vec2 uv;
out vec4 currentPosition;
out vec4 previousPosition;

void main() {
    currentPosition = currentMVP * vec4(vpoint, 1.0);
    previousPosition = previousMVP * vec4(vpoint, 1.0);
    gl_Position = currentPosition;
    uv = vtexcoord;
}
