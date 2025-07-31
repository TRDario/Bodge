#include "../include/graphics.hpp"
#include "../include/settings.hpp"

namespace engine {
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

	// Creates the screen render target.
	tr::render_target setup_screen();

	// Graphics state.
	struct graphics_data {
		// The screen render target.
		tr::render_target screen{setup_screen()};
		// Renderer for drawing blurred and desaturated images.
		blur_renderer blur{screen.size().x};
		// Tooltip manager.
		tooltip_manager tooltip;
	};
	std::optional<graphics_data> graphics;

	// Initializes the 2D renderer.
	void initialize_2d_renderer();
} // namespace engine

tr::render_target engine::setup_screen()
{
	const glm::ivec2 size{tr::window::size()};
	if (size.x > size.y) {
		const tr::irect2 screen{{(size.x - size.y) / 2, 0}, glm::ivec2{size.y}};
		return tr::backbuffer::region_render_target(screen);
	}
	else if (size.y > size.x) {
		const tr::irect2 screen{{0, (size.y - size.x) / 2}, glm::ivec2{size.x}};
		return tr::backbuffer::region_render_target(screen);
	}
	else {
		return tr::backbuffer::render_target();
	}
}

void engine::initialize_2d_renderer()
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

void engine::initialize_graphics()
{
	graphics.emplace();
	initialize_2d_renderer();
	if (cli_settings.debug_mode) {
		tr::debug_renderer::initialize();
	}
	tr::window::show();
	LOG(tr::severity::INFO, "Initialized graphics.");
}

void engine::shut_down_graphics()
{
	tr::window::hide();
	tr::debug_renderer::shut_down();
	tr::renderer_2d::shut_down();
	graphics.reset();
	LOG(tr::severity::INFO, "Shut down graphics.");
}

blur_renderer& engine::blur()
{
	return graphics->blur;
}

float engine::render_scale()
{
	return graphics->screen.size().x / 1000.0f;
}

const tr::render_target& engine::screen()
{
	return graphics->screen;
}

tooltip_manager& engine::tooltip()
{
	return graphics->tooltip;
}

void engine::add_menu_game_overlay_to_renderer()
{
	const tr::simple_color_mesh_ref fade_overlay{tr::renderer_2d::new_color_fan(layer::GAME_OVERLAY, 4)};
	tr::fill_rect_vtx(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, MENU_GAME_OVERLAY_TINT);
}

void engine::add_fade_overlay_to_renderer(float opacity)
{
	if (opacity == 0) {
		return;
	}

	const tr::simple_color_mesh_ref fade_overlay{tr::renderer_2d::new_color_fan(layer::FADE_OVERLAY, 4)};
	tr::fill_rect_vtx(fade_overlay.positions, {{}, {1000, 1000}});
	std::ranges::fill(fade_overlay.colors, tr::rgba8{0, 0, 0, tr::norm_cast<std::uint8_t>(opacity)});
}