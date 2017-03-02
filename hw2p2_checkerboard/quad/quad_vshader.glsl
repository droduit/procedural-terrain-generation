#version 330

in vec3 vpoint;
in vec2 vtexcoord;

out vec2 uv;

mat4 S(float s) {
    return mat4(mat3(s));
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
    gl_Position = S(0.8) * R(10) * vec4(vpoint, 1.0);
    uv = vtexcoord;
}
