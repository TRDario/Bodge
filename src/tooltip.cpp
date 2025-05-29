#include "../include/tooltip.hpp"
#include "../include/engine.hpp"
#include "../include/font_manager.hpp"

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
	engine::layered_renderer().set_layer_texture(layer::TOOLTIP, _texture);
}

void tooltip::add_to_renderer(string_view text)
{
	if (_last_text != text) {
		render(text);
	}

	constexpr float OUTLINE{2};
	constexpr float PADDING{4};
	const vec2 tl{min(engine::mouse_pos(), 1000.0f - _last_size - 2 * OUTLINE - 2 * PADDING)};

	array<clrvtx, 8> clrbuf;
	vector<u16> indices(poly_outline_idx(4));
	fill_poly_outline_idx(indices, 4, 0);
	fill_rect_outline_vtx(positions(clrbuf), {tl + OUTLINE / 2, _last_size + OUTLINE + 2 * PADDING}, OUTLINE);
	rs::fill(colors(clrbuf), rgba8{127, 127, 127, 255});
	engine::layered_renderer().add_color_mesh(layer::TOOLTIP, clrbuf, std::move(indices));
	fill_rect_vtx(positions(clrbuf), {tl + OUTLINE, _last_size + 2 * PADDING});
	rs::fill(colors(clrbuf), rgba8{0, 0, 0, 255});
	engine::layered_renderer().add_color_quad(layer::TOOLTIP, span<clrvtx, 4>{clrbuf.begin(), clrbuf.begin() + 4});

	array<tintvtx, 4> texbuf;
	fill_rect_vtx(positions(texbuf), {tl + OUTLINE / 2 + PADDING, _last_size});
	fill_rect_vtx(uvs(texbuf), {{}, _last_size * engine::render_scale() / vec2{_texture.size()}});
	rs::fill(colors(texbuf), rgba8{255, 255, 255, 255});
	engine::layered_renderer().add_tex_quad(layer::TOOLTIP, texbuf);
}