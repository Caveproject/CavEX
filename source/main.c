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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef PLATFORM_WII
#include <fat.h>
#include <wiiuse/wpad.h>
#endif

#include "chunk_mesher.h"
#include "daytime.h"
#include "game/game_state.h"
#include "game/gui/screen.h"
#include "graphics/gfx_util.h"
#include "graphics/gui_util.h"
#include "item/recipe.h"
#include "network/client_interface.h"
#include "network/server_interface.h"
#include "network/server_local.h"
#include "particle.h"
#include "platform/gfx.h"
#include "platform/input.h"
#include "world.h"

#include "cNBT/nbt.h"
#include "cglm/cglm.h"
#include "lodepng/lodepng.h"

int main(void) {
	bool multiplayer_enabled = true;
	bool split_vertical = true; // false = horizontal split

	struct camera camera1 = {0}, camera2 = {0};
	struct entity* player1 = NULL;
	struct entity* player2 = NULL;

	gstate.quit = false;
	gstate.config.fov = 70.0F;
	gstate.config.render_distance = 192.0F;
	gstate.config.fog_distance = 5 * 16.0F;
	gstate.world_loaded = false;
	gstate.held_item_animation.punch.start = time_get();
	gstate.held_item_animation.switch_item.start = time_get();
	gstate.digging.cooldown = time_get();
	gstate.digging.active = false;

	rand_gen_seed(&gstate.rand_src);

#ifdef PLATFORM_WII
	fatInitDefault();
	WPAD_Init();
#endif

	config_create(&gstate.config_user, "config.json");

	input_init();
	blocks_init();
	items_init();
	recipe_init();
	gfx_setup();

	screen_set(&screen_select_world);

	world_create(&gstate.world);

	for(size_t k = 0; k < 256; k++)
		gstate.windows[k] = NULL;

	clin_init();
	svin_init();
	chunk_mesher_init();
	particle_init();

	player1 = entity_local_player_create(&gstate.world, 0.0f, 64.0f, 0.0f);
	player2 = entity_local_player_create(&gstate.world, 2.0f, 64.0f, 0.0f);
	gstate.local_player = player1;

	camera_init(&camera1);
	camera_init(&camera2);

	struct server_local server;
	server_local_create(&server);

	ptime_t last_frame = time_get();
	ptime_t last_tick = last_frame;

	while (!gstate.quit) {
		ptime_t this_frame = time_get();
		gstate.stats.dt = time_diff_s(last_frame, this_frame);
		gstate.stats.fps = 1.0F / gstate.stats.dt;
		last_frame = this_frame;

		float daytime = (float)((gstate.world_time + time_diff_ms(gstate.world_time_start, this_frame) / DAY_TICK_MS) % DAY_LENGTH_TICKS) / (float)DAY_LENGTH_TICKS;

		clin_update();

		float tick_delta = time_diff_s(last_tick, time_get()) / 0.05F;
		while (tick_delta >= 1.0F) {
			last_tick = time_add_ms(last_tick, 50);
			tick_delta -= 1.0F;
			particle_update();
			entities_client_tick(gstate.entities);
		}

		// Update input states for all players
		updateInput();

		// Process input for players here if needed, e.g. movement or actions:
		// Example (pseudo):
		// if (isKeyPressed(0, IB_JUMP)) { /* player1 jump logic */ }
		// if (isKeyPressed(1, IB_JUMP)) { /* player2 jump logic */ }

		camera_attach(&camera1, player1, tick_delta, gstate.stats.dt);
		camera_attach(&camera2, player2, tick_delta, gstate.stats.dt);

		camera_update(&camera1, false);
		camera_update(&camera2, false);

		world_update_lighting(&gstate.world);
		world_build_chunks(&gstate.world, CHUNK_MESHER_QLENGTH);

		if (gstate.current_screen->update)
			gstate.current_screen->update(gstate.current_screen, gstate.stats.dt);

		gfx_flip_buffers(&gstate.stats.dt_gpu, &gstate.stats.dt_vsync);
		chunk_mesher_receive();
		world_render_completed(&gstate.world, true);

		vec3 top_plane_color, bottom_plane_color, atmosphere_color;
		daytime_sky_colors(daytime, top_plane_color, bottom_plane_color, atmosphere_color);

		gfx_clear_buffers(atmosphere_color[0], atmosphere_color[1], atmosphere_color[2]);
		gfx_fog_color(atmosphere_color[0], atmosphere_color[1], atmosphere_color[2]);

		int width = gfx_width();
		int height = gfx_height();
		int half_width = width / (split_vertical ? 2 : 1);
		int half_height = height / (split_vertical ? 1 : 2);

		int num_views = multiplayer_enabled ? 2 : 1;
		for (int i = 0; i < num_views; ++i) {
			struct camera* cam = i == 0 ? &camera1 : &camera2;
			int x = split_vertical ? i * half_width : 0;
			int y = split_vertical ? 0 : i * half_height;

			GX_SetViewport(x, y, half_width, half_height, 0.0f, 1.0f);
			GX_SetScissor(x, y, half_width, half_height);

			gfx_mode_world();
			gfx_matrix_projection(cam->projection, true);
			gfx_update_light(daytime_brightness(daytime), world_dimension_light(&gstate.world));

			if (gstate.world.dimension == WORLD_DIM_OVERWORLD)
				gutil_sky_box(cam->view, daytime_celestial_angle(daytime), top_plane_color, bottom_plane_color);

			world_render(&gstate.world, cam, false);
			entities_client_render(gstate.entities, cam, tick_delta);
			world_render(&gstate.world, cam, true);

			if (gstate.world.dimension == WORLD_DIM_OVERWORLD)
				gutil_clouds(cam->view, daytime_brightness(daytime));
		}

		gfx_mode_gui();

		if (gstate.current_screen->render2D)
			gstate.current_screen->render2D(gstate.current_screen, width, height);

		// Screenshot input: Check player 0 (you can expand to player 1)
		if (isKeyPressed(0, IB_SCREENSHOT)) {
			size_t width, height;
			gfx_copy_framebuffer(NULL, &width, &height);
			void* image = malloc(width * height * 4);
			if (image) {
				gfx_copy_framebuffer(image, &width, &height);
				char name[64];
				snprintf(name, sizeof(name), "%ld.png", (long)time(NULL));
				lodepng_encode32_file(name, image, width, height);
				free(image);
			}
		}

		gfx_finish(true);
	}

	return 0;
}
