#pragma once
#include "blur_renderer.hpp"
#include "tooltip_manager.hpp"

enum layer {
	BALL_TRAILS,
	BALL_TRAILS_OVERLAY,
	LIFE_FRAGMENTS,
	BALLS,
	PLAYER_TRAIL,
	PLAYER,
	BORDER,
	GAME_OVERLAY,
	UI,
	UI_2,
	UI_3,
	TOOLTIP,
	FADE_OVERLAY,
	CURSOR
};

namespace engine {
	void initialize_graphics();
	void shut_down_graphics();

	tr::gfx::renderer_2d& basic_renderer();
	tr::gfx::debug_renderer& debug_renderer();
	blur_renderer& blur_renderer();
	tooltip_manager& tooltip();
	tr::gfx::gpu_benchmark& gpu_benchmark();

	float render_scale();
	const tr::gfx::render_target& screen();

	void add_menu_game_overlay_to_renderer();
	void add_fade_overlay_to_renderer(float opacity);
} // namespace engine