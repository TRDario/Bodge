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

struct extra_graphics {
	tr::gfx::debug_renderer debug_renderer;
	tr::gfx::gpu_benchmark benchmark;
};

struct graphics {
	const tr::gfx::render_target screen;
	tr::gfx::renderer_2d basic_renderer;
	std::optional<extra_graphics> extra;
	blur_renderer blur_renderer;
	tooltip_manager tooltip_renderer;

	graphics();

	float render_scale() const;

	void add_menu_game_overlay_to_renderer();
	void add_fade_overlay_to_renderer(float opacity);
	void draw_cursor();
};
inline std::optional<graphics> g_graphics;