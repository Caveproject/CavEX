/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
*/

#include "platform/input.h"
#include <string.h>
#include <stdbool.h>

#ifdef PLATFORM_WII
#include <wiiuse/wpad.h>
#endif

// Support arrays for up to 2 local players
#define MAX_PLAYERS 2

struct PlayerInputState {
	bool keys_pressed[IB_MAX_BUTTONS];
	bool keys_held[IB_MAX_BUTTONS];
	float look_x;
	float look_y;
};

static struct PlayerInputState g_players[MAX_PLAYERS];

void input_init(void) {
	memset(g_players, 0, sizeof(g_players));
}

void updateInput(void) {
	// Shift states from "pressed" to "held" for the next frame
	for (int p = 0; p < MAX_PLAYERS; p++) {
		for (int i = 0; i < IB_MAX_BUTTONS; i++) {
			g_players[p].keys_pressed[i] = false;
		}
	}

#ifdef PLATFORM_WII
	// Native hardware polling loop for Nintendo Wii controllers
	WPAD_ScanPads();
	
	for (int p = 0; p < MAX_PLAYERS; p++) {
		uint32_t down = WPAD_ButtonsDown(p);
		uint32_t held = WPAD_ButtonsHeld(p);
		
		// Map structural physical inputs to custom engine actions
		if (down & WPAD_BUTTON_A)        g_players[p].keys_pressed[IB_JUMP] = true;
		if (held & WPAD_BUTTON_A)        g_players[p].keys_held[IB_JUMP] = true;
		else                             g_players[p].keys_held[IB_JUMP] = false;
		
		if (down & WPAD_BUTTON_1)        g_players[p].keys_pressed[IB_ATTACK] = true;
		if (held & WPAD_BUTTON_1)        g_players[p].keys_held[IB_ATTACK] = true;
		else                             g_players[p].keys_held[IB_ATTACK] = false;

		if (down & WPAD_BUTTON_PLUS)     g_players[p].keys_pressed[IB_PAUSE] = true;
		if (down & WPAD_BUTTON_2)        g_players[p].keys_pressed[IB_SCREENSHOT] = true;
	}
#else
	// --- DESKTOP DESKTOP PC FALLBACK POLL ---
	// Player 0 (Keyboard/Mouse Standard bindings)
	// (Simulated binding hook calling standard GLFW/SDL/Win32 backend abstractions)
	/* 
	if (native_key_down(KEY_SPACE)) g_players[0].keys_pressed[IB_JUMP] = true;
	if (native_key_held(KEY_SPACE)) g_players[0].keys_held[IB_JUMP] = true;
	*/
#endif
}

bool isKeyPressed(int player_id, enum InputButton button) {
	if (player_id < 0 || player_id >= MAX_PLAYERS) return false;
	if (button < 0 || button >= IB_MAX_BUTTONS) return false;
	return g_players[player_id].keys_pressed[button];
}

bool isKeyHeld(int player_id, enum InputButton button) {
	if (player_id < 0 || player_id >= MAX_PLAYERS) return false;
	if (button < 0 || button >= IB_MAX_BUTTONS) return false;
	return g_players[player_id].keys_held[button];
}

void input_get_look_axis(int player_id, float* out_x, float* out_y) {
	if (player_id < 0 || player_id >= MAX_PLAYERS) {
		*out_x = 0.0f;
		*out_y = 0.0f;
		return;
	}
	*out_x = g_players[player_id].look_x;
	*out_y = g_players[player_id].look_y;
}
