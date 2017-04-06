#version 330

in vec2 position;

out vec2 uv;
out vec4 vpoint_mv;
out float height;
out vec3 light_dir, view_dir;
out vec3 normal;

uniform sampler2D heightmap;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

void main() {
    uv = (position + vec2(2.0)) / 4.0;
    height = max(0.0, texture(heightmap, uv).r);


    mat4 MV = view * model;
    vpoint_mv = MV * vec4(position, height, 1.0);
    gl_Position = projection * vpoint_mv;

    vec4 light_pos_mv = MV * vec4(light_pos, 1.0);

    light_dir = light_pos_mv.xyz - vpoint_mv.xyz;
    view_dir = -vpoint_mv.xyz;


    vec4 wave = texture(heightmap, uv);
    float s11 = wave.x;
    float s01 = textureOffset(heightmap, uv, ivec2(-1,0)).x;
    float s21 = textureOffset(heightmap, uv, ivec2(1,0)).x;
    float s10 = textureOffset(heightmap, uv, ivec2(0,-1)).x;
    float s12 = textureOffset(heightmap, uv, ivec2(0,1)).x;

    vec3 va = normalize(vec3(vec2(2.0,0.0), s21-s01));
    vec3 vb = normalize(vec3(vec2(0.0, 2.0), s12-s10));
    vec4 bump = vec4(cross(va, vb), s11);
    height = max(0.0, bump.a);
    normal = bump.xyz;

}
