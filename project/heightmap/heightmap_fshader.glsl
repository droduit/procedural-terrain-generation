#version 330

in vec2 uv;

out float color;

uniform float dx, dy;
uniform float hcomp, vcomp, voffset;
uniform float H, lacunarity, warp;
uniform int type, seed, octaves;

float rand(vec2 co){
  return fract(sin(seed + dot(co, vec2(12.9898,78.233))) * 43758.5453);
}

vec2 g(vec2 p) {
    float angle = 2.0 * 3.14159 * rand(p);
    float factor = 2.0 * rand(-p);

    return factor * vec2(cos(angle), sin(angle));
}

float pnoise(vec2 point) {
    vec2 cell = floor(point);
    vec2 poff = fract(point);

    vec2 a = point - (cell + vec2(0.0, 0.0));
    vec2 b = point - (cell + vec2(1.0, 0.0));
    vec2 c = point - (cell + vec2(0.0, 1.0));
    vec2 d = point - (cell + vec2(1.0, 1.0));

    float s = dot(g(cell + vec2(0.0, 0.0)), a); // a = g1.xy
    float t = dot(g(cell + vec2(1.0, 0.0)), b); // b = g2.xy
    float u = dot(g(cell + vec2(0.0, 1.0)), c); // c= g1.zw
    float v = dot(g(cell + vec2(1.0, 1.0)), d); // d = g2.zw

    vec2 f = poff * poff * poff * (10 + poff * (-15.0 + poff * 6.0));

    float st = mix(s, t, f.x);
    float uv = mix(u, v, f.x);
    float noise = mix(st, uv, f.y);

    return noise;
}


vec3 perlinNoiseDeriv(vec2 point) {

    vec2 cell = floor(point);
    vec2 poff = fract(point);

    vec2 a = point - (cell + vec2(0.0, 0.0));
    vec2 b = point - (cell + vec2(1.0, 0.0));
    vec2 c = point - (cell + vec2(0.0, 1.0));
    vec2 d = point - (cell + vec2(1.0, 1.0));

    vec4 g1 = vec4(a.x, a.y, c.x, c.y);
    vec4 g2 = vec4(b.x, b.y, d.x, d.y);

    float s = dot(g(cell + vec2(0.0, 0.0)), a); // a = g1.xy
    float t = dot(g(cell + vec2(1.0, 0.0)), b); // b = g2.xy
    float u = dot(g(cell + vec2(0.0, 1.0)), c); // c= g1.zw
    float v = dot(g(cell + vec2(1.0, 1.0)), d); // d = g2.zw

    vec2 f = poff * poff * poff * (10 + poff * (-15.0 + poff * 6.0));
    vec4 f4 = vec4(1.0, f.x, f.y, f.x * f.y);

    vec4 grads = vec4(s, t - s, u - s, s - t - u + v);

    float noise = dot(f4, grads);

    // Get the derivative dw/df
    vec2 df = poff * poff * (poff * (poff * 30 - 60) + 30); // 30f^4 - 60f^3 + 30f^2

    // Get the derivative d(w*f)/df
    vec2 dfp = poff * poff * poff * (poff * (poff * 36 - 75) + 40); // 36f^5 - 75f^4 + 40f^3

    float dx = (g1.x + (g1.z-g1.x)*f.y) + ((g2.y-g1.y)*f.y - g2.x +
               ((g1.y-g2.y-g1.w+g2.w)*f.y + g2.x + g1.w - g2.z - g2.w)*f.y)*
               df.x + ((g2.x-g1.x) + (g1.x-g2.x-g1.z+g2.z)*f.y)*dfp.x;

    float dy =
        g1.y +
        f.x * (g2.y - g1.y) +
        df.y * (f.x * (f.x * (g1.x - g2.x - g1.z + g2.z) + g2.x + g1.w - g2.z - g2.w + g1.z - g1.x) - g1.w) +
        dfp.y * (g1.w - g1.y + f.x * (g1.y - g2.y - g1.w + g2.w));

    // Return the noise value, roughly normalized in the range [-1, 1]
    // Also return the pseudo dn/dx and dn/dy, scaled by the same factor
    return vec3(noise, dx, dy) * 1.5;
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

float swissTurbulence(vec2 p, int octaves, float lacunarity, float gain, float warp) {
     float sum = 0;
     float freq = 1.0, amp = 1.0;
     vec2 dsum = vec2(0,0);

     for(int i = 0; i < octaves; i++) {
         vec3 n = perlinNoiseDeriv((p + warp * dsum)*freq);

         sum += amp * (1 - abs(n.x));
         dsum += amp * n.yz * -n.x;
         freq *= lacunarity;
         amp *= gain * clamp(sum, 0, 1);
    }

    return sum;
}

void main() {
    // Stretch the montains with 0.75, flatten them with 1.75
    if (type == 0)
        color = fBm(hcomp * uv + vec2(dx, dy), H, lacunarity, octaves) * vcomp + voffset;
    else if (type == 1)
        color = ridged_fBm(hcomp * (uv + vec2(dx, dy)), H, lacunarity, octaves) * vcomp + voffset;
    else if (type == 2)
        color = billowy_fBm(hcomp * (uv + vec2(dx, dy)), H, lacunarity, octaves) * vcomp + voffset;
    else
        color = swissTurbulence(hcomp * uv + vec2(dx, dy), octaves, lacunarity, H, warp) * vcomp + voffset;
}

