#version 330
#define PI 3.141592

in float height;
in vec4 vpoint_mv;
in vec4 vpoint_lightspace;
in vec3 cam_pos_mv;
in vec3 light_dir, view_dir;
in vec3 normal_mv;
in vec3 normal;
in vec2 uv;

uniform vec3 cam_pos;
uniform sampler2D tex_color, tex_snow_color;
uniform sampler2D shadows;
uniform float fheight, fslope, fcolor, hsnow, fsnow;
uniform float diffuse, specular, alpha;

uniform vec3 fog_color;
uniform vec3 diffuse_color;
uniform float fog_start, fog_end, fog_density, fog_power;
uniform int fog_type;

uniform vec2 hoffset;
uniform sampler2D grass_tex;
uniform sampler2D sand_tex;
uniform sampler2D rock_tex;
uniform sampler2D snow_tex;

uniform vec3 light_pos;
uniform float light_bias_min, light_bias_max;

out vec4 out_color;

void main() {
    vec3 norm = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);

    // fetch ground textures
    vec2 uv = uv + hoffset / 6.0; // divide by hcomp

    vec4 sand  = texture(sand_tex,  30 * uv);
    vec4 grass = texture(grass_tex,  100 * uv);
    vec4 rock  = texture(rock_tex,  30 * uv);
    vec4 snow  = texture(snow_tex,  10 * uv);

    // fetch texture mix
    float slope = 1.0 - normalize(normal).z;
    float height = height * fheight;

    vec3 color = texture(tex_color, vec2(height, pow(slope, fslope))).rgb;
    vec3 snowAmount = texture(tex_snow_color, vec2(height, pow(slope, fsnow))).rgb;

    // mix textures
    out_color = vec4(0.0);
    out_color += sand  * color.r;
    out_color += grass * color.g;
    out_color += rock  * color.b;

    if (height >= hsnow)
        out_color = mix(out_color, snow, step(0.0001, snowAmount.r) * snowAmount.r);

    out_color = out_color / dot(vec3(1.0), color.rgb);

    if (light_pos.z > 0.0) {
        // compute shadows
        vec3 lightCoords = (vpoint_lightspace.xyz / vpoint_lightspace.w + 1.0) / 2.0;
        float closestDepth = texture(shadows, lightCoords.xy).r;
        float bias = max(light_bias_max * (1.0 - dot(norm, light_dir)), light_bias_min);
        float shadow = (lightCoords.z - bias) < closestDepth ? 1.0 : 0.0;

        // compute diffuse
        if (shadow == 0.0)
            out_color *= vec4(0.7, 0.7, 0.7, 1.0);
        else
            out_color += dot(norm, light_dir) * vec4(2 * diffuse * diffuse_color, 1.0) * shadow;
    }

    // force sand color underwater
    if (height < 0.0) {
        out_color = mix(out_color, sand, 1.0 - smoothstep(-0.3, 0.0, height));
        out_color = mix(out_color, vec4(0.5, 0.7, 0.9, 1.0), 0.4);
    }

    // add fog
    float distance = length(cam_pos_mv - vpoint_mv.xyz);
    float fog_factor = 1.0 - exp(-pow(fog_density * distance, fog_power));

    out_color = mix(out_color, vec4(fog_color, 1.0), clamp(fog_factor, 0.0, 1.0));
}
