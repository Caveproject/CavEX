/*
	Copyright (c) 2022 ByteBit/xtreme8000

	This file is part of CavEX.

	CavEX is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	CavEX is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with CavEX.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PLAYERS 2

typedef struct {
    float x;
    float y;
} Vec2;

// Input buttons enumeration
enum input_button {
	IB_FORWARD,
	IB_BACKWARD,
	IB_LEFT,
	IB_RIGHT,
	IB_ACTION1,
	IB_ACTION2,
	IB_JUMP,
	IB_SNEAK,
	IB_INVENTORY,
	IB_HOME,
	IB_SCROLL_LEFT,
	IB_SCROLL_RIGHT,
	IB_GUI_UP,
	IB_GUI_DOWN,
	IB_GUI_LEFT,
	IB_GUI_RIGHT,
	IB_GUI_CLICK,
	IB_GUI_CLICK_ALT,
	IB_SCREENSHOT,
	IB_TOTAL_KEYS  // Total number of buttons (for iteration)
};

enum input_category {
	INPUT_CAT_WIIMOTE,
	INPUT_CAT_NUNCHUK,
	INPUT_CAT_CLASSIC_CONTROLLER,
	INPUT_CAT_NONE,
};

// Initialize the input system
void input_init(void);

// Poll and update input states; call once per frame
void updateInput(void);

// Query input states per player (0-based player index)
bool isKeyDown(int player, int key);
bool isKeyPressed(int player, int key);
bool isKeyReleased(int player, int key);

// Get analog stick values per player
Vec2 getLeftStick(int player);
Vec2 getRightStick(int player);

#ifdef __cplusplus
}
#endif

#endif // INPUT_H
