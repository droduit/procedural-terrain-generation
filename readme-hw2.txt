# Homework 2: Perspective, Trackball and Animations

## Perspective Projection

From the course, we have the following informations leading to the perspective matrix projection :

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

For the `ProjectOnSurface()` method, we change the method used to compute `z` depending on the position of the point (within the trackball or outside of it). We use the given equation to compute `z`.

For the `Drag()` method, we used the `orientedAngle()` function to compute the angle between `anchor_pos` and `current_pos`. It now become easy to compute the rotation matrix, but we don't compute the rotation matrix if the computed angle is 0 (`glm::rotate` gets confused with a null angle)!

We finally have to multiply the generated matrix with `old_trackball_matrix` to get the final transformation, the transformation being "committed" into `old_trackball_matrix` only when a new drag begins.

For the zoom, we keep track of the last `y` the mouse was and use the difference with the new `y` in the translation of the `view_matrix`. The `ZOOM_FACTOR` can be modified to change the zooming speed.

## Triangle Grid and Animation

Creating the vertices for the grid was really easy: we just don't forget that there are n+1 vertices on an n-by-n grid, use a 2.0f/n spacing between vertices and that's it. The vertices are created in the canonic CS way: line by line from left to right, which is not the way OpenGL works. Thankfully, the "CS-to-OpenGL" coordinate conversion happens in the grid vertex shader where we only negate the y-coordinate. Now we know the coordinates are correctly handled, we can work in CS coordinates.

To create the mesh using `GL_TRIANGLE_STRIP`, we have to be a bit a smart to utilize the power offered by this instruction. There are two main parts: the line creation, with the two initial vertices (aka "the base case") and next vertices on the line (aka "the induction step"), and the jump to the next line, to proceed cleanly with the next line. We create our lines by iterating over "squares" of vertices with the top-left corner at (x,y) – with x and y the integer coordinates used for the iteration – and bottom-right corner at (x+1,y+1). With this in mind, let's discuss how an entire line is created:

  * The base case is quite simple: we add the two first vertices (x,y) and (x,y+1) when x == 0.
  * For the induction step, we just add the vertices (x+1,y) and (x+1,y+1), the `GL_TRIANGLE_STRIP` will create for us the triangles we need (one for each new vertex pushed).

`GL_TRIANGLE_STRIP` switches between counterclockwise and clockwise declaration of vertices for each new vertex pushed: ABC declares a counterclockwise triangle but in ABCD the triangle BCD will be interpreted as BDC, that is: a triangle with the same orientation as ABC – a useful property when using textures for instance. The problem is that adding vertices becomes a stateful operation and we need to keep track of what orientation is currently used – a problem that arises when we reach the end of the line and need to get to the next line. In addition, when we get to the next line, a triangle between the two last vertices of the last line and the new vertex of the new line is created and shouldn't exist.

The solution was twofold:

  * Re-insert the last vertex of the previous line to avoid the unwanted triangle (we effectively create two zero-area triangles which are discarded later in the pipeline and thus invisible), with the effect of skipping the counterclockwise state.
  * Use a vertically mirrored version of pattern discussed for creating lines but iterate with x from grid_dim-1 to 0 because we begin where the previous line ended.

When we arrive at the beginning of the line, we just apply the same pattern as for the first line. Note: the code isn't an exact transcription of the described algorithm but uses the same general ideas.

If we look at the wireframe rendering, the result is an alternated chain-saw pattern at only the cost of a single vertex per line, needed to get to the next line without an unwanted triangle.

(The formula that should be put in `grid/grid_vshader.glsl` for the animation subtask is `height = 0.1*sin(12*(uv.x + uv.y) + time);`. We put the formuli for the Water animation task but did not include this one.)

## Water Animation

### Sin Waves

In order to obtain a water-like animation, the sine/cosine functions had to be hidden a bit because their pattern would have sold us right away. The function `0.2*sin(uv.x*3+time)*cos(uv.y*3+time*0.5)` was chosen: not too much amplitude with the `0.2` factor, a sine and cosine that are not phased and are dependent of the x-y coordinate of the vertex. The main advantage of this function is the simplicity of implementation and the relatively low computational cost associated with it.

### Gerstner Waves

We also implement a more complex, but more realistic, way to simulate water using the Gerstner waves function (Effective Water Simulation from Physical Models)[http://http.developer.nvidia.com/GPUGems/gpugems_ch01.html]. This method allows us to form sharper crests by moving vertices toward each crest, producing a more convincing simulation for more agitated plane of water.

You can enable this method by uncommenting the `ENABLE_GERSTNER_WAVES` define in `grid_vshader.glsl`.
You may also extend the size of the grid for a better result.

This method works by adding the effects of multiple waves together. We won't paraphrase the GPU Gem, but we will describe the properties that define a wave:

  * direction: a normalized vector containing the wave movement's direction
  * frequency: the length of the wave
  * amplitude: the height of the wave
  * speed: the movement speed of the wave
  * steepness: a value in the range [0.0, 1.0] describing the sharpness of the crest (1.0 being very sharp)

The current settings has the first two waves less noticeable to produce a background movement. The third wave is the sharp one that cross the screen regularly.
The three waves have the same orientation to produce a more readable result. Changing the orientation of a wave easily produce a noisy pattern and the water simulation becomes less convincing.
