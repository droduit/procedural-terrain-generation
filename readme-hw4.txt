# Homework 4: Framebuffers

## Fast Gaussian Blur

First, I started with the biding of the keyboard, because it was the simplest part and it allow to verify rapidely if the value is well passed to the shader.

## Screen Space Reflections

During `Display()`, we first render the reflection by reflecting the position of the camera relatively to the shiny floor and keeping the point looked at and inverting the camera upward direction. We only render the cube.

We then proceed to render the cube and the floor normally.

In the floor's fragment shader, we compute the texture coordinates and invert the X axis to have the correct mirror effect.
If we didn't change the camera upward direction, we would invert the Y axis to have the correct mirror effect.

## Motion Blur

The motion blur is quite simple once the concept is understood: we compute a `motionVector` vector in the quad fragment shader. This vector will ultimately end up in the texture `velocityTex` that we will "query" to get our vector back in the screenquad fragment shader. In this shader, we set the color of the fragment at position `uv` to be an average of the colors of N (= 10 for good visual) samples along the vector starting at `uv` to `uv + motionVector`.
