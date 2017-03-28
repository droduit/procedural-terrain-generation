#version 330 core
in vec2 uv;
in vec4 currentPosition;
in vec4 previousPosition;

uniform sampler2D tex;

layout (location = 0) out vec4 color;
layout (location = 1) out vec2 motionVector;

void main() {
    color = texture(tex, uv);

    // The w coordinate for pixels is '1'; so we correctly get a vector (w = 0)
    // by substracting two homogeneous coordinates
    vec4 diff = currentPosition - previousPosition;

    motionVector = diff.xy; // only need x and y coordinates
}
