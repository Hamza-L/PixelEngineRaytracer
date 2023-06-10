# PixelEngine Raytracer

This Raytraced rendering engine makes use of the PixelEngine renderer which is a Vulkan API based graphics engine designed by yours truly.

This specific raytracer makes use of compute shaders to render a scene. It feature a series of features, mainly:

* Depth of field manual focus control
* Autofocus
* Sample count control
* Object selection visualization (outlining)
* Light position control (on the xz-plane)

Here's a showcase of what that looks like :)

![Alt](/documentation/ezgif-4-c802bd0aa7.gif "Title")

The depth of field blur uses camera position jitter based on a normal distribution. The "lookat" point is the point in focus.
The selection outline is done using a second image that writes a white pixel if :
>dot ( finalHit.normal , ray.direction ) >= -0.2f

The lighting is simple Blinn-Phong lighting. The number of samples dictates how many times the compute shader is run.

Some features I am currently working on are mainly:
* Dynamic compute shader writing
* Extracting data from compute shader using ssbo
* From the data extracted, ability to focus anywhere on the scene.
* Adding more complex object. Mesh-soup ray bounce.
* Eventually Bounding volume hierarchy (BVH) for mesh rendering.

