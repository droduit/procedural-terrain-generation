#version 330 core

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform vec3 cam_pos;
uniform float tesselation, area;
uniform sampler2D heightmap;
uniform sampler2D normal_map;

in vec2 position;

out vec2 uv;
out float height;
out vec4 vpoint_mv;
out vec3 cam_pos_mv;
out vec3 norm;

uniform float water_fb_ratio;

void main() {
    mat4 MV = view * model;
    vec4 point = vec4(position, height, 1.0);

    uv = (position + vec2(area / 2)) / area;
    gl_Position = projection * MV * vec4(position, 0.0, 1.0);
    height = texture(heightmap, uv).r;

	uv = uv * water_fb_ratio;

    vpoint_mv = MV * point;
    cam_pos_mv = (MV * vec4(cam_pos, 1.0)).xyz;
	norm = texture(normal_map, uv).rgb;
}
