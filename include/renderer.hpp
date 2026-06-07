///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides an interface for rendering graphics.                                                                                         //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "renderer/blur_renderer.hpp"
#include "renderer/text_engine.hpp"
#include "renderer/tooltip_manager.hpp"
#include "settings.hpp"

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

// Renderer singleton.
class renderer {
  public:
	// Gets the renderer instance.
	static renderer& instance();

	// Reopens the window according to settings.
	void reopen_window(const settings& settings);
	// Closes the window.
	void close_window();

	// Gets the screen rendering target.
	const tr::gfx::render_target& screen() const;
	// Gets the blur rendering input render target.
	tr::gfx::render_target blur_input();
	// Gets the scale rendering is done at.
	float scale() const;

	// Gets the basic renderer.
	tr::gfx::renderer_2d& basic();
	// Gets the circle renderer.
	tr::gfx::circle_renderer& circle();
	// Renderer text engine.
	text_engine text_engine;

	// Sets the default transformation matrix.
	void set_default_transform(const glm::mat4& mat);

	// Adds the menu game overlay to the renderer.
	void add_menu_game_overlay();
	// Adds the fade overlay to the renderer.
	void add_fade_overlay(float opacity);
	// Adds a tooltip to the renderer.
	void add_tooltip(glm::vec2 tl, std::string_view text_string);

	// Draws everything drawn to blur_input() with a gaussian blur effect.
	void draw_blurred(float saturation, float strength);
	// Draws everything added to the renderer's layers.
	void draw_layers(const tr::gfx::render_target& target);
	// Draws the cursor.
	void draw_cursor(float hue, glm::vec2 mouse_pos);

	// Marks the start of frame rendering.
	void start_benchmark();
	// Marks the end of frame rendering.
	void stop_benchmark();
	// Fetches a benchmark.
	void fetch_benchmark();
	// Draws tick and render benchmarks.
	void draw_benchmarks(float refresh_rate, const tr::benchmark& tick_benchmark, const tr::benchmark& draw_benchmark);

  private:
	// Wrapper over tr window functions.
	struct window {
		// Opens a window.
		window(const settings& settings);
		// Closes the window.
		~window();
	};

	// Window-specific renderer components.
	struct window_specific_components : window {
		// Extra renderer components.
		struct extra {
			// Debug renderer for displaying performance statistics.
			tr::gfx::debug_renderer debug;
			// GPU benchmark measuring drawing performance.
			tr::gfx::gpu_benchmark benchmark;
		};

		// Screen rendering target.
		const tr::gfx::render_target screen;
		// Basic renderer.
		tr::gfx::renderer_2d basic_renderer;
		// Circle renderer.
		tr::gfx::circle_renderer circle_renderer;
		// Blur renderer.
		blur_renderer blur_renderer;
		// Tooltip manager.
		tooltip_manager tooltip_manager;
		// Optional extra components.
		std::optional<extra> extra;

		// Creates window-specific components.
		window_specific_components(const settings& settings);
		// Destroys the window-specific components.
		~window_specific_components();
	};

	// Window-dependent renderer components.
	std::optional<window_specific_components> m_window_specific;

	// Initializes the renderer.
	renderer();
};