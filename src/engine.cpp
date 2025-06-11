#include "../include/engine.hpp"
#include "../include/settings.hpp"
#include "../include/state/name_entry_state.hpp"
#include "../include/state/title_state.hpp"

using namespace std::chrono_literals;

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The upper limit for an acceptable update time.
constexpr tr::dsecs MAX_UPDATE_TIME{1.0s / SECOND_TICKS};

// Blending mode used by layer::BALL_TRAILS.
inline constexpr tr::blend_mode MAX_ALPHA_BLENDING{
	tr::blend_multiplier::ONE, tr::blend_fn::MAX, tr::blend_multiplier::ONE,
	tr::blend_multiplier::ONE, tr::blend_fn::MAX, tr::blend_multiplier::ONE,
};
// Blending mode used by layer::BALL_TRAILS_OVERLAY.
inline constexpr tr::blend_mode REVERSE_ALPHA_BLENDING{
	tr::blend_multiplier::ONE_MINUS_DST_ALPHA, tr::blend_fn::ADD, tr::blend_multiplier::DST_ALPHA,
	tr::blend_multiplier::ONE_MINUS_DST_ALPHA, tr::blend_fn::MAX, tr::blend_multiplier::DST_ALPHA,
};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

// Initializes the 2D renderer.
void initialize_2d_renderer()
{
	tr::renderer_2d::initialize();
	tr::renderer_2d::set_default_transform(TRANSFORM);
	tr::renderer_2d::set_default_layer_blend_mode(layer::BALL_TRAILS, MAX_ALPHA_BLENDING);
	tr::renderer_2d::set_default_layer_blend_mode(layer::BALL_TRAILS_OVERLAY, REVERSE_ALPHA_BLENDING);
	for (int layer = layer::GAME_OVERLAY; layer <= layer::CURSOR; ++layer) {
		// Explicitly set default transform for these because the global default is modified by screenshake.
		tr::renderer_2d::set_default_layer_transform(layer, TRANSFORM);
	}
}

// Creates a draw timer according to the active settings.
tr::timer create_draw_timer()
{
	if (settings.refresh_rate != NATIVE_REFRESH_RATE) {
		return tr::create_draw_timer(settings.refresh_rate);
	}
	else {
		return tr::create_draw_timer();
	}
}

// Adds the cursor crosshairs to the renderer.
void add_cursor_to_renderer()
{
	const vec2 mouse_pos{engine::mouse_pos()};
	const rgba8 color{color_cast<rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};

	simple_color_mesh quad{tr::renderer_2d::new_color_fan(layer::CURSOR, 4)};
	fill_rect_vtx(quad.positions, {{mouse_pos.x - 12, mouse_pos.y - 1}, {8, 2}});
	rs::fill(quad.colors, color);
	quad = tr::renderer_2d::new_color_fan(layer::CURSOR, 4);
	fill_rect_vtx(quad.positions, {{mouse_pos.x + 4, mouse_pos.y - 1}, {8, 2}});
	rs::fill(quad.colors, color);
	quad = tr::renderer_2d::new_color_fan(layer::CURSOR, 4);
	fill_rect_vtx(quad.positions, {{mouse_pos.x - 1, mouse_pos.y - 12}, {2, 8}});
	rs::fill(quad.colors, color);
	quad = tr::renderer_2d::new_color_fan(layer::CURSOR, 4);
	fill_rect_vtx(quad.positions, {{mouse_pos.x - 1, mouse_pos.y + 4}, {2, 8}});
	rs::fill(quad.colors, color);
}

// Determines the upper limit for an acceptable render time.
tr::dsecs max_render_time() noexcept
{
	if (settings.refresh_rate == NATIVE_REFRESH_RATE) {
		return 1.0s / tr::refresh_rate();
	}
	else {
		return 1.0s / settings.refresh_rate;
	}
}

// Determines whether the new settings require a full engine restart.
bool restart_required(const settings_t& old_settings) noexcept
{
	return old_settings.window_size != settings.window_size || old_settings.msaa != settings.msaa;
}

// Creates the screen render target.
tr::render_target setup_screen()
{
	const glm::ivec2 size{tr::window::size()};
	if (size.x > size.y) {
		const tr::irect2 screen{{(size.x - size.y) / 2, 0}, glm::ivec2{size.y}};
		tr::window::set_mouse_bounds(screen);
		return tr::backbuffer::region_render_target(screen);
	}
	else if (size.y > size.x) {
		const tr::irect2 screen{{0, (size.y - size.x) / 2}, glm::ivec2{size.x}};
		tr::window::set_mouse_bounds(screen);
		return tr::backbuffer::region_render_target(screen);
	}
	else {
		return tr::backbuffer::render_target();
	}
}

void add_menu_game_overlay_to_renderer()
{
	const simple_color_mesh fade_overlay{tr::renderer_2d::new_color_fan(layer::GAME_OVERLAY, 4)};
	fill_rect_vtx(fade_overlay.positions, {{}, {1000, 1000}});
	rs::fill(fade_overlay.colors, MENU_GAME_OVERLAY_TINT);
}

void add_fade_overlay_to_renderer(float opacity)
{
	if (opacity == 0) {
		return;
	}

	const simple_color_mesh fade_overlay{tr::renderer_2d::new_color_fan(layer::FADE_OVERLAY, 4)};
	fill_rect_vtx(fade_overlay.positions, {{}, {1000, 1000}});
	rs::fill(fade_overlay.colors, rgba8{0, 0, 0, norm_cast<u8>(opacity)});
}

