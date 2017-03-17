# Homework 3: Shading

## 1. Phong Shading

## 2. Toon Shading

## 3. Artistic Shading

We reuse the Phong vertex shader and fragment shader as a base.
We compute the `nl` and `rv` values using the same formula used by Phong, but instead of multiplying them with the material properties, we use them as coordinates for the artistic texture.

We have to clamp the `nl` and `rv` values in the range [0.0, 1.0], else some visual artifacts appear on the edges of the mesh.

## 4. Flat Shading

## 5. Spot Light Shading

We reuse the Phong vertex shader and fragment shader as a base.
We have a new factor `spot_effect` that contains the dot product of `light_dir` and `spot_dir`. It quantifies the amount of light the spot is giving to this fragment, since the spot is a directed light thus doesn't lit the scene uniformly.
If the `spot_factor` is below the `spot_cos_cutoff`, we consider the fragment to be outside of the spot's cone of light and we don't compute the diffuse and specular effects.
If we are within the cone of light, we still need to attenuate the intensity of the light effects regarding the distance of the fragment from the `spot_dir` vector. The amount of attenuation can be adjusted through the `spot_exp` constant.
