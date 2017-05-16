#version 330 core
out vec4 out_color;

in float height;
in vec4 vpoint_mv;
in vec4 cam_pos_mv;
in vec3 norm; // TODO lookup normal here ?
in float fresnel;

uniform sampler2D heightmap;
uniform sampler2D reflection;

uniform vec3 fog_color;
uniform float fog_start, fog_end, fog_density, fog_power;
uniform int fog_type;
uniform float diffuse, specular, alpha;
uniform float water_fb_ratio;

void main() {

    vec2 uv_mirror = gl_FragCoord.xy / textureSize(reflection, 0) * water_fb_ratio;

    uv_mirror.y = 1.0 - uv_mirror.y;

    //float transparency = 0.8 - smoothstep(-2.4, 0.0, height) / 2.0;

    vec4 reflection_color = texture(reflection, uv_mirror);
    vec4 water_color = vec4(0.2, 0.3, 0.6, 1.0);

    vec3 camToWater = normalize(cam_pos_mv.xyz - vpoint_mv.xyz);
    // float mx = max(camToWater.x, max(camToWater.y, camToWater.z));
    // if (mx == camToWater.x) {
    // 	camToWater.yz = vec2(0,0);
    // } else if (mx == camToWater.y) {
    // 	camToWater.xz = vec2(0,0);
    // } else if (mx == camToWater.z) {
    // 	camToWater.xy = vec2(0,0);
    // }

    float mixing = dot(camToWater.xyz, vec3(0,0,1));
    vec3 color = mix(reflection_color, water_color, clamp(0, 0.7, fresnel)).rgb;

    // add fog
    //float distance = length(camToWater);
    //float fog_factor = 1.0 - exp(-pow(fog_density * distance, fog_power));

    //color = mix(color, fog_color, clamp(fog_factor, 0.0, 1.0));

    //out_color = vec4(color, transparency);
    out_color = vec4(color, 1 - fresnel);//vec4(0.0f);
    //color = out_color.xyz;
}