/////////////////////////////////////////////////////////////// ENGINE DATA ///////////////////////////////////////////////////////////////

// The data and components used by the game engine.
struct engine_data {
	// Timer that emits ticking events.
	tr::timer tick_timer;
	// Timer that emits drawing events.
	tr::timer draw_timer;
	// State manager.
	tr::state_manager state;
	// The screen render target.
	tr::render_target screen;
	// Renderer for drawing blurred and desaturated images.
	blur_renderer blur;
	// Tooltip manager.
	tooltip tooltip;
	// Whether the screen should be redrawn. If above 1, ticks will be paused to catch up.
	int redraw;

	// Initializes the engine data.
	engine_data();
};
// The global engine data instance.
optional<engine_data> engine_data;

engine_data::engine_data()
	: tick_timer{tr::create_tick_timer(cli_settings.game_speed * 240, 0)}
	, draw_timer{create_draw_timer()}
	, screen{setup_screen()}
	, blur{screen.size().x}
	, redraw{true}
{
}

///////////////////////////////////////////////////////////////// LIFETIME ////////////////////////////////////////////////////////////////

void engine::initialize()
{
	const tr::gfx_properties gfx{
		.debug_context = cli_settings.debug_mode,
		.double_buffer = false,
		.multisamples = settings.msaa,
	};
	if (settings.window_size == FULLSCREEN) {
		tr::window::open_fullscreen("Bodge", tr::window_flag::DEFAULT, gfx);
	}
	else {
		tr::window::open_windowed("Bodge", glm::ivec2{settings.window_size}, tr::window_flag::DEFAULT, gfx);
	}
	tr::window::set_vsync(tr::vsync::DISABLED);
	tr::mouse::show_cursor(false);
	initialize_2d_renderer();
	engine_data.emplace();

	if (cli_settings.debug_mode) {
		tr::debug_renderer::initialize(1.0f);
	}
	LOG(INFO, "Initialized the engine.");
}

void engine::set_main_menu_state()
{
	if (scorefile.name.empty()) {
		engine_data->state.state = make_unique<name_entry_state>();
	}
	else {
		engine_data->state.state = make_unique<title_state>();
	}
}

void engine::apply_settings(const settings_t& old_settings)
{
	if (restart_required(old_settings)) {
		tr::state_manager state{std::move(engine_data->state)};
		shut_down();
		initialize();
		engine_data->state = std::move(state);
	}
	else if (old_settings.refresh_rate != settings.refresh_rate) {
		engine_data->draw_timer = create_draw_timer();
	}
}

void engine::shut_down() noexcept
{
	engine_data.reset();
	tr::debug_renderer::shut_down();
	tr::renderer_2d::shut_down();
	tr::window::close();
	LOG(INFO, "Shut down the engine.");
}

////////////////////////////////////////////////////////////////// INPUT //////////////////////////////////////////////////////////////////

bool engine::active() noexcept
{
	return engine_data->state.state != nullptr;
}

void engine::handle_events()
{
	tr::event_queue::handle([&](const tr::event& event) {
		switch (event.type()) {
		case tr::tick_event::ID:
			if (engine_data->redraw < 2) {
				engine_data->state.update(0s);
			}
			break;
		case tr::draw_event::ID:
			++engine_data->redraw;
			break;
		case tr::quit_event::ID:
			engine_data->state.state.reset();
			break;
		default:
			engine_data->state.handle_event(event);
			break;
		}
	});
}

vec2 engine::mouse_pos() noexcept
{
	return to_game_coords(tr::mouse::pos());
}

vec2 engine::to_game_coords(vec2 window_coords) noexcept
{
	const glm::ivec2 window_size{tr::window::size()};
	if (window_size.x > window_size.y) {
		window_coords.x -= (window_size.x - window_size.y) / 2.0f;
		window_coords /= render_scale();
	}
	else if (window_size.y > window_size.x) {
		window_coords.y -= (window_size.y - window_size.x) / 2.0f;
		window_coords /= render_scale();
	}
	else {
		window_coords /= render_scale();
	}
	return glm::clamp(window_coords, {0, 0}, {1000, 1000});
}

//////////////////////////////////////////////////////////////// GRAPHICS /////////////////////////////////////////////////////////////////

blur_renderer& engine::blur_renderer() noexcept
{
	return engine_data->blur;
}

void engine::redraw_if_needed()
{
	if (engine_data->redraw) {
		add_cursor_to_renderer();
		engine_data->state.draw();
		if (tr::debug_renderer::active()) {
			tr::debug_renderer::write_right(engine_data->state.update_benchmark(), "Update:", MAX_UPDATE_TIME);
			tr::debug_renderer::newline_right();
			tr::debug_renderer::write_right(engine_data->state.draw_benchmark(), "Render:", max_render_time());
			tr::debug_renderer::draw();
		}
		tr::backbuffer::flip();
		tr::backbuffer::clear();
		engine_data->redraw = false;
	}
}

float engine::render_scale() noexcept
{
	return engine_data->screen.size().x / 1000.0f;
}

const tr::render_target& engine::screen() noexcept
{
	return engine_data->screen;
}

tooltip& engine::tooltip() noexcept
{
	return engine_data->tooltip;
}