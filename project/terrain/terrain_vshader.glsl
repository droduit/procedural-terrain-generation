#version 330

in vec2 position;

out vec2 uv;
out vec4 vpoint_mv;
out float height;
out vec3 light_dir, view_dir;
out vec3 normal_mv;
out vec3 cam_pos_mv;
out vec3 normal;
out vec4 vpoint_lightspace;

uniform sampler2D heightmap;

uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform mat4 light_matrix;
uniform vec3 cam_pos;
uniform vec3 light_pos;
uniform float tesselation, area;
uniform vec4 clip_plane;

void main() {
    uv = (position + vec2(area / 2)) / area;
    height = texture(heightmap, uv).r;

    mat4 MV = view * model;
    vec4 point = vec4(position, height, 1.0);
    vpoint_mv = MV * point;
    cam_pos_mv = (MV * vec4(cam_pos, 1.0)).xyz;
    gl_Position = projection * vpoint_mv;

    vpoint_lightspace = light_matrix * model * point;

    gl_ClipDistance[0] = dot(point, clip_plane);

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

    normal_mv = inverse(transpose(mat3(MV))) * normal;
}
