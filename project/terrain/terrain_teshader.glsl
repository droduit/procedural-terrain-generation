#version 400

layout(quads, equal_spacing, ccw) in;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 cam_pos;
uniform vec3 light_pos;
uniform float tesselation, area;

patch in vec2 patch_uv;
patch in vec4 patch_vpoint_mv;
patch in float patch_height;
patch in vec3 patch_light_dir, patch_view_dir;
patch in vec3 patch_normal_mv;
patch in vec3 patch_vert_mv;
patch in vec3 patch_cam_pos_mv;
patch in vec3 patch_normal;

out vec2 uv;
out vec4 vpoint_mv;
out float height;
out vec3 light_dir, view_dir;
out vec3 normal_mv;
out vec3 vert_mv;
out vec3 cam_pos_mv;
out vec3 normal;

vec4 interpolate(in vec4 v0, in vec4 v1, in vec4 v2, in vec4 v3)
{
    vec4 a = mix(v0, v1, gl_TessCoord.x);
    vec4 b = mix(v3, v2, gl_TessCoord.x);
    return mix(a, b, gl_TessCoord.y);
}

void main()
{
    gl_Position = interpolate(
        gl_in[0].gl_Position,
        gl_in[1].gl_Position,
        gl_in[2].gl_Position,
        gl_in[3].gl_Position);

    uv         = patch_uv;
    vpoint_mv  = patch_vpoint_mv;
    height     = patch_height;
    light_dir  = patch_light_dir;
    view_dir   = patch_view_dir;
    normal_mv  = patch_normal_mv;
    vert_mv    = patch_vert_mv;
    cam_pos_mv = patch_cam_pos_mv;
    normal     = patch_normal;
}
