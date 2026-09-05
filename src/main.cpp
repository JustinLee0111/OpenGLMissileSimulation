#include "Application.h"

// Description: Unfinished missile simulation, currently a custom physics simulator for bouncing sphere on sphere, sphere on static bounded plane. Uses Continuous Collision Detection (CCD) to detect collisions.
// The benefit of CCD is the ability to prevent high-speed tunneling (objects passing through one another) and calculating precise collisions.
// The biggest challenge of CCD is calculating the Time of Impact (TOI). Two methods used is Analytical and Conservative Advancement.
// Analyical solving is much faster but not as reliable and flexible as Conservative Advancement.
// 
// <-- FEATURES -->
// - Continuous Collision Detection solver with both conservative advancement and analytical solver.
// - Bounded plane collisions with dynamic normals for edge collisions (only for sphere and plane collisions).
// - Sphere on sphere collisions using impulses
// - Dynamic level loading with JSON level data
// 
// --- CONTROLS ---
// Left Alt - Lock/Unlock Camera to Mouse
// Escape - Exit Program
// W - Add up velocity
// A - Add left velocity
// S - Add down velocity
// D - Add right velocity
// U - Unload level
// L - Load level (MAY GET LAGGY IF SPAMMED)

using namespace std;

int main() {
	Application mainApplication;
	
	int windowCreate = mainApplication.appInit();

	if (windowCreate == -1) return -1;

	mainApplication.runApp();
	
    return 0;
}