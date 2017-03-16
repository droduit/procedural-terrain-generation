#version 330

in vec3 vpoint;
in vec3 vnormal;

uniform vec3 La, Ld, Ls;
uniform vec3 ka, kd, ks;
uniform float alpha;
uniform mat4 projection;
uniform mat4 model;
uniform mat4 view;
uniform vec3 light_pos;

out vec3 vcolor;
out vec3 light_dir;
out vec3 view_dir;


void main() {
    mat4 MV = view * model;
    vec4 vpoint_mv = MV * vec4(vpoint, 1.0);
    gl_Position = projection * vpoint_mv;
    vcolor = vec3(0.0,0.0,0.0);
    vec3 normal_mv = normalize(mat3(transpose(inverse(MV))) * vnormal);
    light_dir = light_pos - vpoint_mv.xyz;
    view_dir = -vpoint_mv.xyz;

    vcolor += La*ka;
    vec3 n = normalize(normal_mv);
    vec3 l = normalize(light_dir);
    float lambert = dot(n,l);
    if(lambert > 0.0) {
        vcolor += Ld*kd*lambert;
        vec3 v = normalize(view_dir);
        vec3 r = reflect(-l,n);
        vcolor += Ls*ks*pow(max(dot(r,v), 0.0), alpha);
    }
}
