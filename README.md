# OpenGL Particle Fountain

## How to Run

- Install Visual Studio
- Open the solution file `particle-fountain.sln`
- Build the solution or hit F5 (Run without debugging) to run

OR

- Run the `particle-fountain.exe` in the `particle-fountain/Debug` folder.

## Description and Features

This program accounts for all of the requirements in the assignment outline with the addition of 5 extra features.

### Required Features

- Gravity
    - Decided to use a gravity constant of 0.0981 to stay roughly true to life
    - In the bonus feature, the moon gravity is set to 0.0162 to stay relatively proportional
- Ground bouncing
    - Reducing the speed by 50% per bounce seemed to have the best result
- Die after falling off edge
    - All particles have a lifespan of 240 frames, meaning fallen particles will be removed
- Friction ground
    - When particles contact the ground, their bounce and their speed gets reduced until they are static
    - They despawn after 240 frames automatically
    
---
    
- S - **Random speed toggle**
    - Toggles the speed randomizer for the projectile velocity
    - Speed is 1 when disabled
- Q - **Fire mode toggle**
    - Cycles between automatic continuous mode, manual hold-to-fire, and manual single-shot, as described in assignment outline
    - Manual and single-shot mode is controlled with F
- Z - **Random scale toggle**
    - Switches between static scale for all particles and randomized scale
- R - **Random rotation toggle**
    - Toggles between no rotation and random spin rotation amount per particle
- H - **Toggle menu**
    - Show/hide the menu overlay
- I - **Reset/initialize**
    - Clears particles and resets all parameters to default s
- ESC - **Quit**
    - Closes the window
    
---
	

### Extra Features

- Normals and lighting
- C - **Random colour toggle**
    - Toggles between random RGB colour values and static colour blue
- X - **Random shape toggle**
    - Toggles between random shape for each particle and shape selector
    - Switch between shapes using number keys
    - Shapes include:
        - 1 - Sphere
        - 2 - Cube
        - 3 - Tetrahedron
        - 4 - Torus
        - 5 - Cone
        - 6 - Cylinder
- G - **Moon gravity (new effect)**
    - Lowers the particle gravity
- A - **Spray mode toggle**
    - Toggles between a narrow spray stream and wide spray stream