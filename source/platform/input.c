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

#include <assert.h>

#include "input.h"
#include <math.h>

#ifdef PLATFORM_PC
#include <SDL2/SDL.h>
#endif

#ifdef PLATFORM_WII
#include <wiiuse/wpad.h>
#endif

#define MAX_PLAYERS 2

typedef struct {
    float x, y;
} Vec2;

Vec2 leftStick[MAX_PLAYERS] = {{0}};
Vec2 rightStick[MAX_PLAYERS] = {{0}};  // Reserved for future use if needed
bool keys[MAX_PLAYERS][256] = {{0}};
bool prevKeys[MAX_PLAYERS][256] = {{0}};

// For PC or single Wii remote fallback (default to 0)
int inputChannel = 0;

#ifdef PLATFORM_WII
// Map your key indices to Wii buttons (customize if needed)
u32 mapKeyToWiiButton(int key) {
    switch (key) {
        case 0: return WPAD_BUTTON_A;
        case 1: return WPAD_BUTTON_B;
        case 2: return WPAD_BUTTON_PLUS;
        case 3: return WPAD_BUTTON_MINUS;
        case 4: return WPAD_BUTTON_HOME;
        case 5: return WPAD_BUTTON_1;
        case 6: return WPAD_BUTTON_2;
        case 7: return WPAD_BUTTON_UP;
        case 8: return WPAD_BUTTON_DOWN;
        case 9: return WPAD_BUTTON_LEFT;
        case 10: return WPAD_BUTTON_RIGHT;
        default: return 0;
    }
}
#endif

void updateInput() {
#ifdef PLATFORM_PC
    // PC updates single input (no multiplayer)
    const Uint8 *state = SDL_GetKeyboardState(NULL);
    for (int i = 0; i < 256; ++i) {
        prevKeys[0][i] = keys[0][i];
        keys[0][i] = state[i];
    }

    int x, y;
    Uint32 mouseButtons = SDL_GetMouseState(&x, &y);
    leftStick[0].x = (float)x;
    leftStick[0].y = (float)y;
#endif

#ifdef PLATFORM_WII
    WPAD_ScanPads();

    for (int player = 0; player < MAX_PLAYERS; ++player) {
        s32 err = WPAD_Probe(player, NULL);
        if (err == WPAD_ERR_NO_CONTROLLER) {
            // No controller connected for this player, clear input
            for (int i = 0; i < 256; ++i) {
                prevKeys[player][i] = keys[player][i];
                keys[player][i] = false;
            }
            leftStick[player].x = 0;
            leftStick[player].y = 0;
            continue;
        }

        u32 heldButtons = WPAD_ButtonsHeld(player);

        for (int i = 0; i < 256; ++i) {
            prevKeys[player][i] = keys[player][i];
            keys[player][i] = (heldButtons & mapKeyToWiiButton(i)) != 0;
        }

        WPADData *data = WPAD_Data(player);
        if (data->exp.type == WPAD_EXP_NUNCHUK) {
            leftStick[player].x = (float)data->exp.nunchuk.js.pos.x;
            leftStick[player].y = (float)data->exp.nunchuk.js.pos.y;
        } else {
            leftStick[player].x = 0;
            leftStick[player].y = 0;
        }
    }
#endif
}

// Query functions now take player number (0 or 1)
bool isKeyDown(int player, int key) {
    if (player < 0 || player >= MAX_PLAYERS) return false;
    return keys[player][key];
}

bool isKeyPressed(int player, int key) {
    if (player < 0 || player >= MAX_PLAYERS) return false;
    return keys[player][key] && !prevKeys[player][key];
}

bool isKeyReleased(int player, int key) {
    if (player < 0 || player >= MAX_PLAYERS) return false;
    return !keys[player][key] && prevKeys[player][key];
}

// Optional: Set input channel for single player (mostly for PC or fallback)
void setInputChannel(int channel) {
    inputChannel = channel;
}

// Get the left stick position of a player
Vec2 getLeftStick(int player) {
    if (player < 0 || player >= MAX_PLAYERS) return (Vec2){0,0};
    return leftStick[player];
}
