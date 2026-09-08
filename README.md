
<h2  style="margin-bottom: 0px;">OpenGL Missile Simulation</h2>

  

**<u>Description:</u>**

Project to learn OpenGL and refine C++ coding skills. Currently a custom collision physics simulator using Continuous Collision Detection with spheres and bounded planes.

**<u>--- CONTROLS ---</u>**
- Left Alt - Lock/Unlock Camera to Mouse
- Escape - Exit Program
- W - Add up velocity to sphere
- A - Add left velocity to sphere
- S - Add down velocity to sphere
- D - Add right velocity to sphere
- U - Unload level
- L - Load level (MAY GET LAGGY IF SPAMMED)
  
  **<u>-- FEATURES --</u>**
- Continuous Collision Detection solver with both conservative advancement and analytical Time of Impact (TOI) solver.
- Bounded plane collisions with dynamic normals for edge collisions (only for sphere and plane collisions).
- Sphere on sphere collisions using impulses
- Dynamic level loading with JSON level data

**<u>Currently:</u>**

Basic ball physics simulation

  

**<u>Endgoal:</u>**

A basic lightweight missile simulation using OpenGL. Simulates infrared target tracking, working aerodynamics, IRCCM, thrust vectoring, and proximity fuse.

  

**<u>Requirements</u>**

- Windows 10 or 11

- OpenGL 4.6

- Visual Studio 2022+ (Desktop Dev w/ C++)
