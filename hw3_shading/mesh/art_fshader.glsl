#version 330

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;

out vec3 color;

uniform float alpha;
uniform sampler2D tex2D;

void main() {
    //>>>>>>>>>> TODO >>>>>>>>>>>
    // TODO 3.2: Artistic shading.
    // 1) compute the output color by doing a look-up in the texture using the
    //    texture sampler tex.
    //<<<<<<<<<< TODO <<<<<<<<<<<

    vec3 normal_mv = normalize(normal_mv);
    vec3 light_dir = normalize(light_dir);
    vec3 view_dir = normalize(view_dir);

    float nl = dot(normal_mv, light_dir);
    float rv = dot(reflect(-light_dir, normal_mv), view_dir);

    vec2 texcoord = vec2(nl, pow(rv, alpha));

    color = texture(tex2D, texcoord).rgb;
}
