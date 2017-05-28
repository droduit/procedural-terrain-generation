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

mat4 T(float tx, float ty, float tz) {
    mat4 T = mat4(1);
    T[3][0] = tx;
    T[3][1] = ty;
    T[3][2] = tz;
    return T;
}

void main() {
    vec4 transformed = T(0, -cam_pos.z, 0) * S(300, 300, 300) * vec4(vpoint, 1.0);
    gl_Position = MVP * transformed;
    texCoord = transformed.xyz;
}
