#version 330

in vec2 uv;
in vec4 vpoint_mv;
in float height;
in vec3 light_dir, view_dir;
in vec3 normal_mv;
in vec3 normal;

uniform sampler2D tex_color;

out vec3 color;

void main() {
    vec3 norm = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);

    float slope = 1.0 - normalize(normal).z;

    float height = height * 2.4;

    if (height == 0.0)
        color = vec3(0.0, 0.0, 1.0);
    else
        color = texture(tex_color, vec2(height, slope)).rgb;

    color = color / 1.2;

    float nl = dot(norm, light_dir);
    if (nl > 0.0) {
        color += nl * vec3(0.5);

        // Add reflection on water and snow
        bool is_water = (height <= 0.0);
        bool is_snow = (height > 0.5 && dot(vec3(1.0), color) > 2.0);
        if (is_water || is_snow) {
            float rv = dot(reflect(-light_dir, norm), view_dir);
            color += pow(max(0.0, rv), 60.0) * vec3(0.8);
        }
    }


}
