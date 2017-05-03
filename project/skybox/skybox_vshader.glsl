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


mat4 R(float degrees){
    mat3 R = mat3(1);
    float alpha = radians(degrees);
    R[0][0] =  cos(alpha);
    R[0][1] =  sin(alpha);
    R[1][0] = -sin(alpha);
    R[1][1] =  cos(alpha);
    return mat4(R);
}

void main() {
    gl_Position = MVP * R(90) * T(0,1,0) * S(100,250,150) * vec4(vpoint,1);
    uv = vtexcoord;
}
