# The Lost Lantern

A 2D story game built with C++ and OpenGL (FreeGLUT) for the CS2206 Computer Graphics course.

A small traveler moves through a dark world to collect **3 light fragments** and relight the village lantern.

## Scenes

| # | Scene | What happens |
|---|---|---|
| 1 | Start Screen | Title, short story intro and the list of controls |
| 2 | Dark Village | The village is dark; clicking the mouse activates the lantern and lights up the houses |
| 3 | Forest Path | First light fragment, a rotating star between the trees |
| 4 | Windy Bridge | Second light fragment; the bridge swings slightly and the water waves move |
| 5 | Lantern Tower | Final light fragment; the tower uses a stone texture and its lantern glows |
| 6 | Final Scene | "Light Restored!" The village lantern and houses are lit |

A counter at the top of the screen shows the current scene and how many fragments have been collected.

## Technologies

- C++
- OpenGL
- FreeGLUT

## Computer graphics concepts applied

- **2D shapes**: circles (triangle fan), triangles, rectangles, lines and stars, combined into houses, trees, the hero, the moon, the bridge, the tower and the lantern
- **Transformations**
  - Translation: hero movement and moving clouds
  - Rotation: spinning light fragments and the swinging bridge
  - Scaling: the lantern glow grows and shrinks
- **Animation**: an idle callback updates clouds, stars, waves, the bridge and the glow every frame
- **Keyboard and mouse interaction**
- **Texture mapping**: two checker textures (grass and stone) generated in code, so no image files are needed
- **Text rendering**: scene titles, instructions and the progress counter using GLUT bitmap fonts

## Controls

| Key | Action |
|---|---|
| `N` | Next scene |
| `B` | Previous scene |
| Arrow keys | Move the character |
| `Space` | Collect the light fragment / scene action |
| `R` | Reset the character position and lantern in the current scene |
| Left mouse click | Activate the lantern |
| `Q` / `Esc` | Quit |

## How to run

The project was built with **Visual Studio** on Windows.

1. Set up OpenGL and FreeGLUT in Visual Studio (include and library directories).
2. Add `TheLostLantern.cpp` to an empty C++ project.
3. Link these libraries: `freeglut.lib`, `opengl32.lib`, `glu32.lib`.
4. Make sure `freeglut.dll` is next to the executable (or on the system path).
5. Build and run.

## Team project

This was a team project by 4 students for **CS2206 Computer Graphics**.

My part:
- The **Forest Path** scene
- The logic for collecting the **first light fragment**
- **Character movement** with the keyboard (arrow keys)
