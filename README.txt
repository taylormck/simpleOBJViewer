Name: Taylor McKinney
UT EID: tsm544
CS ID: tsm544

Comments:
//---------------------------RENDERING OPTIONS !!----------------------------//
Added a few displaying options
c - toggles the toon outline (off by default)
p - toggles phong shading
l - uses a light source fixed to the world (default is fixed to camera)
b - shows a bounding box
a - shows the axis
n - shows the normals
f - uses flat shading (default is smooth)
r - resets to original view (runtime only)

Note that the arguments must be separate and must have the '-' flag in front

The program can run with these as arguments or they can be typed at run time
to toggle the option on or off).
i.e., 
viewer [.obj file] -b -a
will run the viewer showing both a bounding box and the axis.

Pressing 'b' at during runtime will hide the bounding box.
Pressing 'b' again will show it again.


//---------------------------TOON OUTLINE !!---------------------------------//
While not actually cel (toon) shaded, this viewer has the option of rendering
an outline around the mesh that gives it a similar appearance to cel shading.
Shading is still unaffected, but for now, it looks pretty cool.

//---------------------------PHONG SHADING !!--------------------------------//
I have included a phong shading shader.
It's a fully functional shader that requires glew, so make sure that's
installed and use my provided makefile and you should be fine.
(Otherwise, you could get a compiling/linking error.)
Use the -p flag or press p at runtime to toggle it on and off.
It's relatively simple, so it only works with the head light and not the
scene light, unfortunately.  But still, phong shading !!

Most of the code for the shader source files is not my own, it came from
ozone3d.net/tutorials/glsl_lighting_phong_p2.php
Since it's an "extra feature" and not a core task for the assignment, I
figured it wasn't a problem.  It's really just to show off the shaders.


//---------------------------PANNING !!--------------------------------------//
Feel free to use the arrow keys to pan around.
Up is forward, down is backwards, left is left, and right is right.


//-------------------------CUSTOM OBJECT !!----------------------------------//
The original.obj and original.mtl were both made from scratch, and
red.jpg was founding using Google's image search.
I simply made a cube and textured it.
Making anything particularly more complex would require modeling software or
a nice big cup of crazy.


//-------------------------CUSTOM SCENE !!-----------------------------------//
Currently renders a Grass field and some mountains.
Made using scene.obj, scene.mtl file, and two textures.

Screenshot was taken after setting the camera for the best shot.
