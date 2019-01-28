#ifndef SETTINGS_H
#define SETTINGS_H

class Settings {
public:
	// Settings values for when a new scene file is created
	constexpr static double NEW_SCENE_GRID_SPACING = 0.2;
	const static int NEW_SCENE_WINDOW_SIZE = 32;
	constexpr static double NEW_SCENE_RENDER_TIME = 1.0;
	constexpr static double NEW_SCENE_AIR_DENSITY = 1.2;
	const static int NEW_SCENE_SOUND_SPEED = 340;
	const static int NEW_SCENE_PML_CELLS = 50;
	const static int NEW_SCENE_ATTENUATION = 20000;
	
	// Absorption values for when a new domain is created
	constexpr static double NEW_DOMAIN_ABSORPTION_TOP = 0;
	constexpr static double NEW_DOMAIN_ABSORPTION_BOTTOM = 0;
	constexpr static double NEW_DOMAIN_ABSORPTION_LEFT = 0;
	constexpr static double NEW_DOMAIN_ABSORPTION_RIGHT = 0;
	
	// ELR values for when a new domain is created
	const static bool NEW_DOMAIN_ELR_TOP = false;
	const static bool NEW_DOMAIN_ELR_BOTTOM = false;
	const static bool NEW_DOMAIN_ELR_LEFT = false;
	const static bool NEW_DOMAIN_ELR_RIGHT = false;
};

#endif