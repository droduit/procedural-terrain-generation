#version 330

in vec2 uv;
in vec4 vpoint_mv;
in float height;
in vec3 light_dir, view_dir;
in vec3 normal_mv;
in vec3 vert_mv;
in vec3 normal;

uniform sampler2D tex_color;
uniform float fheight, fslope, fcolor, hsnow, fsnow;
uniform float diffuse, specular, alpha;

out vec3 color;

void main() {
    vec3 norm = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);

    float slope = 1.0 - normalize(normal).z;
    float height = height * fheight;

    if (height > 0.0) {
        color = texture(tex_color, vec2(height, pow(slope, fslope))).rgb;
    } else {
        color = vec3(0.0, 0.0, 1.0);
        norm = vert_mv;
    }

    color = color * fcolor;

    float nl = dot(norm, light_dir);
    if (nl > 0.0) {
        color += nl * vec3(diffuse);

        // Add reflection on water and snow
        bool is_water = (height <= 0.0);
        bool is_snow = (height > hsnow && dot(vec3(1.0), color) > fsnow);
        if (is_water || is_snow) {
            float rv = dot(reflect(-light_dir, norm), view_dir);
            color += pow(max(0.0, rv), alpha) * vec3(specular) *
                     min(1.0, smoothstep(hsnow, hsnow + 0.1, height));
        }
    }
}
