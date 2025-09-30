#include "../include/graphics.hpp"
#include "../include/settings.hpp"

namespace engine {
	tr::gfx::render_target setup_screen();

	struct graphics_data {
		graphics_data();

		tr::gfx::render_target screen{setup_screen()};
		tr::gfx::renderer_2d basic;
		std::optional<tr::gfx::debug_renderer> debug;
		::blur_renderer blur{screen.size().x};
		tooltip_manager tooltip{basic};
	};
	std::optional<graphics_data> graphics;

	void initialize_2d_renderer();
} // namespace engine

tr::gfx::render_target engine::setup_screen()
{
	const glm::ivec2 size{tr::sys::window_size()};
	if (size.x > size.y) {
		const tr::irect2 screen{{(size.x - size.y) / 2, 0}, glm::ivec2{size.y}};
		return tr::gfx::backbuffer_region_render_target(screen);
	}
	else if (size.y > size.x) {
		const tr::irect2 screen{{0, (size.y - size.x) / 2}, glm::ivec2{size.x}};
		return tr::gfx::backbuffer_region_render_target(screen);
	}
	else {
		return tr::gfx::backbuffer_render_target();
	}
}

engine::graphics_data::graphics_data()
{
	basic.set_default_transform(TRANSFORM);
	basic.set_default_layer_blend_mode(layer::BALL_TRAILS, tr::gfx::MAX_BLENDING);
	basic.set_default_layer_blend_mode(layer::BALL_TRAILS_OVERLAY, tr::gfx::REVERSE_ALPHA_BLENDING);
	for (int layer = layer::GAME_OVERLAY; layer <= layer::CURSOR; ++layer) {
		// Explicitly set default transform for these because the global default is modified by screenshake.
		basic.set_default_layer_transform(layer, TRANSFORM);
	}
}

void engine::initialize_graphics()
{
	graphics.emplace();
	if (cli_settings.show_fps) {
		graphics->debug.emplace();
	}
	tr::sys::show_window();
	LOG(tr::severity::INFO, "Initialized graphics.");
}

void engine::shut_down_graphics()
{
	tr::sys::hide_window();
	graphics.reset();
	LOG(tr::severity::INFO, "Shut down graphics.");
}

tr::gfx::renderer_2d& engine::basic_renderer()
{
	return graphics->basic;
}

tr::gfx::debug_renderer& engine::debug_renderer()
{
	return *graphics->debug;
}

blur_renderer& engine::blur_renderer()
{
	return graphics->blur;
}

tooltip_manager& engine::tooltip()
{
	return graphics->tooltip;
}

float engine::render_scale()
{
	return graphics->screen.size().x / 1000.0f;
}

const tr::gfx::render_target& engine::screen()
{
	return graphics->screen;
}

void engine::add_menu_game_overlay_to_renderer()
{
	const tr::gfx::simple_color_mesh_ref fade_overlay{basic_renderer().new_color_fan(layer::GAME_OVERLAY, 4)};
	tr::fill_rectangle_vertices(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, MENU_GAME_OVERLAY_TINT);
}

void engine::add_fade_overlay_to_renderer(float opacity)
{
	if (opacity == 0) {
		return;
	}

	const tr::gfx::simple_color_mesh_ref fade_overlay{basic_renderer().new_color_fan(layer::FADE_OVERLAY, 4)};
	tr::fill_rectangle_vertices(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, tr::rgba8{0, 0, 0, tr::norm_cast<u8>(opacity)});
}