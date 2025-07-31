#include "../include/system.hpp"
#include "../include/graphics.hpp"
#include "../include/settings.hpp"
#include "../include/state/name_entry_state.hpp"
#include "../include/state/title_state.hpp"

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
	// Determines whether the new settings require a full engine restart.
	bool restart_required(const ::settings& old);

	// System state.
	struct system_data {
		// Timer that emits ticking events.
		tr::timer tick_timer{tr::create_tick_timer(240, 0)};
		// Timer that emits drawing events.
		tr::timer draw_timer{create_draw_timer()};
		// State manager.
		tr::state_manager state;
		// Whether the screen should be redrawn. If above 1, ticks will be paused to catch up.
		int redraw{true};
		// The held keyboard modifiers.
		tr::keymod held_keymods{tr::keymod::NONE};
		// The position of the mouse.
		glm::vec2 mouse_pos{500, 500};
		// The held mouse buttons.
		tr::mouse_button held_buttons{};
	};
	std::optional<system_data> system;
} // namespace engine

tr::timer engine::create_draw_timer()
{
	if (settings.refresh_rate != NATIVE_REFRESH_RATE) {
		return tr::create_draw_timer(settings.refresh_rate);
	}
	else {
		return tr::create_draw_timer();
	}
}

void engine::set_icon()
{
	try {
		tr::window::set_icon(tr::load_bitmap_file(cli_settings.datadir / "graphics" / "icon.qoi"));
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Failed to set window icon.");
		LOG_CONTINUE("{}", err.what());
	}
}

void engine::draw_cursor()
{
	const glm::vec2 mouse_pos{engine::mouse_pos()};
	const tr::rgba8 color{color_cast<tr::rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};

	tr::simple_color_mesh_ref quad{tr::renderer_2d::new_color_fan(layer::CURSOR, 4)};
	tr::fill_rect_vtx(quad.positions, {{mouse_pos.x - 12, mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = tr::renderer_2d::new_color_fan(layer::CURSOR, 4);
	tr::fill_rect_vtx(quad.positions, {{mouse_pos.x + 4, mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = tr::renderer_2d::new_color_fan(layer::CURSOR, 4);
	tr::fill_rect_vtx(quad.positions, {{mouse_pos.x - 1, mouse_pos.y - 12}, {2, 8}});
	std::ranges::fill(quad.colors, color);
	quad = tr::renderer_2d::new_color_fan(layer::CURSOR, 4);
	tr::fill_rect_vtx(quad.positions, {{mouse_pos.x - 1, mouse_pos.y + 4}, {2, 8}});
	std::ranges::fill(quad.colors, color);

	tr::renderer_2d::draw(engine::screen());
}

tr::dsecs engine::max_render_time()
{
	if (settings.refresh_rate == NATIVE_REFRESH_RATE) {
		return 1.0s / tr::refresh_rate();
	}
	else {
		return 1.0s / settings.refresh_rate;
	}
}

bool engine::restart_required(const ::settings& old)
{
	return old.window_size != settings.window_size || old.msaa != settings.msaa;
}

///////////////////////////////////////////////////////////////// LIFETIME ////////////////////////////////////////////////////////////////

void engine::initialize_system()
{
	const tr::gfx_properties gfx{
		.debug_context = cli_settings.debug_mode,
		.double_buffer = true,
		.multisamples = settings.msaa,
	};
	if (settings.window_size == FULLSCREEN) {
		tr::window::open_fullscreen("Bodge", tr::window_flag::DEFAULT, gfx);
	}
	else {
		tr::window::open_windowed("Bodge", glm::ivec2{settings.window_size}, tr::window_flag::DEFAULT, gfx);
	}
	set_icon();
	tr::window::set_vsync(tr::vsync::DISABLED);
	tr::mouse::show_cursor(false);
	tr::mouse::set_relative_mode(true);
	system.emplace();
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
	else if (old.refresh_rate != settings.refresh_rate) {
		system->draw_timer = create_draw_timer();
	}
	apply_audio_settings();
}

void engine::shut_down_system()
{
	system.reset();
	tr::window::close();
	LOG(tr::severity::INFO, "Shut down system.");
}

////////////////////////////////////////////////////////////////// INPUT //////////////////////////////////////////////////////////////////

bool engine::active()
{
	return system->state.state != nullptr;
}

void engine::handle_events()
{
	tr::event_queue::handle([&](const tr::event& event) {
		switch (event.type()) {
		case tr::tick_event::ID:
			if (system->redraw < 2) {
				system->state.update(0s);
			}
			return;
		case tr::draw_event::ID:
			++system->redraw;
			return;
		case tr::quit_event::ID:
			system->state.state.reset();
			return;
		case tr::window_gain_focus_event::ID:
			tr::mouse::show_cursor(false);
			tr::mouse::set_relative_mode(true);
			return;
		case tr::window_lose_focus_event::ID:
			tr::mouse::show_cursor(true);
			tr::mouse::set_relative_mode(false);
			return;
		case tr::key_down_event::ID:
			system->held_keymods = tr::key_down_event{event}.mods;
			break;
		case tr::key_up_event::ID:
			system->held_keymods = tr::key_up_event{event}.mods;
			break;
		case tr::mouse_motion_event::ID: {
			if (!tr::window::has_focus()) {
				return;
			}
			const glm::vec2 delta{tr::mouse_motion_event{event}.delta / render_scale() * tr::window::pixel_density()};
			system->mouse_pos = glm::clamp(system->mouse_pos + delta, 0.0f, 1000.0f);
			break;
		}
		case tr::mouse_down_event::ID:
			system->held_buttons |= tr::mouse_down_event{event}.button;
			break;
		case tr::mouse_up_event::ID:
			system->held_buttons &= ~tr::mouse_up_event{event}.button;
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
		if (tr::debug_renderer::active()) {
			tr::debug_renderer::write_right(system->state.update_benchmark(), "Update:", MAX_UPDATE_TIME);
			tr::debug_renderer::newline_right();
			tr::debug_renderer::write_right(system->state.draw_benchmark(), "Render:", max_render_time());
			tr::debug_renderer::draw();
		}
		tr::backbuffer::flip();
		tr::backbuffer::clear();
		system->redraw = false;
	}
}

tr::keymod engine::held_keymods()
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

tr::mouse_button engine::held_buttons()
{
	return system->held_buttons;
}