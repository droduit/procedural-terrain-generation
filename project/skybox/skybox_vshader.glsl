#version 330 core

in vec3 vpoint;

uniform mat4 MVP;
uniform float horizon_z;
uniform vec3 cam_pos;

out vec3 texCoord;

mat4 S(float tx, float ty, float tz){
    mat4 T = mat4(1);
    T[0][0] = tx;
    T[1][1] = ty;
    T[2][2] = tz;
    return T;
}

void main() {
    gl_Position = MVP * S(300, 300, 300) * vec4(vpoint,1);
    texCoord = vpoint;
}
