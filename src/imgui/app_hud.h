#pragma once

class World;
class Application;

class AppHUD {
public:
	static void ShowAppHUD(Application& mainWindow, World& world);
	static inline int item_selected_idx;
};