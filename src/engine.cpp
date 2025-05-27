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

// Creates the layered renderer.
tr::layered_2d_renderer create_layered_renderer()
{
	tr::layered_2d_renderer renderer;
	renderer.add_color_layer(layer::BALL_TRAILS, TRANSFORM, MAX_ALPHA_BLENDING);
	renderer.add_color_layer(layer::BALL_TRAILS_OVERLAY, TRANSFORM, REVERSE_ALPHA_BLENDING);
	renderer.add_color_layer(layer::BALLS, TRANSFORM);
	renderer.add_color_layer(layer::PLAYER_TRAIL, TRANSFORM);
	renderer.add_color_layer(layer::PLAYER, TRANSFORM);
	renderer.add_color_layer(layer::BORDER, TRANSFORM);
	renderer.add_color_layer(layer::GAME_OVERLAY, TRANSFORM);
	renderer.add_color_layer(layer::UI, TRANSFORM);
	renderer.add_color_layer(layer::TOOLTIP, TRANSFORM);
	renderer.add_color_layer(layer::FADE_OVERLAY, TRANSFORM);
	renderer.add_color_layer(layer::CURSOR, TRANSFORM);
	return renderer;
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

// Draws the cursor crosshairs.
void draw_cursor()
{
	const vec2 mouse_pos{engine::mouse_pos()};

	vector<u16> indices(4 * poly_idx(4));
	fill_poly_idx(indices.begin(), 4, 0);
	fill_poly_idx(indices.begin() + poly_idx(4), 4, 4);
	fill_poly_idx(indices.begin() + poly_idx(4) * 2, 4, 8);
	fill_poly_idx(indices.begin() + poly_idx(4) * 3, 4, 12);

	array<clrvtx, 16> vtx;
	rs::fill(colors(vtx), color_cast<rgba8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1}));
	fill_rect_vtx(positions(vtx).begin(), {{mouse_pos.x - 12, mouse_pos.y - 1}, {8, 2}});
	fill_rect_vtx(positions(vtx).begin() + 4, {{mouse_pos.x + 4, mouse_pos.y - 1}, {8, 2}});
	fill_rect_vtx(positions(vtx).begin() + 8, {{mouse_pos.x - 1, mouse_pos.y - 12}, {2, 8}});
	fill_rect_vtx(positions(vtx).begin() + 12, {{mouse_pos.x - 1, mouse_pos.y + 4}, {2, 8}});
	engine::layered_renderer().add_color_mesh(layer::CURSOR, vtx, std::move(indices));
	engine::layered_renderer().draw(engine::screen());
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
	const glm::ivec2 size{window::size()};
	if (size.x > size.y) {
		const tr::irect2 screen{{(size.x - size.y) / 2, 0}, glm::ivec2{size.y}};
		window::set_mouse_bounds(screen);
		return tr::backbuffer::region_render_target(screen);
	}
	else if (size.y > size.x) {
		const tr::irect2 screen{{0, (size.y - size.x) / 2}, glm::ivec2{size.x}};
		window::set_mouse_bounds(screen);
		return tr::backbuffer::region_render_target(screen);
	}
	else {
		return tr::backbuffer::render_target();
	}
}

void add_fade_overlay_to_renderer(float opacity)
{
	if (opacity == 0) {
		return;
	}

	array<clrvtx, 4> quad;
	fill_rect_vtx(positions(quad), {{}, {1000, 1000}});
	rs::fill(colors(quad), rgba8{0, 0, 0, norm_cast<u8>(opacity)});
	engine::layered_renderer().add_color_quad(layer::FADE_OVERLAY, quad);
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
	// Layered 2D renderer used for most drawing.
	tr::layered_2d_renderer layers;
	// Batched 2D renderer mostly used for UI.
	tr::batched_2d_renderer batched;
	// Renderer for drawing blurred and desaturated images.
	blur_renderer blur;
	// Tooltip manager.
	tooltip tooltip;
	// Shared scratch space for one-off vertex data.
	vector<clrvtx> clrvtx2_buffer;
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
	, redraw{true}
	, screen{setup_screen()}
	, layers{create_layered_renderer()}
	, blur{screen.size().x}
{
}

///////////////////////////////////////////////////////////////// LIFETIME ////////////////////////////////////////////////////////////////

void engine::initialize()
{
	const tr::gfx_properties gfx{.multisamples = settings.msaa};
	if (settings.window_size == FULLSCREEN) {
		window::open_fullscreen("Bodge", tr::window_flag::DEFAULT, gfx);
	}
	else {
		window::open_windowed("Bodge", glm::ivec2{settings.window_size}, tr::window_flag::DEFAULT, gfx);
	}
	mouse::show_cursor(false);
	engine_data.emplace();

	if (cli_settings.debug_mode) {
		tr::debug_renderer::initialize();
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
		shutdown();
		initialize();
		engine_data->state = std::move(state);
	}
	else if (old_settings.refresh_rate != settings.refresh_rate) {
		engine_data->draw_timer = create_draw_timer();
	}
}

void engine::shutdown() noexcept
{
	tr::debug_renderer::shut_down();
	engine_data.reset();
	window::close();
	LOG(INFO, "Shut down the engine.");
}

////////////////////////////////////////////////////////////////// INPUT //////////////////////////////////////////////////////////////////

bool engine::active() noexcept
{
	return engine_data->state.state != nullptr;
}

void engine::handle_events()
{
	event_queue::handle([&](const tr::event& event) {
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
	return to_game_coords(mouse::pos());
}

vec2 engine::to_game_coords(vec2 window_coords) noexcept
{
	const glm::ivec2 window_size{window::size()};
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

tr::batched_2d_renderer& engine::batched_renderer() noexcept
{
	return engine_data->batched;
}

blur_renderer& engine::blur_renderer() noexcept
{
	return engine_data->blur;
}

tr::layered_2d_renderer& engine::layered_renderer() noexcept
{
	return engine_data->layers;
}

void engine::redraw_if_needed()
{
	if (engine_data->redraw) {
		tr::backbuffer::clear();
		engine_data->state.draw();
		draw_cursor();
		if (tr::debug_renderer::active()) {
			tr::debug_renderer::write_right(engine_data->state.update_benchmark(), "Update:", MAX_UPDATE_TIME);
			tr::debug_renderer::newline_right();
			tr::debug_renderer::write_right(engine_data->state.draw_benchmark(), "Render:", max_render_time());
			tr::debug_renderer::draw();
		}
		tr::backbuffer::flip();
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

vector<clrvtx>& engine::vertex_buffer() noexcept
{
	return engine_data->clrvtx2_buffer;
}