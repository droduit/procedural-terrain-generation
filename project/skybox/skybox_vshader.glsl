#version 330 core

in vec3 vpoint;
in vec2 vtexcoord;

uniform mat4 MVP;

out vec2 uv;


mat4 T(float tx, float ty, float tz) {
    mat4 T = mat4(1);
    T[3][0] = tx;
    T[3][1] = ty;
    T[3][2] = tz;
    return T;
}

mat4 S(float tx, float ty, float tz){
    mat4 T = mat4(1);
    T[0][0] = tx;
    T[1][1] = ty;
    T[2][2] = tz;
    return T;
}

void main() {
    gl_Position = MVP * T(0,1,1) * S(100,250,150) * vec4(vpoint,1);
    uv = vtexcoord;
}
