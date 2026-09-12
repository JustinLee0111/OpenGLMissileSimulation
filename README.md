
<h2  style="margin-bottom: 0px;">OpenGL Missile Simulation</h2>

  

**<u>Description:</u>**

- Unfinished missile simulation, currently a custom physics simulator for bouncing sphere on sphere, sphere on static bounded plane. Uses Continuous Collision Detection (CCD) to detect collisions.
- The benefit of CCD is the ability to prevent high-speed tunneling (objects passing through one another) and calculating precise collisions.
- The biggest challenge of CCD is calculating the Time of Impact (TOI). Two methods used is Analytical and Conservative Advancement.
- Analyical solving is much faster but not as reliable and flexible as Conservative Advancement.

**<u>--- CONTROLS ---</u>**
- Left Alt - Lock/Unlock Camera to Mouse
- Escape - Exit Program
- W - Add up velocity to sphere
- A - Add left velocity to sphere
- S - Add down velocity to sphere
- D - Add right velocity to sphere
- U - Unload level
- L - Load level
- T - Load level with moving plane
- M - Load missile level
- Left Arrow - Rotate counter-clockwise
- Right Arrow - Rotate clockwise
  
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
