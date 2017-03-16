#version 330


in vec4 vpoint_mv;
in vec3 light_dir, view_dir;

out vec3 color;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;

void main() {
    // The only difference from the Phong shading is that we
    // compute the normal vector differently
    vec3 dx = dFdx(vpoint_mv.xyz);
    vec3 dy = dFdy(vpoint_mv.xyz);
    vec3 normal_mv = normalize(cross(dx,dy));

    color = vec3(0.0f);
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
