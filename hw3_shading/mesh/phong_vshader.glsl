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
    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 1.1: Phong shading.
    /// 1) compute normal_mv using the model_view matrix.
    normal_mv = normalize(mat3(inverse(transpose(MV))) * vnormal);
    /// 2) compute the light direction light_dir.
    light_dir = light_pos - vpoint_mv.xyz;
    /// 3) compute the view direction view_dir.
    view_dir = -vpoint_mv.xyz;
    ///<<<<<<<<<< TODO <<<<<<<<<<<

}
