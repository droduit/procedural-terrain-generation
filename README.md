# Homework 1 : 2D transformations and GLSL

## Triangle Spirals

For the simple spiral, a trick was applied in order to achieve the wanted result: the translation matrix was applied before the rotation matrix. This way, we rotate the triangle around the central point directly to its final position, instead of rotating the triangle in place and then translating it. The variable `r` goes from 0 to 1 depending on the on which triangle we are currently drawing; this variable is used to determine the scale, translation and rotation of the triangle.

A similar trick was applied to the Fermat's spiral, with `r` (and `theta`) given by the Fermat's formula for the spiral.

## Checkerboard

1. The first step consisted to start very simply in drawing the texture only according to the uv.x position, to see how the colors were distributed on the square along the x axis.

2. What can we use to access the full range of color between the position `[-1,1]` ? : the sine function.
We always have to remember that the sine curve will pick the color at the position corresponding to its value,  on the texture drawn in the first step.

3. We have to think about "how to get 10 alternate red&green ribbons along the x axis" ? This is achieved in changing the frequency of the sine. With one period of the sine (2*pi), we get 1 green and 1 red ribbon, so we have to accelerate the sine curve to have 5 time the alternate green/red ribbons : `sin(uv.x * (5 * 2pi))`.

4. Now the red ribbons are too thick and not very well positioned. We have to shift them to the right. In other words, we want to shift the sine curve to the top to only have positive values : `sin(uv.x * (5 * 2pi)) + 1`.

5. But we can't be satisfied with that, because we are left with values between `[0,2]`. To get the ribbons correctly positioned, we simply divide the overall by 2, to limit the values in the range `[0,1]` : `(sin(uv.x * (5 * 2pi)) + 1) / 2`.

6. We are close to the expected result for the x-axis. We now work on the y-axis with exactly the same reasoning as before to make 10 alternates ribbons but using the uv.y position : `sin(uv.x * (5 * 2pi)) * sin(uv.y * (5 * 2pi) + 1) / 2`. We are done !

## Solar System

It's important that the rotation matrix is always applied after the translation matrix.

- `Sun`: no particular difficulty, the scale and translation matrices are fixed, only the rotation matrix takes the time as parameter to make the sun rotate on itself.

- `Earth` : To make it rotate on an ellipsoidal trajectory, we use the parametric equation for an ellipse as translation matrix: `(a * cos (theta * t), b * sin (theta * t), 0)`.

- `Moon` : As the trajectory must follow those of the earth, the important thing is to not start from the identity matrix as before, but from the earth's transformation matrix. Afterward we can change its position regarding the earth by the translation matrix, scale it, and finally make it rotate on itself.
