# Homework 3: Shading

## 1. Phong Shading

## 2. Toon Shading

## 3. Artistic Shading

We reuse the Phong vertex shader and fragment shader as a base.
We compute the `nl` and `rv` values using the same formula used by Phong, but instead of multiplying them with the material properties, we use them as coordinates for the artistic texture.

We have to clamp the `nl` and `rv` values in the range [0.0, 1.0], else some visual artifacts appear on the edges of the mesh.

## 4. Flat Shading

## 5. Spot Light Shading
