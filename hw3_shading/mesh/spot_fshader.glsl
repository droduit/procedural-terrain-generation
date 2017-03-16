#version 330

uniform vec3 spot_dir;
uniform vec3 La, Ld, Ls; // colors of the lights
uniform vec3 ka, kd, ks; // materials of the mesh
uniform float alpha;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

out vec3 color;

const float spot_cos_cutoff = 0.985; // cos 10 degrees
const float spot_exp = 150;

void main() {    
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 5: Spot light.
    // Complete the shader to obtain spot light effect
    //<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 normal_mv = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);
    vec3 spot_dir = normalize(spot_dir);

    color = vec3(0.0);

    // Compute diffuse/specular only if we're in the spotlight
    float lambert = dot(normal_mv, light_dir);
    float spot_effect = dot(light_dir, spot_dir);

    if (lambert > 0.0 && spot_effect > spot_cos_cutoff) {
        color += Ld * kd * lambert;

        float rv = dot(reflect(-light_dir, normal_mv), view_dir);
        color += Ls * ks * pow(max(rv, 0.0), alpha);

        // Apply attenuation
        color *= pow(spot_effect, spot_exp);
    }

    // Add ambiant
    color += ka * La;
}
