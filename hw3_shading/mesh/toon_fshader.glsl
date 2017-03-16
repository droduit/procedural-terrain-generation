#version 330


out vec3 color;

uniform sampler1D tex1D;

uniform vec3 La, Ld, Ls; // colors of the lights
uniform vec3 ka, kd, ks; // materials of the mesh
uniform float alpha;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

void main() {

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO 2.2: Toon shading.
    /// 1) compute ambient term.
    /// 2) compute diffuse term using the texture sampler tex.
    /// 3) compute specular term using the texture sampler tex.
    ///<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 normal_mv = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);

    float lambert = clamp(dot(normal_mv, light_dir), 0.0, 1.0);
    float rv = clamp(dot(reflect(-light_dir, normal_mv), view_dir), 0.0, 1.0);

    vec3 ambient = La * ka;

    vec3 diffuse = vec3(0.0), specular = vec3(0.0);
    if (lambert > 0.0) {
        diffuse = Ld * kd * texture(tex1D, lambert).r;
        specular = Ls * ks * texture(tex1D, pow(clamp(rv, 0.0, 1.0), alpha)).r;
    }

    color = ambient + diffuse + specular;
}
