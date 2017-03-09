#version 330
#define M_PI 3.1415926535897932384626433832795

in vec2 uv;

out vec3 color;

uniform sampler1D colormap;


void main() {

    float freq = 5 * (2*M_PI);
    float value =  (sin(uv.x * freq) * sin(uv.y * freq) + 1) / 2;
    color = texture(colormap, value).rgb;

    /*
    an (experimental) alternative (approximately)
    color = vec3(
          1 - sin(10.0 * M_PI * uv.y) * sin(10.0 * M_PI * uv.x),
          1 - sin(10.0 * M_PI * uv.y + M_PI) * sin(10.0 * M_PI * uv.x),
          0.0
      );
    */
}


