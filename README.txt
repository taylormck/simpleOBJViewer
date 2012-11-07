Name: Taylor McKinney
UT EID: tsm544
CS ID: tsm544

Comments:

Added a few displaying options
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