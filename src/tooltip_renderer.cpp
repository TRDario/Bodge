#include "../include/tooltip_renderer.hpp"
#include "../include/engine.hpp"
#include "../include/font_manager.hpp"

void tooltip_renderer::render(std::string_view text)
{
	const tr::bitmap render{font_manager.render_text(text, font_manager.determine_font(text), 18 * engine::render_scale(), 0)};
	_last_text = text;
	_last_size = render.size();

	if (_texture.size().x < _last_size.x || _texture.size().y < _last_size.y) {
		std::exchange(_texture, {{std::bit_ceil<std::uint32_t>(_last_size.x), std::bit_ceil<std::uint32_t>(_last_size.y)}});
		engine::layered_renderer().set_layer_texture(layer::TOOLTIP, _texture);
		_texture.set_region({}, render);
	}
	else {
		_texture.clear({});
		_texture.set_region({}, render);
	}
}

void tooltip_renderer::draw(std::string_view text)
{
	if (_last_text != text) {
		render(text);
	}

	constexpr float OUTLINE{2};
	constexpr float PADDING{4};
	const glm::vec2 size{_last_size / engine::render_scale()};
	const glm::vec2 tl{glm::min(engine::mouse_pos(), 1000.0f - size - 2 * OUTLINE - 2 * PADDING)};

	std::array<tr::clrvtx2, 8> clrbuf;
	std::vector<std::uint16_t> indices(tr::poly_outline_idx(4));
	tr::fill_poly_outline_idx(indices, 4, 0);
	tr::fill_rect_outline_vtx(tr::positions(clrbuf), {tl + OUTLINE / 2, size + OUTLINE + 2 * PADDING}, OUTLINE);
	std::ranges::fill(tr::colors(clrbuf), tr::rgba8{127, 127, 127, 255});
	engine::layered_renderer().add_color_mesh(layer::TOOLTIP, clrbuf, std::move(indices));
	tr::fill_rect_vtx(tr::positions(clrbuf), {tl + OUTLINE, size + 2 * PADDING});
	std::ranges::fill(tr::colors(clrbuf), tr::rgba8{0, 0, 0, 255});
	engine::layered_renderer().add_color_quad(layer::TOOLTIP, std::span<tr::clrvtx2, 4>{clrbuf.begin(), clrbuf.begin() + 4});

	std::array<tr::tintvtx2, 4> texbuf;
	tr::fill_rect_vtx(tr::positions(texbuf), {tl + OUTLINE / 2 + PADDING, size});
	tr::fill_rect_vtx(tr::uvs(texbuf), {{}, _last_size / glm::vec2{_texture.size()}});
	std::ranges::fill(tr::colors(texbuf), tr::rgba8{255, 255, 255, 255});
	engine::layered_renderer().add_tex_quad(layer::TOOLTIP, texbuf);
}