/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.
*/

#ifndef PLATFORM_INPUT_H
#define PLATFORM_INPUT_H

#include <stdbool.h>

/**
 * @brief Engine-level abstract button mappings.
 * This decouples your physical device bindings (Wiimote/Keyboards) from gameplay logic.
 */
enum InputButton {
	IB_JUMP,
	IB_ATTACK,
	IB_USE,
	IB_PAUSE,
	IB_SCREENSHOT,
	IB_FORWARD,
	IB_BACKWARD,
	IB_LEFT,
	IB_RIGHT,
	IB_SNEAK,
	IB_INVENTORY,
	
	// Keep this at the absolute bottom to automatically track total array size
	IB_MAX_BUTTONS 
};

/**
 * @brief Initializes global input memory storage structures.
 */
void input_init(void);

/**
 * @brief Polls physical hardware layers (e.g., WPAD_ScanPads or Desktop Event Handlers)
 * and shifts temporal buffer frames.
 */
void updateInput(void);

/**
 * @brief Checks if a specific action button was freshly pressed down on this frame.
 * 
 * @param player_id Index of the local controller (0 = Player 1, 1 = Player 2).
 * @param button The target abstract InputButton mapping.
 * @return true If the action was activated this frame.
 */
bool isKeyPressed(int player_id, enum InputButton button);

/**
 * @brief Checks if a specific action button is actively being held down.
 * 
 * @param player_id Index of the local controller (0 = Player 1, 1 = Player 2).
 * @param button The target abstract InputButton mapping.
 * @return true If the button continues to be depressed.
 */
bool isKeyHeld(int player_id, enum InputButton button);

/**
 * @brief Retrieves the analog look/camera tracking axes for a given player.
 * 
 * @param player_id Index of the local controller (0 = Player 1, 1 = Player 2).
 * @param out_x Pointer to receive horizontal delta movement.
 * @param out_y Pointer to receive vertical delta movement.
 */
void input_get_look_axis(int player_id, float* out_x, float* out_y);

#endif // PLATFORM_INPUT_H
