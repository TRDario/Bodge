#include "../include/system.hpp"
#include "../include/audio.hpp"
#include "../include/graphics.hpp"
#include "../include/settings.hpp"
#include "../include/state/state.hpp"

using namespace std::chrono_literals;

namespace engine {
	// The upper limit for an acceptable update time.
	constexpr tr::dsecs MAX_UPDATE_TIME{1.0s / SECOND_TICKS};

	// Creates a draw timer according to the active settings.
	tr::timer create_draw_timer();
	// Sets the program icon.
	void set_icon();
	// Adds the cursor crosshairs to the renderer.
	void draw_cursor();
	// Determines the upper limit for an acceptable render time.
	tr::dsecs max_render_time();

	// System state.
	struct system_data {
		// Timer that emits ticking events.
		tr::timer tick_timer{tr::system::create_tick_timer(240 * cli_settings.game_speed, 0)};
		// Timer that emits drawing events.
		tr::timer draw_timer{create_draw_timer()};
		// State manager.
		tr::state_manager state;
		// Whether the screen should be redrawn. If above 1, ticks will be paused to catch up.
		int redraw{true};
		// The held keyboard modifiers.
		tr::system::keymod held_keymods{tr::system::keymod::NONE};
		// The position of the mouse.
		glm::vec2 mouse_pos{500, 500};
		// The held mouse buttons.
		tr::system::mouse_button held_buttons{};
	};
	std::optional<system_data> system;
} // namespace engine

tr::timer engine::create_draw_timer()
{
	if (cli_settings.refresh_rate != NATIVE_REFRESH_RATE) {
		return tr::system::create_draw_timer(cli_settings.refresh_rate);
	}
	else {
		return tr::system::create_draw_timer();
	}
}

void engine::set_icon()
{
	try {
		tr::system::set_window_icon(tr::load_bitmap_file(cli_settings.data_directory / "graphics" / "icon.qoi"));
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to set window icon.");
		LOG_CONTINUE(err);
	}
}

void engine::draw_cursor()
{
	const glm::vec2 mouse_pos{engine::mouse_pos()};
	const tr::rgba8 color{color_cast<tr::rgba8>(tr::hsv{float(settings.primary_hue), 1, 1})};

	tr::gfx::simple_color_mesh_ref quad{tr::gfx::renderer_2d::new_color_fan(layer::CURSOR, 4)};
	tr::fill_rect_vtx(quad.positions, {{mouse_pos.x - 12, mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = tr::gfx::renderer_2d::new_color_fan(layer::CURSOR, 4);
	tr::fill_rect_vtx(quad.positions, {{mouse_pos.x + 4, mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = tr::gfx::renderer_2d::new_color_fan(layer::CURSOR, 4);
	tr::fill_rect_vtx(quad.positions, {{mouse_pos.x - 1, mouse_pos.y - 12}, {2, 8}});
	std::ranges::fill(quad.colors, color);
	quad = tr::gfx::renderer_2d::new_color_fan(layer::CURSOR, 4);
	tr::fill_rect_vtx(quad.positions, {{mouse_pos.x - 1, mouse_pos.y + 4}, {2, 8}});
	std::ranges::fill(quad.colors, color);

	tr::gfx::renderer_2d::draw(engine::screen());
}

tr::dsecs engine::max_render_time()
{
	if (cli_settings.refresh_rate == NATIVE_REFRESH_RATE) {
		return 1.0s / tr::system::refresh_rate();
	}
	else {
		return 1.0s / cli_settings.refresh_rate;
	}
}

bool engine::restart_required(const ::settings& old)
{
	return old.display_mode != settings.display_mode ||
		   (settings.display_mode == display_mode::WINDOWED && old.window_size != settings.window_size) || old.msaa != settings.msaa;
}

///////////////////////////////////////////////////////////////// LIFETIME ////////////////////////////////////////////////////////////////

void engine::initialize_system()
{
	const tr::gfx::properties gfx{
		.debug_context = cli_settings.debug_mode,
		.double_buffer = true,
		.multisamples = settings.msaa,
	};
	if (settings.display_mode == display_mode::FULLSCREEN) {
		tr::system::open_fullscreen_window("Bodge", tr::system::window_flag::DEFAULT, gfx);
	}
	else {
		tr::system::open_window("Bodge", glm::ivec2{settings.window_size}, tr::system::window_flag::DEFAULT, gfx);
	}
	set_icon();
	tr::system::set_window_vsync(tr::system::vsync::DISABLED);
	tr::system::show_cursor(false);
	tr::system::set_mouse_relative_mode(true);
	system.emplace();
	tr::system::flush_events();
	LOG(tr::severity::INFO, "Initialized system.");
}

void engine::set_main_menu_state()
{
	if (scorefile.name.empty()) {
		system->state.state = std::make_unique<name_entry_state>();
	}
	else {
		system->state.state = std::make_unique<title_state>();
	}
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
	tr::system::close_window();
	LOG(tr::severity::INFO, "Shut down system.");
}

////////////////////////////////////////////////////////////////// INPUT //////////////////////////////////////////////////////////////////

bool engine::active()
{
	return system->state.state != nullptr;
}

void engine::handle_events()
{
	tr::system::handle_events([&](const tr::system::event& event) {
		switch (event.type()) {
		case tr::system::tick_event::ID:
			if (system->redraw < 2) {
				system->state.update(0s);
			}
			return;
		case tr::system::draw_event::ID:
			++system->redraw;
			return;
		case tr::system::quit_event::ID:
			system->state.state.reset();
			return;
		case tr::system::window_gain_focus_event::ID:
			tr::system::show_cursor(false);
			tr::system::set_mouse_relative_mode(true);
			return;
		case tr::system::window_lose_focus_event::ID:
			tr::system::show_cursor(true);
			tr::system::set_mouse_relative_mode(false);
			return;
		case tr::system::key_down_event::ID:
			system->held_keymods = tr::system::key_down_event{event}.mods;
			break;
		case tr::system::key_up_event::ID:
			system->held_keymods = tr::system::key_up_event{event}.mods;
			break;
		case tr::system::mouse_motion_event::ID: {
			if (!tr::system::window_has_focus()) {
				return;
			}
			const glm::vec2 delta{tr::system::mouse_motion_event{event}.delta / render_scale() * tr::system::window_pixel_density()};
			system->mouse_pos = glm::clamp(system->mouse_pos + delta, 0.0f, 1000.0f);
			break;
		}
		case tr::system::mouse_down_event::ID:
			system->held_buttons |= tr::system::mouse_down_event{event}.button;
			break;
		case tr::system::mouse_up_event::ID:
			system->held_buttons &= ~tr::system::mouse_up_event{event}.button;
			break;
		default:
			break;
		}

		system->state.handle_event(event);
	});
}

void engine::redraw_if_needed()
{
	if (system->redraw) {
		system->state.draw();
		draw_cursor();
		if (tr::gfx::debug_renderer::active()) {
			tr::gfx::debug_renderer::write_right(system->state.update_benchmark(), "Update:", MAX_UPDATE_TIME);
			tr::gfx::debug_renderer::newline_right();
			tr::gfx::debug_renderer::write_right(system->state.draw_benchmark(), "Render:", max_render_time());
			tr::gfx::debug_renderer::draw();
		}
		tr::gfx::flip_backbuffer();
		tr::gfx::clear_backbuffer();
		system->redraw = false;
	}
}

tr::system::keymod engine::held_keymods()
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

tr::system::mouse_button engine::held_buttons()
{
	return system->held_buttons;
}