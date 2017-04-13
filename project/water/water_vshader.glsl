#version 330 core

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform vec3 cam_pos;
uniform float tesselation, area;
uniform sampler2D heightmap;


in vec2 position;

out vec2 uv;
out float height;
out vec4 vpoint_mv;
out vec3 cam_pos_mv;

void main() {
    uv = (position + vec2(area / 2)) / area;
    gl_Position = projection * view * model * vec4(position, 0.0, 1.0);
    height = texture(heightmap, uv).r;


    mat4 MV = view * model;
    vec4 point = vec4(position, height, 1.0);


    vpoint_mv = MV * point;
    cam_pos_mv = (MV * vec4(cam_pos, 1.0)).xyz;
}
