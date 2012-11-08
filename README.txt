Name: Taylor McKinney
UT EID: tsm544
CS ID: tsm544

Comments:

TOON OUTLINE !!
While not actually cel (toon) shaded, this viewer has the option of rendering
an outline around the mesh that gives it a similar appearance to cel shading.
Shading is still unaffected, but for now, it looks pretty cool.

Also, anti-aliasing is enabled.  It wasn't difficult, though I'm unsure
exactly which algorithm is being used.

Added a few displaying options
c - enables the toon outline
l - uses a light source fixed to the world (default is fixed to camera)
b - shows a bounding box
a - shows the axis
n - shows the normals
f - uses flat shading (default is smooth)
debug - prints debug statements, for development purposes

Note that the arguments must be separate and must have the '-' flag in front

The program can run with these as arguments or they can be typed at run time
to alternate the option on or off).
i.e., 
viewer [.obj file] -b -a
will run the viewer showing both a bounding box and the axis.

Pressing 'b' at during runtime will hide the bounding box.
Pressing 'b' again will show it again.