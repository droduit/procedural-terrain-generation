#version 330

in vec2 uv;
in vec4 fcolor;

out vec4 color;

uniform sampler2D tex;

void main() {
    /*
    color = texture(tex,uv).rgba;
    if (dot(color.rgb, vec3(1.0, 1.0, 1.0)) > 2.9)
        color.a = 0.0;
     */

    color = fcolor;
}


