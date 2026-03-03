------------------------------------------

This README:  

- Explains what the simulation is in plain terms  
- Explains **controls and GUI**  
- Explains **how to compile and run**  
- Mentions the “central magnetic force” idea  
- Mentions glowing visuals for a polished look  

------------------------------------------
cool visual:
          *****        <- high-energy shell (red)
       *         *
     *             *
    *       O       *   <- central magnetic-like force
     *             *
       *         *
          *****    <- low-energy shell (blue)

------------------------------------------

# Realistic Particle Simulation

A 2D particle simulation in C using [Raylib](https://www.raylib.com/) and [Raygui](https://github.com/raysan5/raygui).  

Particles move freely, collide elastically, and can be optionally influenced by a central “magnetic-like” force that organizes them into energy-based shells. Includes a glowing particle effect and velocity-based coloring.

------------------------------------------

## Features

- **Elastic particle collisions** – particles bounce off each other realistically.  
- **Central force layering** – press `F` to toggle a magnetic-like field that organizes particles into shells based on energy.  
- **Velocity-based coloring** – particles change from blue (slow) to red (fast).  
- **Particle glow** – soft halo effect for a smooth, plasma-like appearance.  
- **GUI controls** via Raygui:
  - Adjust **particle count**  
  - Adjust **speed multiplier** 
 
 ------------------------------------------
 
 ## Controls
 
 Key/GUI || Action
 `F` || Toggle central layering force
 Particle count slider || Increase or dexcrease number of particles
 Speed slider || Adjust simulation speed
 
 ------------------------------------------
 
 ## Requirements
 
- [Raylib](https://www.raylib.com/) (C library for graphics and input)  
- [Raygui](https://github.com/raysan5/raygui) (included in project)  
- C compiler (GCC, MinGW, clang, etc.)  

 ------------------------------------------
 
 ## Build Instructions
 
 Simply clone this repository and it should work just fine, there is already a built file provided (.exe), which works on Windows.
