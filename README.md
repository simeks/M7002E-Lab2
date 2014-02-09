M7002E-Lab2
===========
Simon Ekström

Dependencies:
SDL: Simple DirectMedia Layer, http://www.libsdl.org/
GLEW: The OpenGL Extension Wrangler Library, http://glew.sourceforge.net/

Core features:
- Supported primitives: Pyramids, Cubes, Spheres.
- Shapes with customizable material parameters (ambient, specular, diffuse, etc).
- Light sources with customizable parameters (ambient, specular, diffuse, etc).
- Object manipulation (move, resize, delete, etc).

Extra features:
- Dynamic moving, resizing and rotating of objects with the mouse.
- Loading and saving of the scene.
- Utilizes GLSL shaders.

User manual:

Creating shapes:
- [1] : Creates a pyramid at the current mouse location.
- [2] : Creates a cube at the current mouse location.
- [3] : Creates a sphere at the current mouse location.
- [4] : Creates a point-light at the current mouse location.

Manipulating shapes:
- [Left mouse] : Select an object by clicking on it.
- [Left mouse] : Move an object around on the x- and the z-axis by dragging it with the mouse.
- [Left ctrl] + [Left mouse] : Moves an object around on the y-axis.
- [S] + [Left mouse] : Scale an object on the x-, and the z-axis. Scaling a light source will simply scale its radius.
- [Left ctrl] + [S] + [Left mouse] : Scale an object on the y-axis.
- [R] + [Left mouse] : Rotate an object.
- [Delete] : Deletes the selected object.
- [Left ctrl] + [Delete] : Deletes all objects.

Misc:
- [Escape] : Exits the program.
- [F1] : Saves the current scene to the file "scene.json".
- [F2] : Loads a scene from the file "scene.json".

Material/Light properties:

When selecting either a light source or an object a color picker will popup in the lower right corner. This color picker allows you to set material and light properties.
The first (top) set of gradients selects the ambient color, the second selects the specular color and the third selects the diffuse color.

Saving:

The scene is always saved automatically when the user exits program and then automatically loaded when the user starts the program again.
The scene is saved to a filed called "scene.json" which should be located in the same folder as the executable. 
The save-file is formatted in JSON, which is human readable so it's possible to manipulate the saved scene with a basic text editor.

Future work:

The biggest problem currently is the lack of precision when selecting objects with a more complex shape, like the pyramid. This is caused by fact that the program uses bounding spheres when selecting entities. Possible solutions for this could be to use more precise bounding volumes, or to use color picking.

