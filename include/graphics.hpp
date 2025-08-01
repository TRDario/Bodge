#pragma once
#include "blur_renderer.hpp"
#include "tooltip_manager.hpp"

// Standard 2D renderer layers.
enum layer {
	// Layer used to draw ball trails.
	BALL_TRAILS,
	// Layer used for the overlay needed for drawing ball trails.
	BALL_TRAILS_OVERLAY,
	// Layer used to draw balls.
	BALLS,
	// Layer used to draw the player trail.
	PLAYER_TRAIL,
	// Layer used to draw the player.
	PLAYER,
	// Layer used to draw the border.
	BORDER,
	// Layer used to draw game overlays (lives, timer, etc.)
	GAME_OVERLAY,
	// Layer used to draw UI elements.
	UI,
	// Layer used to draw tooltips.
	TOOLTIP,
	// Layer used to draw the fade overlay.
	FADE_OVERLAY,
	// Layer used to draw the cursor.
	CURSOR
};

namespace engine {
	// Initializes the graphics.
	void initialize_graphics();
	// Shuts down the graphics.
	void shut_down_graphics();

	// Gets the blur renderer.
	blur_renderer& blur();
	// Gets the tooltip manager.
	tooltip_manager& tooltip();

	// Gets the rendering scale factor.
	float render_scale();
	// Gets the screen rendering target.
	const tr::gfx::render_target& screen();

	// Adds the menu game overlay to the renderer.
	void add_menu_game_overlay_to_renderer();
	// Adds a fade overlay to the renderer.
	void add_fade_overlay_to_renderer(float opacity);
} // namespace engine