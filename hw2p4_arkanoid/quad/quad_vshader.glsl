#version 330

#define M_PI 3.1415926535897932384626433832795

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;
out vec4 fcolor;

uniform float time;
uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(vpoint, 1.0);
    uv = vtexcoord;
    fcolor = vec4(
        (sin(time + gl_Position.x) + 1.0) / 2.0,
        (sin(time + gl_Position.y  + M_PI / 3.0) + 1.0) / 2.0,
        (sin(time + M_PI * 2.0 / 3.0) + 1.0) / 2.0,
        1.0
    );
}
