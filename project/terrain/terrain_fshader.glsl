#version 330

in vec2 uv;
in vec4 vpoint_mv;
in float height;
in vec3 light_dir, view_dir;
in vec3 normal_mv;

uniform sampler2D tex_color;

out vec3 color;

void main() {
    vec3 norm = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);


    if (height <= 0.0)
        color = vec3(0.2, 0.3, 0.9) / 2.0; // Water
    else if (height < 0.04)
        color = vec3(0.8, 0.7, 0.2) / 2.0; // Sand
    else if (height < 0.28)
        color = vec3(0.2, 0.8, 0.4) / 2.0; // Grass
    else if (height < 0.3)
        color = vec3(0.6, 0.6, 0.6) / 2.0; // Rock
    else
        color = vec3(0.9, 0.9, 1.0) / 2.0; // Snow

    float nl = dot(norm, light_dir);
    if (nl > 0.0) {
        color += nl * vec3(0.5);

        // Add reflection on water and snow
        if (height <= 0.0 || height >= 0.3) {
            float rv = dot(reflect(-light_dir, norm), view_dir);
            color += pow(max(0.0, rv), 60.0) * vec3(0.8);
        }
    }

    //color *= texture(tex_color, uv).rgb;

}
