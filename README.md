# Procedural Terrain Generator @ EPFL

<p><img src="https://ttreyer.github.io/procedural-terrain-generation/images/image3.png" /></p>
<p><img src="https://ttreyer.github.io/procedural-terrain-generation/images/image1.png" /></p>
<p><img src="https://ttreyer.github.io/procedural-terrain-generation/images/image2.png" /></p>

## Introduction
Procedural terrain generator written from scratch using various procedural technics to reproduce the Swiss topology.
The topology is a 3D mesh made of triangles. Among the various methods applied, we find in particular:
- Noise algorithms to create the height map (Perlin noise, Fractal Brownian motion, Swiss turbulence)
- Camera modes (Free navigation in the space, FPS like mode, Camera following Bezier curves)
- Water in motion 
- Reflexion of the landscape in the water
- Fogs
- SkyBox
- Day cycles
- Snow 
- Shadows of the mountains
- Texture mapping

The infinite map is fully explorable in 3D and every parameter are adjustable.

## How to use
- Clone the repository in your local workspace
- Open a terminal at the root folder (the folder containing the whole repo)
- `mkdir build`
- `cd build`
- `cmake ..`
- `make`
- `cd project`
- `./project`

## Authors
- Dominique Roduit ([@droduit](https://github.com/droduit))
- Thierry Treyer ([@ttreyer](https://github.com/ttreyer))
- Romain Gehrig ([@RomainGehrig](https://github.com/RomainGehrig))

## Note
This is the result of a team project for the [Introduction to Computer Graphics](http://edu.epfl.ch/coursebook/fr/introduction-to-computer-graphics-CS-341?cb_cycle=min_&cb_section=min_sc) course taught at [EPFL](https://epfl.ch).

The project took 7 weeks and was divided into 3 parts:
- Part I: Procedural generation of terrain (2 weeks)
- Part II: Advanced rendering (2 weeks)
- Part III: Animation / Interaction (3 weeks)
