#version 330

in vec2 uv;
in vec4 fragPos;

out vec3 color;

void main() {
    //color = texture(tex, uv).rgb;

    vec3 dx = dFdx(fragPos.xyz);
    vec3 dy = dFdy(fragPos.xyz);
    vec3 normal = normalize(cross(dx, dy));
    vec3 light_dir = normalize(vec3(-2.0, -2.0, 4.0));
    vec3 view_dir = normalize(vec3(-2.0, -2.0, 2.0));

    if (fragPos.z <= 0.0)
        color = vec3(0.2, 0.3, 0.9) / 2.0; // Water
    else if (fragPos.z < 0.04)
        color = vec3(0.8, 0.7, 0.2) / 2.0; // Sand
    else if (fragPos.z < 0.28)
        color = vec3(0.2, 0.8, 0.4) / 2.0; // Grass
    else if (fragPos.z < 0.3)
        color = vec3(0.6, 0.6, 0.6) / 2.0; // Rock
    else
        color = vec3(0.9, 0.9, 1.0) / 2.0; // Snow

    float nl = dot(normal, light_dir);
    if (nl > 0.0) {
        color += nl * vec3(0.5);

        float vr = max(dot(reflect(-light_dir, normal), view_dir), 0.0);

        color += pow(vr, 60.0) * vec3(0.8);
    }
}
