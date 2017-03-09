#version 330

in vec2 uv;

out vec4 color;

uniform sampler2D tex;

void main() {
    color = texture(tex,uv).rgba;

    if (dot(color.rgb, vec3(1.0, 1.0, 1.0)) > 2.9)
        color.a = 0.0;
}


