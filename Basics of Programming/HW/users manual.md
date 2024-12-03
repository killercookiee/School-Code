I/ Introduction and Goal of the Program
    The goal of this program is the creation of a 3D graphics engine intended to create, manipulate, and render objects with colour, transparency, within a virtual 3D space of N*N*N pixels using line-based construction. The 3D render will then be projected on a 2D plane on the screen with an ASCII representation.

    Line-based construction means the usage of Bresenham’s line algorithm in 3D to create lines between points, allowing the possibility of defining objects like cubes using a series of connected lines. Line based construction will also depend on the line origin preset that we choose such as it being only originating from 1 point, from 1 face, from a series of points on a face etc… 
    
    Object manipulation should be able to undergo basic transformations like rotation.




II/ Instructions And User Guide
Activation:
If you are not using a built in compiler like me, run in the terminal:
    gcc user_GUI.c animation.c -o program
    ./program

Menu Overview: The user interaction and inputs in the menu will be as follows:
	1. Choose line construction: 
        1. Limitless
        2. Go Back

    2. Create Object: Lets the user specify an object type
        1. Choose from Sample
            1. Hollow Sphere
            2. Filled Sphere
            3. Hollow Cube
            4. Filled Cube
            5. Hollow Donut
            6. Filled Donut
            7. Go Back
        2. Go Back

    3. Transform Object: Options to rotate the object.
        1. Rotate object
        2. Go back

    4. Save/Load Object: Allows saving the current 3D scene to a file and reloading it.
        1. Save
        2. Load
        3. Go Back

    5. Exit: Ends the program.





III/ File Saves
Files can be saved and loaded in Object Presets and Render Presets folder:
Main Folder
    Object Presets
        - example_object.txt            <- Files saved and loaded here
    Render Presets
        - example_render.txt            <- Files saved and loaded here
    animation.h
    animation.c
    user_GUI.c