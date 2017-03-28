#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform sampler2D tex2;

uniform float tex_width;
uniform float tex_height;

uniform float std;
uniform vec3 G;


void main() {

    vec3 color_tot = vec3(0,0,0);
    float weight_tot = 0;
    int SIZE = 1 + 2 * 3 * int(ceil(std));

    float variance = std*std;

    vec3 tmp = vec3(0.0);
    for(int i=-SIZE; i<=SIZE; i++){
        for(int j=-SIZE; j<=SIZE; j++){
            float x_squared = i*i + j*j;
            float weight = exp(-x_squared/(2.0*variance) );
            vec3 neigh_color = texture(tex, uv+vec2(i/tex_width,j/tex_height)).rgb;
            color_tot += weight * neigh_color;
            weight_tot += weight;
        }
    }


    color = color_tot / weight_tot;

}

