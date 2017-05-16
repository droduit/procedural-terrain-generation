#version 330 core

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform vec3 cam_pos;
uniform float tesselation, area;
uniform sampler2D heightmap;
uniform sampler2D normal;

in vec2 position;

out vec2 uv;
out float height;
out vec4 vpoint_mv;
out vec4 cam_pos_mv;
out vec3 norm;
out float fresnel;

uniform float water_fb_ratio;

void main() {
    mat4 MV = view * model;

    uv = (position + vec2(area / 2)) / area;
    //   vec2 dudv = texture(normal, uv).xy * 2 - 1;
    gl_Position = projection * MV * vec4(position, 0.0, 1.0);
    height = 0; // texture(heightmap, uv).r;

    uv = uv * water_fb_ratio;

    vpoint_mv = model * vec4(position, height, 1.0);
    cam_pos_mv = model * vec4(cam_pos, 1.0);

    //norm = texture(normal_map, uv).rgb;
    norm = vec3(0.0f, 0.0f, 1.0f);
    fresnel = dot(normalize((cam_pos_mv - vpoint_mv).xyz), norm);
}
