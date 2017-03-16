#version 330

in vec3 vpoint;
in vec3 vnormal;


uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

void main() {
    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO: Artistic shading.
    // 1) compute normal_mv using the model_view matrix.
    // 2) compute the light direction light_dir.
    // 3) compute the view direction view_dir.
    //<<<<<<<<<< TODO <<<<<<<<<<<
}
