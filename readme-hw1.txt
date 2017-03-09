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

- `Space`: a space background has been added. We enable `GL_BLEND` with `GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA` as the blend function to remove the white background of the sun, earth and moon.

- `Sun`: no particular difficulty, the scale and translation matrices are fixed, only the rotation matrix takes the time as parameter to make the sun rotate on itself.

- `Earth` : To make it rotate on an ellipsoidal trajectory, we use the parametric equation for an ellipse as translation matrix: `(a * cos (theta * t), b * sin (theta * t), 0)`.

- `Moon` : As the trajectory must follow those of the earth, the important thing is to not start from the identity matrix as before, but from the earth's transformation matrix. Afterward we can change its position regarding the earth by the translation matrix, scale it, and finally make it rotate on itself.

## Arkanoid

To simplify positioning and scaling of quads, some features has been added to the `Quad` class. It now has a position (x, y), a size (width, height) and the accessors to retreive and set them.
The `Draw()` method use those information to compute a transformation matrix accordingly. A new method `CollidesWith()` also determines if two quads collide with each other.
It declares a method `Update()` but doesn't provide an implementation, it will be implemented by the Quad's children to handle movement. This method take a parameter `dt` containing the delta time since the last update. It allows the different actors to keep a consistent movement speed independently of the number of updates per second.

The first child to extends `Quad` is the `Paddle` class. It only new attribute is `direction` that keep track of the
movement direction. The keyboard callback in `main.cpp` set the direction according to the keyboard input.
The `Update()` method uses the direction to move the paddle and ensure it doesn't move out of the screen.

The second child is the `Ball` class. It contains a 2D vector (vx, vy) to keep track of its velocity.
The `Update()` method move the ball according to its velocity and bounce the ball on the walls and the floor.
The `Bounce()` method is triggered in `main.cpp`, when the paddle and the ball collide. The `dx` parameter represent where on the paddle the ball bounced, it is used to change the angle of the bounce.

The last class `Walls` doesn't extends `Quad`. It just groups the 3 walls together as 3 quads.
It could be changed to store more walls and create a more complex layout, like a labyrinth.

In `main.cpp`, we instanciate and init our objects as usual.
The keyboard callback has been extended to watch the arrow keys and update the paddle's direction accordingly.
The render loop does now compute the time since the last iteration started and pass that information to the `Update()` method that will ensure the paddle and the ball are updated. The `Update()` method also check if the ball touch the floor and print a message if it does, and it check for collision between the ball and the paddle to make the ball bounce on it.

Finally the vertex shader receive the current time as a uniform and compute the color for each vertice using the time and the position, giving us that colorfull effect!
