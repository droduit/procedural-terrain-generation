#version 330 core
out vec4 out_color;

in float height;
in vec4 vpoint_mv;
in vec3 cam_pos_mv;

uniform sampler2D heightmap;
uniform sampler2D reflection;

uniform vec3 fog_color;
uniform float fog_start, fog_end, fog_density, fog_power;
uniform int fog_type;
uniform float diffuse, specular, alpha;


void main() {

    vec2 uv_mirror = gl_FragCoord.xy / textureSize(reflection, 0);

    uv_mirror.y = 1.0 - uv_mirror.y;

    float transparency = 0.8 - smoothstep(-2.4, 0.0, height) / 2.0;

    vec4 reflection_color = texture(reflection, uv_mirror);
    vec4 water_color = vec4(0.2, 0.3, 0.6, 1.0);

    vec3 color = mix(water_color, reflection_color, 0.2).rgb;

    // add fog
    float distance = length(cam_pos_mv - vpoint_mv.xyz);
    float fog_factor = 1.0 - exp(-pow(fog_density * distance, fog_power));

    color = mix(color, fog_color, clamp(fog_factor, 0.0, 1.0));

    out_color = vec4(color, transparency);
}
