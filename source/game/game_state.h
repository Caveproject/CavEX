/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
*/

#ifndef GAME_GAME_STATE_H
#define GAME_GAME_STATE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "world.h"
#include "platform/gfx.h"
#include "game/gui/screen.h"

// Define foundational custom platform timing types matching your main loop variables
typedef uint64_t ptime_t;

/**
 * @brief Animation clock trackers for items and tools held by players.
 */
struct AnimationTimer {
	ptime_t start;
};

struct HeldItemAnimation {
	struct AnimationTimer punch;
	struct AnimationTimer switch_item;
};

/**
 * @brief State container for actively digging/breaking blocks.
 */
struct DiggingState {
	ptime_t cooldown;
	bool active;
};

/**
 * @brief Dynamic engine performance profile statistics.
 */
struct EngineStats {
	float dt;
	float fps;
	float dt_gpu;
	float dt_vsync;
};

/**
 * @brief User configuration properties populated via config.json.
 */
struct UserConfig {
	float fov;
	float render_distance;
	float fog_distance;
};

/**
 * @brief Global monolithic Game State Structure tracking engine runtime layers.
 */
struct GameState {
	bool quit;
	bool world_loaded;
	
	struct UserConfig config;
	struct EngineStats stats;
	struct HeldItemAnimation held_item_animation;
	struct DiggingState digging;
	
	uint64_t rand_src;           // Seed tracker for pseudo-random noise generation
	void* config_user;           // Handle for user configuration JSON files
	
	struct world world;          // Voxel chunk map data structure
	uint64_t world_time;         // Raw ticks tracking total daytime progression
	ptime_t world_time_start;    // Real-time anchor tracking loop elapsed moments
	
	struct entity* local_player; // Local tracking shortcut to Player 1 entity pointer
	void* entities;              // Master manager list container for current active map entities
	
	struct screen* current_screen; // Active UI window or inventory menu overlay
	void* windows[256];          // GUI pointer lookup table tracking active window frames
};

// Expose the global state object instance referenced in main.c
extern struct GameState gstate;

/**
 * @brief Helper utility functions for custom seed randomization algorithms.
 */
void rand_gen_seed(uint64_t* seed_ptr);

/**
 * @brief Instantiates user configurations matching a named system file string path.
 */
void config_create(void** config_dest, const char* filepath);

#endif // GAME_GAME_STATE_H
