# Homework 4: Framebuffers

## Fast Gaussian Blur

First, I started with the binding of the keyboard, because it was the simplest part and it allowed to check quickly if the value was well propagated in the screenquad class (with a simple cout << std << endl). I fix the min value for the standard deviation to 0.25 to avoid a division by 0 who would results in a fully black application.

Then, I worked on the computation of the kernel, so I created a new method compute_kernel in the screenquad class, who fill the kernel of a given size, passed in argument as an array of float. This kernel and its size are passed to the fragment shader. In the shader, I fixed an arbitrary size of 400 for the kernel array, it's a largely sufficient value to prevent from having some weird visual  effects. 

Once the kernel was retrieved in the fragment shader, I already tried to display the first pass of the gaussian blur in x on the texture received. Then, I started to allow the second texture in the framebuffer on the basis of the framebuffer of the third exercise. The first texture will then be blured by the first pass of the algorithm and be assigned to the second texture who will then experience the second pass of the gaussian blur. These 2 pass are done one after the other with two succesive calls in the Display() method of main.cpp.

I also had to solve a problem who displayed the wrong texture when we were resizing the window. The framebuffer was cleaned and the screenquad was badly reinitialized with missing values. The problem was solved in passing the missing second texture as parameter.  

## Screen Space Reflections

During `Display()`, we first render the reflection by reflecting the position of the camera relatively to the shiny floor and keeping the point looked at and inverting the camera upward direction. We only render the cube.

We then proceed to render the cube and the floor normally.

In the floor's fragment shader, we compute the texture coordinates and invert the X axis to have the correct mirror effect.
If we didn't change the camera upward direction, we would invert the Y axis to have the correct mirror effect.

## Motion Blur

The motion blur is quite simple once the concept is understood: we compute a `motionVector` vector in the quad fragment shader. This vector will ultimately end up in the texture `velocityTex` that we will "query" to get our vector back in the screenquad fragment shader. In this shader, we set the color of the fragment at position `uv` to be an average of the colors of N (= 10 for good visual) samples along the vector starting at `uv` to `uv + motionVector`.
