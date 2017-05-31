#version 330 core
out vec4 out_color;

in float height;
in vec4 vpoint_mv;
in vec4 cam_pos_mv;
in vec2 uv;

uniform sampler2D heightmap;
uniform sampler2D reflection;
uniform sampler2D normal_map;

uniform float dx;
uniform float dy;

uniform float time;
uniform vec3 fog_color;
uniform float fog_start, fog_end, fog_density, fog_power;
uniform int fog_type;
uniform float diffuse, specular, alpha;
uniform float water_fb_ratio;

void main() {

    vec2 uv_mirror = gl_FragCoord.xy / textureSize(reflection, 0) * water_fb_ratio;
    float border_transparency = smoothstep(-2.4, 0.0, height);

    vec3 normal_vec = texture(normal_map, (uv + vec2(dx,dy)*100*water_fb_ratio + time)/10).rgb * 2 - 1;
    normal_vec *= (1 - border_transparency); // soften the waves at the edges
    normal_vec.z += 4.5; // lesser amplitude
    normal_vec = normalize(normal_vec);
    vec3 camToWater = (cam_pos_mv - vpoint_mv).xyz;
    float fresnel = dot(normalize(camToWater), mix(normal_vec, vec3(0,0,1), 0.8));
    uv_mirror.y = 1.0 - uv_mirror.y;

    vec3 reflected_uv = reflect(vec3(uv_mirror, 0.0), normal_vec);
    vec4 reflection_color = texture(reflection, reflected_uv.xy);
    vec4 water_color = vec4(0.2, 0.3, 0.6, 1.0);

    vec3 color = reflection_color.rgb;

    // add fog
    float distance = length(camToWater);
    float fog_factor = 1.0 - exp(-pow(fog_density * distance, fog_power));
    color = mix(color, fog_color, clamp(fog_factor, 0.0, 1.0));

    float total_transparency = clamp(border_transparency/2 + fresnel, 0.0, 0.8);
    out_color = vec4(color, 1 - total_transparency);
    //color = out_color.xyz;
}
