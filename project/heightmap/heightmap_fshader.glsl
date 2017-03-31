#version 330

in vec2 uv;

out float color;

uniform float seed;
uniform float dx, dy;

float rand(vec2 co){
  return fract(sin(seed + dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

vec2 g(vec2 p) {
    vec2 G[4];

    G[0] = vec2( 1,  1);
    G[1] = vec2(-1,  1);
    G[2] = vec2( 1, -1);
    G[3] = vec2(-1, -1);

    return G[int(4 * rand(p))];
}

float fnoise(vec2 point) {
    vec2 cell = floor(point);
    vec2 poff = fract(point);

    vec2 a = point - (cell + vec2(0.0, 0.0));
    vec2 b = point - (cell + vec2(1.0, 0.0));
    vec2 c = point - (cell + vec2(0.0, 1.0));
    vec2 d = point - (cell + vec2(1.0, 1.0));

    float s = dot(g(cell + vec2(0.0, 0.0)), a);
    float t = dot(g(cell + vec2(1.0, 0.0)), b);
    float u = dot(g(cell + vec2(0.0, 1.0)), c);
    float v = dot(g(cell + vec2(1.0, 1.0)), d);

    float fx = 6.0 * pow(poff.x, 5) - 15.0 * pow(poff.x, 4) + 10.0 * pow(poff.x, 3);
    float fy = 6.0 * pow(poff.y, 5) - 15.0 * pow(poff.y, 4) + 10.0 * pow(poff.y, 3);

    float st = mix(s, t, fx);
    float uv = mix(u, v, fx);
    float noise = mix(st, uv, fy);

    return noise;
}

float fBm(vec2 point, float H, float lacunarity, int octaves) {
    int i = 0;
    float value = 0.0;

    for (i = 0; i < octaves; i++) {
        value += fnoise(point.xy) * pow(lacunarity, -H * i);
        point *= lacunarity;
    }

    return value;
}

void main() {
    // Stretch the montains with 0.75, flatten them with 1.75
    color = fBm(0.75 * uv + vec2(dx, dy), 1.0, 2.0, 12) / 1.75;
}

