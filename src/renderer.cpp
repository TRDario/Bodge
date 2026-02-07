#include "../include/renderer.hpp"
#include "../include/settings.hpp"

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

renderer::renderer()
	: screen{setup_screen()}, circle{scale()}, blur{screen.size().x}, tooltip{basic}
{
	if (g_cli_settings.show_perf) {
		extra.emplace();
	}

	basic.set_default_transform(TRANSFORM);
	basic.set_default_layer_blend_mode(layer::BALL_TRAILS, tr::gfx::MAX_BLENDING);
	basic.set_default_layer_blend_mode(layer::BALL_TRAILS_OVERLAY, tr::gfx::REVERSE_ALPHA_BLENDING);
	for (int layer = layer::GAME_OVERLAY; layer <= layer::CURSOR; ++layer) {
		// Explicitly set default transform for these because the global default is modified by screenshake.
		basic.set_default_layer_transform(layer, TRANSFORM);
	}

	circle.set_default_transform(TRANSFORM);
}

//

float renderer::scale() const
{
	return screen.size().x / 1000.0f;
}

//

void renderer::set_default_transform(const glm::mat4& mat)
{
	basic.set_default_transform(mat);
	circle.set_default_transform(mat);
}

//

void renderer::add_menu_game_overlay()
{
	const tr::gfx::simple_color_mesh_ref fade_overlay{basic.new_color_fan(layer::GAME_OVERLAY, 4)};
	tr::fill_rectangle_vertices(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, MENU_GAME_OVERLAY_TINT);
}

void renderer::add_fade_overlay(float opacity)
{
	if (opacity == 0) {
		return;
	}

	const tr::gfx::simple_color_mesh_ref fade_overlay{basic.new_color_fan(layer::FADE_OVERLAY, 4)};
	tr::fill_rectangle_vertices(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, tr::rgba8{0, 0, 0, tr::norm_cast<u8>(opacity)});
}

//

void renderer::draw_layers(const tr::gfx::render_target& target)
{
	tr::gfx::draw_layer_range(layer::BALL_TRAILS, layer::FADE_OVERLAY, target, basic, circle);
}

void renderer::draw_cursor()
{
	const tr::rgba8 color{color_cast<tr::rgba8>(tr::hsv{float(g_settings.primary_hue), 1, 1})};

	tr::gfx::simple_color_mesh_ref quad{basic.new_color_fan(layer::CURSOR, 4)};
	tr::fill_rectangle_vertices(quad.positions, {{g_mouse_pos.x - 12, g_mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = basic.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{g_mouse_pos.x + 4, g_mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = basic.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{g_mouse_pos.x - 1, g_mouse_pos.y - 12}, {2, 8}});
	std::ranges::fill(quad.colors, color);
	quad = basic.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{g_mouse_pos.x - 1, g_mouse_pos.y + 4}, {2, 8}});
	std::ranges::fill(quad.colors, color);

	basic.draw(screen);
}