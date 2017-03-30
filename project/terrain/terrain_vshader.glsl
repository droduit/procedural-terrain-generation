#version 330

in vec2 position;

out vec2 uv;
out vec4 vpoint_mv;
out float height;
out vec3 light_dir, view_dir;

uniform sampler2D heightmap;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

void main() {
    uv = (position + vec2(2.0)) / 4.0;
    height = max(0.0, texture(heightmap, uv).r - 0.4);

    mat4 MV = view * model;
    vpoint_mv = MV * vec4(position, height, 1.0);
    gl_Position = projection * vpoint_mv;

    vec4 light_pos_mv = MV * vec4(light_pos, 1.0);

    light_dir = light_pos_mv.xyz - vpoint_mv.xyz;
    view_dir = -vpoint_mv.xyz;
}
