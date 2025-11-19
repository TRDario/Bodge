#include "../include/system.hpp"
#include "../include/audio.hpp"
#include "../include/graphics.hpp"
#include "../include/settings.hpp"
#include "../include/state/state.hpp"
#include <SDL3/SDL.h>

namespace engine {
	// Sets the program icon.
	void set_icon();
	// Adds the cursor crosshairs to the renderer.
	void draw_cursor();

	// System state.
	struct system_data {
		// State manager.
		tr::state_manager state;
		// The held keyboard modifiers.
		tr::sys::keymod held_keymods{tr::sys::keymod::NONE};
		// The position of the mouse.
		glm::vec2 mouse_pos{500, 500};
		// The held mouse buttons.
		tr::sys::mouse_button held_buttons{};
	};
	std::optional<system_data> system;
} // namespace engine

void engine::set_icon()
{
	try {
		tr::sys::set_window_icon(tr::load_bitmap_file(g_cli_settings.data_directory / "graphics" / "icon.qoi"));
	}
	catch (std::exception&) {
		return;
	}
}

void engine::draw_cursor()
{
	tr::gfx::renderer_2d& renderer{basic_renderer()};
	const glm::vec2 mouse_pos{engine::mouse_pos()};
	const tr::rgba8 color{color_cast<tr::rgba8>(tr::hsv{float(g_settings.primary_hue), 1, 1})};

	tr::gfx::simple_color_mesh_ref quad{renderer.new_color_fan(layer::CURSOR, 4)};
	tr::fill_rectangle_vertices(quad.positions, {{mouse_pos.x - 12, mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = renderer.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{mouse_pos.x + 4, mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = renderer.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{mouse_pos.x - 1, mouse_pos.y - 12}, {2, 8}});
	std::ranges::fill(quad.colors, color);
	quad = renderer.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{mouse_pos.x - 1, mouse_pos.y + 4}, {2, 8}});
	std::ranges::fill(quad.colors, color);

	renderer.draw(engine::screen());
}

bool engine::restart_required(const ::settings& old)
{
	return old.display_mode != g_settings.display_mode ||
		   (g_settings.display_mode == display_mode::WINDOWED && old.window_size != g_settings.window_size) || old.msaa != g_settings.msaa;
}

///////////////////////////////////////////////////////////////// LIFETIME ////////////////////////////////////////////////////////////////

void engine::initialize_system()
{
	const tr::gfx::properties gfx{
		.multisamples = g_settings.msaa,
	};
	if (g_settings.display_mode == display_mode::FULLSCREEN) {
		tr::sys::open_fullscreen_window("Bodge", tr::sys::NOT_RESIZABLE, gfx);
	}
	else {
		tr::sys::open_window("Bodge", glm::ivec2{g_settings.window_size}, tr::sys::NOT_RESIZABLE, gfx);
	}
	set_icon();
	tr::sys::set_window_vsync(g_settings.vsync ? tr::sys::vsync::ADAPTIVE : tr::sys::vsync::DISABLED);
	tr::sys::set_draw_frequency(g_settings.vsync ? 0 : g_cli_settings.refresh_rate);
	tr::sys::raise_window();
	system.emplace();
}

void engine::set_main_menu_state()
{
	if (g_scorefile.name.empty()) {
		system->state.state = std::make_unique<name_entry_state>();
	}
	else {
		system->state.state = std::make_unique<title_state>();
	}
	g_audio.play_song("menu", 1.0s);
}

void engine::apply_settings(const ::settings& old)
{
	if (restart_required(old)) {
		tr::state_manager temp{std::move(system->state)};
		shut_down_graphics();
		shut_down_system();
		initialize_system();
		initialize_graphics();
		system->state = std::move(temp);
	}
	apply_audio_settings();
}

void engine::shut_down_system()
{
	system.reset();
	tr::sys::close_window();
}

////////////////////////////////////////////////////////////////// INPUT //////////////////////////////////////////////////////////////////

bool engine::active()
{
	return system->state.state != nullptr;
}

void engine::handle_event(tr::sys::event& event)
{
	event | tr::match{
				[](tr::sys::tick_event) { system->state.update(0s); },
				[](tr::sys::quit_event) { system->state.state.reset(); },
				[](tr::sys::window_gain_focus_event) { tr::sys::set_mouse_relative_mode(true); },
				[](tr::sys::window_lose_focus_event) { tr::sys::set_mouse_relative_mode(false); },
				[](tr::sys::key_down_event event) { system->held_keymods = event.mods; },
				[](tr::sys::key_up_event event) { system->held_keymods = event.mods; },
				[](tr::sys::mouse_motion_event event) {
					if (!tr::sys::window_has_focus()) {
						return;
					}
					const glm::vec2 delta{event.delta / render_scale() * tr::sys::window_pixel_density()};
					system->mouse_pos = glm::clamp(system->mouse_pos + delta, 0.0f, 1000.0f);
				},
				[](tr::sys::mouse_down_event event) { system->held_buttons |= event.button; },
				[](tr::sys::mouse_up_event event) { system->held_buttons &= ~event.button; },
				[](auto) {},
			};
	system->state.handle_event(event);
}

void engine::redraw()
{
	if (g_cli_settings.show_perf) {
		gpu_benchmark().start();
	}
	system->state.draw();
	draw_cursor();
	if (g_cli_settings.show_perf) {
		tr::gfx::debug_renderer& renderer{debug_renderer()};
		renderer.write_right(system->state.update_benchmark(), "Update:", 1.0s / 1_s);
		renderer.newline_right();
		renderer.write_right(system->state.draw_benchmark(), "Render (CPU):", 1.0s / g_cli_settings.refresh_rate);
		renderer.newline_right();
		renderer.write_right(gpu_benchmark(), "Render (GPU):", 1.0s / g_cli_settings.refresh_rate);
		renderer.draw();
	}
	if (g_cli_settings.show_perf) {
		gpu_benchmark().stop();
	}
	tr::gfx::flip_backbuffer();
	tr::gfx::clear_backbuffer();
	if (g_cli_settings.show_perf) {
		gpu_benchmark().fetch();
	}
}

tr::sys::keymod engine::held_keymods()
{
	return system->held_keymods;
}

glm::vec2 engine::mouse_pos()
{
	return system->mouse_pos;
}

void engine::set_mouse_pos(glm::vec2 pos)
{
	system->mouse_pos = pos;
}

tr::sys::mouse_button engine::held_buttons()
{
	return system->held_buttons;
}