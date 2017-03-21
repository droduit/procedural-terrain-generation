# Homework 3: Shading

## 1. Phong Shading

We basically kept the same code as the Gouraud shading, but instead of interpolating the color in the vertex shader, we only compute the normal, light direction and view direction in it. Then we pass them to the fragment shader which will compute the 3 terms of the shading for all pixels.

An important point to note is that we normalize the normal, light direction and view direction in the fragment shader and not at the end of the vertex shader, otherwise interpolation would denormalize the vectors.

## 2. Toon Shading

We reused the Phong vertex shader without any modification.
We still use the same fragment shader of the phong shading, with the following modifications :
	
- the ambient term is not modified
- the diffuse term is now multiplied by the texture coordinate of the dot product of the normal and light direction clamped between 0 and 1.
- the specular term is multiplied by the texture coordinate of its last term instead of the last term itself, again clamped between 0 and 1 to avoid some weird effects on the edges. 

## 3. Artistic Shading

We reuse the Phong vertex shader and fragment shader as a base.
We compute the `nl` and `rv` values using the same formula used by Phong, but instead of multiplying them with the material properties, we use them as coordinates for the artistic texture.

We have to clamp the `nl` and `rv` values in the range [0.0, 1.0], otherwise some visual artifacts appear on the edges of the mesh.

## 4. Flat Shading

In the vertex shader, we only compute `light_dir` and `view_dir` and pass it to the fragment shader. There, the only difference from the Phong shading is that we have to compute the `normal_mv` "ourself" and not have it interpolated by OpenGL. For this, we use the functions `dFdx` and `dFdy` on `vpoint_mv.xyz`, which give us two non-colinear vectors along the texture. As we want a normal vector, we just have to do a cross product of these vectors (and normalize of course) to obtain it. The rest of the flat fragment shader is the same as the Phong fragment shader.

## 5. Spot Light Shading

We reuse the Phong vertex shader and fragment shader as a base.
We have a new factor `spot_effect` that contains the dot product of `light_dir` and `spot_dir`. It quantifies the amount of light the spot is giving to this fragment, since the spot is a directed light thus doesn't lit the scene uniformly.
If the `spot_factor` is below the `spot_cos_cutoff`, we consider the fragment to be outside of the spot's cone of light and we don't compute the diffuse and specular effects.
If we are within the cone of light, we still need to attenuate the intensity of the light effects regarding the distance of the fragment from the `spot_dir` vector. The amount of attenuation can be adjusted through the `spot_exp` constant.
