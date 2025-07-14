#pragma once
#include "blur_renderer.hpp"
#include "settings.hpp"
#include "tooltip.hpp"

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

// Windowing and graphics functionality.
struct engine {
	/////////////////////////////////////////////////////////////// LIFETIME //////////////////////////////////////////////////////////////

	// Initializes the engine.
	static void initialize();
	// Sets the game state to the main menu state.
	static void set_main_menu_state();
	// Applies new settings to the engine.
	static void apply_settings(const settings_t& old_settings);
	// Shuts the engine down.
	static void shut_down() noexcept;

	//////////////////////////////////////////////////////////////// INPUT ////////////////////////////////////////////////////////////////

	// Gets whether the engine is active.
	static bool active() noexcept;
	// Handles any events.
	static void handle_events();
	// Gets the held keyboard modifiers.
	static tr::keymod held_keymods() noexcept;
	// Chooses one out of 3 options based on the held keymods.
	template <class T> static T keymods_choose(T min, T mid, T max) noexcept;
	// Gets the normalized mouse position.
	static glm::vec2 mouse_pos() noexcept;
	// Sets the normalized mouse position.
	static void set_mouse_pos(glm::vec2 pos) noexcept;
	// Gets the held mouse buttons.
	static tr::mouse_button held_buttons() noexcept;

	////////////////////////////////////////////////////////////// GRAPHICS ///////////////////////////////////////////////////////////////

	// Gets the blur renderer.
	static blur_renderer& blur_renderer() noexcept;
	// Redraws the screen if needed.
	static void redraw_if_needed();
	// Gets the rendering scale factor.
	static float render_scale() noexcept;
	// Gets the screen rendering target.
	static const tr::render_target& screen() noexcept;
	// Gets the tooltip manager.
	static tooltip& tooltip() noexcept;
};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

// Adds the menu game overlay to the renderer.
void add_menu_game_overlay_to_renderer();
// Adds a fade overlay to the renderer.
void add_fade_overlay_to_renderer(float opacity);

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <class T> T engine::keymods_choose(T min, T mid, T max) noexcept
{
	if (held_keymods() & tr::keymod::CTRL) {
		return max;
	}
	else if (held_keymods() & tr::keymod::SHIFT) {
		return mid;
	}
	else {
		return min;
	}
}