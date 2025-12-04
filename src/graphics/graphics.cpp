#include "../../include/graphics/graphics.hpp"
#include "../../include/settings.hpp"

tr::gfx::render_target setup_screen()
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

//

graphics::graphics()
	: screen{setup_screen()}, blur_renderer{screen.size().x}, tooltip_renderer{basic_renderer}
{
	if (g_cli_settings.show_perf) {
		extra.emplace();
	}

	basic_renderer.set_default_transform(TRANSFORM);
	basic_renderer.set_default_layer_blend_mode(layer::BALL_TRAILS, tr::gfx::MAX_BLENDING);
	basic_renderer.set_default_layer_blend_mode(layer::BALL_TRAILS_OVERLAY, tr::gfx::REVERSE_ALPHA_BLENDING);
	for (int layer = layer::GAME_OVERLAY; layer <= layer::CURSOR; ++layer) {
		// Explicitly set default transform for these because the global default is modified by screenshake.
		basic_renderer.set_default_layer_transform(layer, TRANSFORM);
	}
}

float graphics::render_scale() const
{
	return screen.size().x / 1000.0f;
}

//

void graphics::add_menu_game_overlay_to_renderer()
{
	const tr::gfx::simple_color_mesh_ref fade_overlay{basic_renderer.new_color_fan(layer::GAME_OVERLAY, 4)};
	tr::fill_rectangle_vertices(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, MENU_GAME_OVERLAY_TINT);
}

void graphics::add_fade_overlay_to_renderer(float opacity)
{
	if (opacity == 0) {
		return;
	}

	const tr::gfx::simple_color_mesh_ref fade_overlay{basic_renderer.new_color_fan(layer::FADE_OVERLAY, 4)};
	tr::fill_rectangle_vertices(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, tr::rgba8{0, 0, 0, tr::norm_cast<u8>(opacity)});
}

void graphics::draw_cursor()
{
	const tr::rgba8 color{color_cast<tr::rgba8>(tr::hsv{float(g_settings.primary_hue), 1, 1})};

	tr::gfx::simple_color_mesh_ref quad{basic_renderer.new_color_fan(layer::CURSOR, 4)};
	tr::fill_rectangle_vertices(quad.positions, {{g_mouse_pos.x - 12, g_mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = basic_renderer.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{g_mouse_pos.x + 4, g_mouse_pos.y - 1}, {8, 2}});
	std::ranges::fill(quad.colors, color);
	quad = basic_renderer.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{g_mouse_pos.x - 1, g_mouse_pos.y - 12}, {2, 8}});
	std::ranges::fill(quad.colors, color);
	quad = basic_renderer.new_color_fan(layer::CURSOR, 4);
	tr::fill_rectangle_vertices(quad.positions, {{g_mouse_pos.x - 1, g_mouse_pos.y + 4}, {2, 8}});
	std::ranges::fill(quad.colors, color);

	basic_renderer.draw(screen);
}