/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
*/

#include "game/game_state.h"
#include <stdio.h>
#include <stdlib.h>

// Allocate the master global state struct instance declared as an extern in game_state.h
struct GameState gstate;

void rand_gen_seed(uint64_t* seed_ptr) {
	if (!seed_ptr) return;
	
	// A basic linear congruential or splitmix64-style step to initialize pseudo-random fields
	// Mix a starting system address anchor with a temporal numeric constant
	*seed_ptr = 0x123456789ABCDEF0ULL ^ (uint64_t)((uintptr_t)seed_ptr);
}

void config_create(void** config_dest, const char* filepath) {
	if (!config_dest || !filepath) return;

	// In the future, this can hook into a JSON parser (like cJSON) to load from storage.
	// For now, we print a verification trace and initialize an empty handle placeholder.
#ifdef DEBUG
	printf("[Engine] Loading user configuration profile from: %s\n", filepath);
#endif

	*config_dest = NULL; 
}
