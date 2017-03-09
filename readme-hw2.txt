# Homework 2: Perspective, Trackball and Animations

## Perspective Projection

From the course, we have the following informations leading to the perspective matrix projection :

```
float top = near * tan(fovy);
float bottom = -top;
float right = top * aspect;
float left = -right;

1. projection[0][0] = 2.0f*near / (right - left);
2. projection[1][1] = 2.0f*near / (top - bottom);
3. projection[2][0] = (right + left) / (right - left);
4. projection[2][1] = (top + bottom) / (top - bottom);
5. projection[2][2] = -(far + near) / (far - near);
6. projection[2][3] = -1.0f;
7. projection[3][2] = -2.0f * far*near / (far - near);
```

But we can completely avoid to use intermediate variables top, bottom, right, left and remove some expressions in rewriting these equations : 


1. right - left = 2 * near * tan(fovy) * aspect
=> 2 * near / (2 * near * tan(fovy) * aspect) = 1 / aspect * tan(fovy)

2. top - bottom = 2 * near * tan(fovy)
=> 2 * near / 2 * near * tan(fovy) = 1 / tan(fovy)

3. right + left = 0
=> 0

4. top + bottom = 0
=> 0

5, 6, 7. no change

## Trackball and Zoom

For the `ProjectOnSurface()` method, we change the method used to copmute `z` depending on the position of the point (within the trackball or outside of it). We use the given equation to compute `z`.

For the `Drag()` method, we used the `orientedAngle()` function to compute the angle between `anchor_pos` and `current_pos`. It now become easy to compute the rotation matrix.

We finally have to multiply the generated matrix with `old_trackball_matrix` to get the final transformation, the transformation being "committed" into `old_trackball_matrix` only when a new drag begins.

For the zoom, we keep track of the last `y` the mouse was and use the difference with the new `y` in the translation of the `view_matrix`. The `ZOOM_FACTOR` can be modified to change the zooming speed.

## Triangle Grid and Animation

## Water Animation

