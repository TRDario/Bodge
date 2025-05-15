#pragma once
#include "blur_renderer.hpp"
#include "settings.hpp"

// Standard 2D renderer layers.
enum layer {
	// Layer used to draw the cursor.
	CURSOR
};

// Windowing and graphics functionality.
struct engine {
	/////////////////////////////////////////////////////////////// LIFETIME //////////////////////////////////////////////////////////////

	// Initializes the engine.
	static void initialize();
	// Applies new settings to the engine.
	static void apply_settings(const settings_t& old_settings);
	// Shuts the engine down.
	static void shutdown() noexcept;

	//////////////////////////////////////////////////////////////// INPUT ////////////////////////////////////////////////////////////////

	// Gets whether the engine is active.
	static bool active() noexcept;
	// Handles any events.
	static void handle_events();
	// Gets the normalized mouse position.
	static glm::vec2 mouse_pos() noexcept;

	////////////////////////////////////////////////////////////// GRAPHICS ///////////////////////////////////////////////////////////////

	// Gets the blur renderer.
	static blur_renderer& blur_renderer() noexcept;
	// Gets the layered renderer.
	static tr::layered_2d_renderer& layered_renderer() noexcept;
	// Redraws the screen if needed.
	static void redraw_if_needed();
	// Gets the rendering scale factor.
	static float render_scale() noexcept;
	// Gets the screen rendering target.
	static const tr::render_target& screen() noexcept;
	// Gets the vertex scratch space buffer.
	static std::vector<tr::clrvtx2>& vertex_buffer() noexcept;
};