#version 330

in vec2 uv;
in vec4 vpoint_mv;
in float height;
in vec3 light_dir, view_dir;
in vec3 normal_mv;
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

    float height = height * 2.4;

    if (height == 0.0)
        color = vec3(0.0, 0.0, 1.0);
    else
        color = texture(tex_color, vec2(height, pow(slope, 1.5))).rgb;

    color = color * fcolor;

    float nl = dot(norm, light_dir);
    if (nl > 0.0) {
        color += nl * vec3(diffuse);

        // Add reflection on water and snow
        bool is_water = (height <= 0.0);
        bool is_snow = (height > hsnow && dot(vec3(1.0), color) > fsnow);
        if (is_water || is_snow) {
            float rv = dot(reflect(-light_dir, norm), view_dir);
            color += pow(max(0.0, rv), alpha) * vec3(specular);
        }
    }
}
