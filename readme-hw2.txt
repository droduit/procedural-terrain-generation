# Homework 2: Perspective, Trackball and Animations

## Perspective Projection

## Trackball and Zoom

For the `ProjectOnSurface()` method, we change the method used to copmute `z` depending on the position of the point (within the trackball or outside of it). We use the given equation to compute `z`.

For the `Drag()` method, we used the `orientedAngle()` function to compute the angle between `anchor_pos` and `current_pos`. It now become easy to compute the rotation matrix.

We finally have to multiply the generated matrix with `old_trackball_matrix` to get the final transformation, the transformation being "committed" into `old_trackball_matrix` only when a new drag begins.

For the zoom, we keep track of the last `y` the mouse was and use the difference with the new `y` in the translation of the `view_matrix`. The `ZOOM_FACTOR` can be modified to change the zooming speed.

## Triangle Grid and Animation

## Water Animation

