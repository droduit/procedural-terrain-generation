#version 330

uniform vec3 La, Ld, Ls; // colors of the lights
uniform vec3 ka, kd, ks; // materials of the mesh
uniform float alpha;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

out vec3 color;

void main() {
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 1.2: Phong shading.
    // 1) compute ambient term.
    // 2) compute diffuse term.
    // 3) compute specular term.
    // To avoid GPU bug, remove
    // the code above after
    // implementing Phong shading.
    //<<<<<<<<<< TODO <<<<<<<<<<<

    // Normalize AFTER vshader! Interpolation will denormalize your vectors!
    vec3 normal_mv = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);

    color = La * ka;

    float lambert = dot(normal_mv, light_dir);

    if (lambert > 0.0) {
        color += Ld * kd * lambert;

        float rv = dot(reflect(-light_dir, normal_mv), view_dir);
        color += Ls * ks * pow(max(rv, 0.0), alpha);
    }
}
