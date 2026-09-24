#define GLFW_INCLUDE_NONE

#include "core/Application.h"

// Description: Working basic missile simulation combined with a custom physics simulator for bouncing sphere on sphere and sphere on bounded plane. Uses Continuous Collision Detection (CCD) to detect collisions.
// Using proportional navigation to guide the missile.
// The benefit of CCD is the ability to prevent high-speed tunneling (objects passing through one another) and calculating precise collisions.
// The biggest challenge of CCD is calculating the Time of Impact (TOI). Two methods used is Analytical and Conservative Advancement.
// Analyical solving is much faster but not as reliable and flexible as Conservative Advancement.
// 
// <-- FEATURES -->
// - Proportional navigation guided missile with an engine
// - Continuous Collision Detection solver with both conservative advancement and analytical solver.
// - Bounded plane collisions with dynamic normals for edge collisions (only for sphere and plane collisions).
// - Sphere on sphere collisions using impulses
// - Dynamic level loading with JSON level data
// 
// --- CONTROLS ---
// Left Alt - Lock/Unlock Camera to Mouse
// Escape - Exit Program
// 1 - Camera 1 (Missile chase camera in MissileSim.json level, or default cam)
// 2 - Camera 2 (If it exists in the level)
// 
// W - Add forward velocity
// A - Add left velocity
// S - Add back velocity
// D - Add right velocity
// Left Arrow - Rotate counter-clockwise
// Right Arrow - Rotate clockwise
// R - Disable/Enable Missile Seeker
// F - Deploy Flares
// Space - Enable missile's engine
// Left Shift - Find target within gimbal limit
// 
// U - Unload level
// M - Load fast missile level
// N - Load slower missile level
// B - Load alternate slower missile level
// V - Load level with a still missile
// L - Load level
// T - Load level with moving plane

using namespace std;

int main() {
	Application mainApplication;
	
	int windowCreate = mainApplication.appInit();

	if (windowCreate == -1) return -1;

	mainApplication.runApp();
	
    return 0;
}