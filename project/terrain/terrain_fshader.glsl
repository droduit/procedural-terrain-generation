#version 330

in vec2 uv;
in float height;
in vec4 vpoint_mv;
in vec3 cam_pos_mv;
in vec3 light_dir, view_dir;
in vec3 normal_mv;
in vec3 vert_mv;
in vec3 normal;

uniform vec3 cam_pos;
uniform sampler2D tex_color;
uniform float fheight, fslope, fcolor, hsnow, fsnow;
uniform float diffuse, specular, alpha;

uniform vec3 fog_color;
uniform float fog_start, fog_end, fog_density, fog_power;
uniform int fog_type;

out vec4 out_color;

void main() {
    vec3 norm = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);

    float slope = 1.0 - normalize(normal).z;
    float height = height * fheight;

    vec3 color = texture(tex_color, vec2(height, pow(slope, fslope))).rgb * fcolor;

    float nl = dot(norm, light_dir);
    if (nl > 0.0) {
        color += nl * vec3(diffuse);

        // Add reflection on water and snow
        if (height > hsnow && dot(vec3(1.0), color) > fsnow) {
            float rv = dot(reflect(-light_dir, norm), view_dir);
            vec3 spec = vec3(specular) * min(1.0, smoothstep(hsnow, hsnow + 0.1, height));

            color += pow(max(0.0, rv), alpha) * spec;
                     
        }
    }

    if (height < 0.0)
        color = mix(color, vec3(0.3, 0.3, 0.9), 0.6);

    // Fog
    float fog_factor = 0.0;
    float distance = length(cam_pos_mv - vpoint_mv.xyz);
    if (fog_type == 0) // Linear factor
        fog_factor = 1.0 - (fog_end - distance) / (fog_end - fog_start);
    else // Exp factor
        fog_factor = 1.0 - exp(-pow(fog_density * distance, fog_power));

    color = mix(color, fog_color, clamp(fog_factor, 0.0, 1.0));

    out_color = vec4(color, 1.0);
}
