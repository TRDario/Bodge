///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements renderer.hpp.                                                                                                              //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/renderer.hpp"
#include "../include/input.hpp"
#include "../include/settings.hpp"
#include "../include/state.hpp"

//////////////////////////////////////////////////////////// INTERNAL HELPERS /////////////////////////////////////////////////////////////

// Sets up the render target for the screen.
static tr::gfx::render_target setup_screen()
{
	const glm::ivec2 window_size{tr::sys::window_size()};
	if (window_size.x > window_size.y) {
		const tr::irect2 screen{{(window_size.x - window_size.y) / 2, 0}, glm::ivec2{window_size.y}};
		return tr::gfx::backbuffer_region_render_target(screen);
	}
	else if (window_size.y > window_size.x) {
		const tr::irect2 screen{{0, (window_size.y - window_size.x) / 2}, glm::ivec2{window_size.x}};
		return tr::gfx::backbuffer_region_render_target(screen);
	}
	else {
		return tr::gfx::backbuffer_render_target();
	}
}

//////////////////////////////////////////////////////////////// RENDERER /////////////////////////////////////////////////////////////////

renderer::window::window(const settings& settings)
{
	const tr::gfx::properties gfx{.multisamples = tr::sys::max_msaa()};
	if (settings.display_mode == display_mode::FULLSCREEN) {
		tr::sys::open_fullscreen_window("Bodge", tr::sys::NOT_RESIZABLE, gfx);
	}
	else {
		tr::sys::open_window("Bodge", glm::ivec2{settings.window_size}, tr::sys::NOT_RESIZABLE, gfx);
	}
	tr::sys::set_window_icon(tr::load_bitmap_file(debug_settings::instance().data_directory() / "graphics" / "icon.qoi"));
	tr::sys::set_window_vsync(settings.vsync ? tr::sys::vsync::ADAPTIVE : tr::sys::vsync::DISABLED);
	tr::sys::raise_window();
}

renderer::window::~window()
{
	tr::sys::close_window();
}

//

renderer::window_specific_components::window_specific_components(const settings& settings)
	: window{settings}
	, screen{setup_screen()}
	, circle_renderer{screen.size().x / 1000.0f}
	, blur_renderer{screen.size().x}
	, tooltip_manager{basic_renderer}
{
	if (debug_settings::instance().show_performance_overlay()) {
		extra.emplace();
	}

	basic_renderer.set_default_transform(TRANSFORM);
	basic_renderer.set_default_layer_blend_mode(layer::BALL_TRAILS, tr::gfx::MAX_BLENDING);
	basic_renderer.set_default_layer_blend_mode(layer::BALL_TRAILS_OVERLAY, tr::gfx::REVERSE_ALPHA_BLENDING);
	for (int layer = layer::GAME_OVERLAY; layer <= layer::CURSOR; ++layer) {
		// Explicitly set default transform for these because the global default is modified by screenshake.
		basic_renderer.set_default_layer_transform(layer, TRANSFORM);
	}

	basic_renderer.set_default_transform(TRANSFORM);

	tr::sys::show_window();
}

renderer::window_specific_components::~window_specific_components()
{
	tr::sys::hide_window();
}

//

renderer::renderer()
	: text_engine{localization::instance().available_languages.contains(active_settings::instance()->language)
					  ? localization::instance().available_languages.at(active_settings::instance()->language).font
					  : std::string{}}
	, m_window_specific{active_settings::instance()}
{
}

renderer& renderer::instance()
{
	static renderer instance{};
	return instance;
}

//

void renderer::reopen_window(const settings& settings)
{
	close_window();
	m_window_specific.emplace(settings);
}

void renderer::close_window()
{
	m_window_specific.reset();
}

//

const tr::gfx::render_target& renderer::screen() const
{
	return m_window_specific->screen;
}

float renderer::scale() const
{
	return screen().size().x / 1000.0f;
}

//

tr::gfx::renderer_2d& renderer::basic()
{
	return m_window_specific->basic_renderer;
}

tr::gfx::circle_renderer& renderer::circle()
{
	return m_window_specific->circle_renderer;
}

blur_renderer& renderer::blur()
{
	return m_window_specific->blur_renderer;
}

//

void renderer::set_default_transform(const glm::mat4& mat)
{
	basic().set_default_transform(mat);
	circle().set_default_transform(mat);
}

//

void renderer::add_menu_game_overlay()
{
	const tr::gfx::simple_color_mesh_ref fade_overlay{basic().new_color_fan(layer::GAME_OVERLAY, 4)};
	tr::fill_rectangle_vertices(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, MENU_GAME_OVERLAY_TINT);
}

void renderer::add_fade_overlay(float opacity)
{
	if (opacity == 0) {
		return;
	}

	const tr::gfx::simple_color_mesh_ref fade_overlay{basic().new_color_fan(layer::FADE_OVERLAY, 4)};
	tr::fill_rectangle_vertices(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, tr::rgba8{0, 0, 0, tr::norm_cast<u8>(opacity)});
}

void renderer::add_tooltip(glm::vec2 tl, std::string_view text_string)
{
	m_window_specific->tooltip_manager.add(tl, text_string);
}

//

void renderer::draw_layers(const tr::gfx::render_target& target)
{
	tr::gfx::draw_layer_range(layer::BALL_TRAILS, layer::FADE_OVERLAY, target, basic(), circle());
}

void renderer::draw_cursor()
{
	const glm::vec2 mouse_pos{input::instance().mouse_pos};

	tr::rgba8 color{color_cast<tr::rgba8>(tr::hsv{float(active_settings::instance()->primary_hue), 1, 1})};
	if (g_state.get<state>().transparent_cursor()) {
		color.a = 160;
	}

	tr::gfx::simple_color_mesh_ref quad{basic().new_color_fan(layer::CURSOR, 4)};
	tr::fill_rectangle_vertices(quad.positions, {{mouse_pos.x - 12, mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = basic().new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{mouse_pos.x + 4, mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = basic().new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{mouse_pos.x - 1, mouse_pos.y - 12}, {2, 8}});
	std::ranges::fill(quad.colors, color);
	quad = basic().new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{mouse_pos.x - 1, mouse_pos.y + 4}, {2, 8}});
	std::ranges::fill(quad.colors, color);

	basic().draw(screen());
}

//

void renderer::start_benchmark()
{
	if (m_window_specific->extra.has_value()) {
		m_window_specific->extra->benchmark.start();
	}
}

void renderer::stop_benchmark()
{
	if (m_window_specific->extra.has_value()) {
		m_window_specific->extra->benchmark.stop();
	}
}

void renderer::fetch_benchmark()
{
	if (m_window_specific->extra.has_value()) {
		m_window_specific->extra->benchmark.fetch();
	}
}

void renderer::draw_benchmarks()
{
	if (m_window_specific->extra.has_value()) {
		const tr::dsecs max_render_time{1.0s / debug_settings::instance().refresh_rate()};
		m_window_specific->extra->debug.write_right(g_state.tick_benchmark(), "Tick:", 1.0s / 1_s);
		m_window_specific->extra->debug.newline_right();
		m_window_specific->extra->debug.write_right(g_state.draw_benchmark(), "Render (CPU):", max_render_time);
		m_window_specific->extra->debug.newline_right();
		m_window_specific->extra->debug.write_right(m_window_specific->extra->benchmark, "Render (GPU):", max_render_time);
		m_window_specific->extra->debug.draw();
	}
}