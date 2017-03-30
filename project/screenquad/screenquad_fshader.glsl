#version 330

in vec2 uv;

out vec3 color;

uniform sampler2D tex;
uniform float tex_width;
uniform float tex_height;

mat3 Gx = mat3(
     1.0,  2.0,  1.0,
     0.0,  0.0,  0.0,
    -1.0, -2.0, -1.0
);

mat3 Gy = mat3(
    1.0, 0.0, -1.0,
    2.0, 0.0, -2.0,
    1.0, 0.0, -1.0
);

mat3 Gauss = mat3(
    1.0, 2.0, 1.0,
    2.0, 4.0, 2.0,
    1.0, 2.0, 1.0
);

float rgb_2_luma(vec3 c) {
    return dot(vec3(0.3, 0.59, 0.11), c);
}

float convolveM(mat3 kernel, mat3 matrix) {
    int i, j;

    float res = 0.0;
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            res += kernel[i][j] * matrix[i][j];

    return res;
}

vec3 gaussian(vec3[9] matrix) {
    int i, j;

    vec3 res = vec3(0.0);
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            res += Gauss[i][j] * matrix[i * 3 + j];
        }
    }

    return res / 16.0;
}

vec3[9] aroundFragments() {
    vec3[9] res;

    res[0] = textureOffset( tex, uv, ivec2( -1, -1 ) ).rgb;
    res[1] = textureOffset( tex, uv, ivec2(  0, -1 ) ).rgb;
    res[2] = textureOffset( tex, uv, ivec2(  1, -1 ) ).rgb;
    res[3] = textureOffset( tex, uv, ivec2( -1,  0 ) ).rgb;
    res[4] = textureOffset( tex, uv, ivec2(  0,  0 ) ).rgb;
    res[5] = textureOffset( tex, uv, ivec2(  1,  0 ) ).rgb;
    res[6] = textureOffset( tex, uv, ivec2( -1,  1 ) ).rgb;
    res[7] = textureOffset( tex, uv, ivec2(  0,  1 ) ).rgb;
    res[8] = textureOffset( tex, uv, ivec2(  1,  1 ) ).rgb;

    return res;
}

mat3 mapToLuma(vec3[9] fragments) {
    int i, j;
    mat3 res;

    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            res[i][j] = rgb_2_luma( fragments[ i * 3 + j] );

    return res;
}

void main() {
    vec3[9] Iin = aroundFragments();

    // Edge detection
    /*
    mat3 luma = mapToLuma(Iin);

    float horizEdge = convolve(Gx, luma);
    float vertEdge  = convolve(Gy, luma);

	float edge = 1 - sqrt((horizEdge * horizEdge) + (vertEdge * vertEdge));
	color = vec3( edge );
    */

    // Gaussian
    color = gaussian(Iin);
}

