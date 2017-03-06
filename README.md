# computer-graphics

## Triangle Spirals

For the simple spiral, a trick was applied in order to achieve the wanted result: the translation matrix was applied before the rotation matrix. This way, we rotate the triangle around the central point directly to its final position, instead of rotating the triangle in place and then translating it. The variable `r` goes from 0 to 1 depending on the on which triangle we are currently drawing; this variable is used to determine the scale, translation and rotation of the triangle.

A similar trick was applied to the Fermat's spiral, with `r` (and `theta`) given by the Fermat's formula for the spiral.
