# Users Manual — 3D ASCII Graphics Engine

## I. Introduction and Goal of the Program

The goal of this program is the creation of a 3D graphics engine intended to create, manipulate, and render objects with colour and transparency within a virtual 3D space of N×N×N voxels using line-based construction. The 3D scene is projected onto a 2D plane and displayed as an ASCII image directly in the terminal.

**Line-based construction** uses Bresenham's line algorithm in 3D to draw lines between points, allowing objects such as cubes and spheres to be defined as a series of connected edges. The direction and origin of lines can be controlled through the construction mode selected at runtime.

Object manipulation supports basic transformations such as rotation with real-time frame-by-frame rendering.

---

## II. Build Instructions

Compile with any C++17-compatible compiler:

```bash
g++ -std=c++17 user_GUI.cpp animation.cpp -o program -lm
./program
```

On Windows (MSVC):
```
cl /std:c++17 user_GUI.cpp animation.cpp /Fe:program.exe
```

---

## III. Menu Overview

### 1. Choose Line Construction
Select the line generation strategy applied when constructing objects.

- **1. Limitless** — Lines can originate from any point in the space.
- **2. Go Back** — Return to the main menu.

### 2. Create Object
Construct a new 3D object. You will be prompted for:

| Parameter | Description |
|---|---|
| 3D space size N | Side length of the voxel cube (e.g. 20) |
| Medium transparency | Background voxel transparency 0.0–1.0 |
| Medium brightness | Background voxel brightness 0.0–1.0 |
| Line brightness | Brightness value applied along construction lines 0.0–1.0 |
| Shape parameters | Size-specific values (diameter, side length, radii) |

Available sample shapes:
1. Hollow Sphere
2. Filled Sphere
3. Hollow Cube
4. Filled Cube
5. Hollow Donut
6. Filled Donut
7. Go Back

### 3. Transform Object
Rotate the current object in real time.

1. **Rotate object** — Enter per-axis rotation speed (degrees/second) for X, Y, Z and a total duration in seconds. The engine renders the animation at 12 FPS.
2. **Go Back**

### 4. Save / Load Object
Persist the current scene to disk or reload a previously saved scene.

1. **Save** — Enter a name; files are written to `Object Presets/<name>.txt` and `Render Presets/<name>.txt`.
2. **Load** — Lists available saves; enter the corresponding number to load.
3. **Go Back**

### 5. Exit
Cleanly frees all memory and exits the program.

---

## IV. File Layout

```
Project Root/
├── animation.hpp         ← Class & function declarations
├── animation.cpp         ← Engine implementation
├── user_GUI.cpp          ← User interface & main()
├── Object Presets/
│   └── example_object.txt
└── Render Presets/
    └── example_render.txt
```