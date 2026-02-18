///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides an interface for rendering graphics.                                                                                         //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "renderer/blur_renderer.hpp"
#include "renderer/tooltip_manager.hpp"

///////////////////////////////////////////////////////////////// RENDERER ////////////////////////////////////////////////////////////////

// Renderer layers.
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

// Extra renderer components.
struct renderer_extra {
	// Debug renderer for displaying performance statistics.
	tr::gfx::debug_renderer debug;
	// GPU benchmark measuring drawing performance.
	tr::gfx::gpu_benchmark benchmark;
};

// Rendering subsystem.
struct renderer {
	// The screen rendering target.
	const tr::gfx::render_target screen;

	// Basic renderer.
	tr::gfx::renderer_2d basic;
	// Circle renderer.
	tr::gfx::circle_renderer circle;
	// Optional extra components.
	std::optional<renderer_extra> extra;
	// Blur renderer.
	blur_renderer blur;
	// Tooltip manager.
	tooltip_manager tooltip;

	// Initializes the renderer.
	renderer();

	// Gets the scale rendering is done at.
	float scale() const;

	// Sets the default transformation matrix.
	void set_default_transform(const glm::mat4& mat);

	// Adds the menu game overlay to the renderer.
	void add_menu_game_overlay();
	// Adds the fade overlay to the renderer.
	void add_fade_overlay(float opacity);

	// Draws everything added to the renderer's layers.
	void draw_layers(const tr::gfx::render_target& target);
	// Draws the cursor.
	void draw_cursor();
};
// Global renderer.
inline std::optional<renderer> g_renderer;