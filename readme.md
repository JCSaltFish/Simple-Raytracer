SIMPLE RAYTRACER USING OPENMP ACCELERATION
Jed Wang
jcsaltfish@gmail.com

![image] (https://github.com/JCSaltFish/Simple-Raytracer/blob/master/img/img01.png)

- Only supports for specular reflections.

- Scene is described by the txt file.
	Object types:
	- LIGHT
	- SPHERE
	- QUAD

- More options can be found on the scene description text file.

- Uses OpenMP and OpenGL output to make the animation possible.

- All objects (including lights) can be animated.
	The animation is defined by 3 attributes in the scene description file:
	- MOVEDIR x y z
	- MOVEDISTANCE x
	- MOVESPEED x

- Implemented Super Sampling Anti-Aliasing (SSAA).
	Defined by ANTIALIAS tag in the scene description file:
	- Set to 0 or 1 to disable SSAA
	- Set to a value > 1 to enable SSAA.