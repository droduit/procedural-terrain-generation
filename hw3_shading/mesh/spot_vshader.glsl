#version 330

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

in vec3 vpoint;
in vec3 vnormal;

out vec3 normal_mv;
out vec3 light_dir;
out vec3 view_dir;

void main() {
    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;

    normal_mv = mat3(inverse(transpose(MV))) * vnormal;
    light_dir = light_pos - vpoint_mv.xyz;
    view_dir = -vpoint_mv.xyz;
}
