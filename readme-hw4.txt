# Homework 4: Framebuffers

## Fast Gaussian Blur

## Screen Space Reflections

During `Display()`, we first render the reflection by reflecting the position of the camera relatively to the shiny floor and keeping the point looked at and inverting the camera upward direction. We only render the cube.

We then proceed to render the cube and the floor normally.

In the floor's fragment shader, we compute the texture coordinates and invert the X axis to have the correct mirror effect.
If we didn't change the camera upward direction, we would invert the Y axis to have the correct mirror effect.

## Motion Blur

