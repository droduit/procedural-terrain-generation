#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;

uniform float tex_width;
uniform float tex_height;

uniform int kernel_size;
uniform float kernel[400];
uniform int axis;

void main() {

    float weight_tot = 0.0;
    vec3 color_tot = vec3(0.0);
    vec3 neigh_color = vec3(0.0);
    vec2 offset = vec2(0.0);

    for(int i = 0; i < kernel_size; i++){
       float weight = kernel[i];

       if(axis == 0) { // pass for the x axis
            offset = vec2((i - (kernel_size / 2.0)) / tex_width, 0.0);
       } else {
            offset = vec2(0.0, (i - (kernel_size / 2.0)) / tex_height);
       }

       neigh_color = texture(tex, uv + offset).rgb;
       color_tot += weight * neigh_color;
       weight_tot += weight;
    }

    color = color_tot / weight_tot;

}

