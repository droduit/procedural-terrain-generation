#version 330


out vec3 color;

in vec3 normal_mv;
in vec3 light_dir;
in vec3 view_dir;


uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

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

    color = vec3(0.0,0.0,0.0);

    color += La*ka;
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);
    if(lambert > 0.0) {
        color += Ld*kd*lambert;
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l,n);
        color += Ls*ks*pow(max(dot(r,v), 0.0), alpha);
    }
}
