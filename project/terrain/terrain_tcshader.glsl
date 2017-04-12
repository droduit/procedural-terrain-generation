#version 400

layout (vertices = 4) out;

in vec2 uv[];
in float height[];
in vec4 vpoint_mv[];
in vec3 cam_pos_mv[];
in vec3 light_dir[];
in vec3 view_dir[];
in vec3 normal_mv[];
in vec3 vert_mv[];
in vec3 normal[];

patch out vec2 patch_uv;
patch out vec4 patch_vpoint_mv;
patch out float patch_height;
patch out vec3 patch_light_dir;
patch out vec3 patch_view_dir;
patch out vec3 patch_normal_mv;
patch out vec3 patch_vert_mv;
patch out vec3 patch_cam_pos_mv;
patch out vec3 patch_normal;

void main() {
    gl_TessLevelInner[0] = 8;
    gl_TessLevelInner[1] = 8;

    gl_TessLevelOuter[0] = 2;
    gl_TessLevelOuter[1] = 4;
    gl_TessLevelOuter[2] = 6;
    gl_TessLevelOuter[3] = 8;
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

    patch_uv         = uv[gl_InvocationID];
    patch_vpoint_mv  = vpoint_mv[gl_InvocationID];
    patch_height     = height[gl_InvocationID];
    patch_light_dir  = light_dir[gl_InvocationID];
    patch_view_dir   = view_dir[gl_InvocationID];
    patch_normal_mv  = normal_mv[gl_InvocationID];
    patch_vert_mv    = vert_mv[gl_InvocationID];
    patch_cam_pos_mv = cam_pos_mv[gl_InvocationID];
    patch_normal     = normal[gl_InvocationID];
}
