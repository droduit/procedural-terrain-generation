#version 330 core
out vec4 color;
in vec2 uv;
in float height;

uniform sampler2D heightmap;
uniform sampler2D reflection;

void main() {
    vec2 uv_mirror = gl_FragCoord.xy / textureSize(reflection, 0);

    uv_mirror.y = 1.0 - uv_mirror.y;

    vec4 reflection_color = texture(reflection, uv_mirror);
    vec4 water_color = color = vec4(0.2, 0.3, 0.6, 0.8 - smoothstep(-2.4, 0.0, height) / 2.0);

    color = mix(water_color, reflection_color, 0.2);
}
