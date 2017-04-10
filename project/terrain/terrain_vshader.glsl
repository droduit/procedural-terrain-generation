#version 330

in vec2 position;

out vec2 uv;
out vec4 vpoint_mv;
out float height;
out vec3 light_dir, view_dir;
out vec3 normal_mv;
out vec3 vert_mv;
out vec3 normal;

uniform sampler2D heightmap;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;
uniform float tesselation, area;

void main() {
    uv = (position + vec2(area / 2)) / area;
    height = texture(heightmap, uv).r;

    mat4 MV = view * model;
    vpoint_mv = MV * vec4(position, max(0.0, height), 1.0);
    gl_Position = projection * vpoint_mv;

    float sx0 = textureOffset(heightmap, uv, ivec2(-1 ,0)).r;
    float sx1 = textureOffset(heightmap, uv, ivec2( 1, 0)).r;
    float sy0 = textureOffset(heightmap, uv, ivec2( 0,-1)).r;
    float sy1 = textureOffset(heightmap, uv, ivec2( 0, 1)).r;

    vec3 vx = vec3(area / (2 * tesselation), 0, (sx1 - sx0));
    vec3 vy = vec3(0, area / (2 * tesselation), (sy1 - sy0));

    normal = cross(vx, vy);

    vec4 light_pos_mv = MV * vec4(light_pos, 1.0);

    light_dir = light_pos_mv.xyz - vpoint_mv.xyz;
    view_dir = -vpoint_mv.xyz;

    mat3 MVT = inverse(transpose(mat3(MV)));

    normal_mv = MVT * normal;
    vert_mv = MVT * vec3(0.0, 0.0, 1.0);
}
