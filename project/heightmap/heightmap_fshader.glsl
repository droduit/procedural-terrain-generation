#version 330

in vec2 uv;

out float color;

uniform float dx, dy;
uniform float hcomp, vcomp, voffset;
uniform float H, lacunarity, offset;
uniform int type, seed, octaves;

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

float pnoise(vec2 point) {
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

    vec2 f = 6.0 * pow(poff, vec2(5)) - 15.0 * pow(poff, vec2(4)) + 10.0 * pow(poff, vec2(3));

    float st = mix(s, t, f.x);
    float uv = mix(u, v, f.x);
    float noise = mix(st, uv, f.y);

    return noise;
}

float fBm(vec2 point, float H, float lacunarity, int octaves) {
    int i = 0;
    float value = 0.0;

    for (i = 0; i < octaves; i++) {
        value += pnoise(point) * pow(lacunarity, -H * i);
        point *= lacunarity;
    }

    return value;
}

float ridged_fBm(vec2 point, float H, float lacunarity, int octaves) {
    int i = 0;
    float value = 0.0;

    for (i = 0; i < octaves; i++) {
        value += (1.0 - abs(pnoise(point))) * pow(lacunarity, -H * i);
        point *= lacunarity;
    }

    return value;
}

float billowy_fBm(vec2 point, float H, float lacunarity, int octaves) {
    int i = 0;
    float value = 0.0;

    for (i = 0; i < octaves; i++) {
        value += abs(pnoise(point)) * pow(lacunarity, -H * i);
        point *= lacunarity;
    }

    return value;
}

float multifractal(vec2 point, float H, float lacunarity, int octaves, float offset) {
    int i = 0;
    float value = 1.0;

    for (i = 0; i < octaves; i++) {
        value *= pnoise(point) * pow(lacunarity, -H * i);
        point *= lacunarity;
    }

    return value;
}

void main() {
    // Stretch the montains with 0.75, flatten them with 1.75
    if (type == 0)
        color = fBm(hcomp * (uv + vec2(dx, dy)), H, lacunarity, octaves) * vcomp + voffset;
    else if (type == 1)
        color = ridged_fBm(hcomp * (uv + vec2(dx, dy)), H, lacunarity, octaves) * vcomp + voffset;
    else if (type == 2)
        color = billowy_fBm(hcomp * (uv + vec2(dx, dy)), H, lacunarity, octaves) * vcomp + voffset;
    else if (type == 3)
        color = multifractal(hcomp * (uv + vec2(dx, dy)), H, lacunarity, octaves, offset) * vcomp + voffset;
    else
        color = vcomp * sin(hcomp * (uv.x + dx)) + cos(hcomp * (uv.y + dy)) + voffset;
}

