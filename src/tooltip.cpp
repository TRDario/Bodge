#include "../include/engine.hpp"
#include "../include/font_manager.hpp"
#include "../include/tooltip.hpp"

void tooltip::render(string_view text)
{
	const bitmap render{font_manager.render_text(text, font_manager.determine_font(text), NORMAL, 20, 0, 800)};
	_last_text = text;
	_last_size = font_manager.text_size(text, font_manager.determine_font(text), NORMAL, 20, 0, 800);
	const vec2 scaled_last_size{_last_size * engine::render_scale()};

	if (_texture.size().x < scaled_last_size.x || _texture.size().y < scaled_last_size.y) {
		std::exchange(_texture, {{std::bit_ceil<u32>(scaled_last_size.x), std::bit_ceil<u32>(scaled_last_size.y)}});
		_texture.set_region({}, render.sub({{}, scaled_last_size}));
	}
	else {
		_texture.clear({});
		_texture.set_region({}, render.sub({{}, scaled_last_size}));
	}
	tr::renderer_2d::set_default_layer_texture(layer::TOOLTIP, _texture);
}

void tooltip::add_to_renderer(string_view text)
{
	if (_last_text != text) {
		render(text);
	}

	constexpr float OUTLINE{2};
	constexpr float PADDING{4};
	const vec2 tl{min(engine::mouse_pos(), 1000.0f - _last_size - 2 * OUTLINE - 2 * PADDING)};

	const color_alloc outline{tr::renderer_2d::new_color_outline(layer::TOOLTIP, 4)};
	fill_rect_outline_vtx(outline.positions, {tl + OUTLINE / 2, _last_size + OUTLINE + 2 * PADDING}, OUTLINE);
	rs::fill(outline.colors, rgba8{127, 127, 127, 255});
	const color_alloc fill{tr::renderer_2d::new_color_fan(layer::TOOLTIP, 4)};
	fill_rect_vtx(fill.positions, {tl + OUTLINE, _last_size + 2 * PADDING});
	rs::fill(fill.colors, rgba8{0, 0, 0, 255});
	const tex_alloc texture{tr::renderer_2d::new_tex_fan(layer::TOOLTIP, 4)};
	fill_rect_vtx(texture.positions, {tl + OUTLINE / 2 + PADDING, _last_size});
	fill_rect_vtx(texture.uvs, {{}, _last_size * engine::render_scale() / vec2{_texture.size()}});
	rs::fill(texture.tints, rgba8{255, 255, 255, 255});
}