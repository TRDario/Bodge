#include "../include/engine.hpp"
#include "../include/font_manager.hpp"
#include "../include/tooltip.hpp"

void tooltip::render(std::string_view text)
{
	const tr::bitmap render{font_manager.render_text(text, font_manager.determine_font(text), tr::ttf_style::NORMAL, 20, 0, 800)};
	_last_text = text;
	_last_size = font_manager.text_size(text, font_manager.determine_font(text), tr::ttf_style::NORMAL, 20, 0, 800);
	const glm::vec2 scaled_last_size{_last_size * engine::render_scale()};

	if (_texture.size().x < scaled_last_size.x || _texture.size().y < scaled_last_size.y) {
		const glm::ivec2 new_size{
			static_cast<int>(std::bit_ceil(static_cast<std::uint32_t>(scaled_last_size.x))),
			static_cast<int>(std::bit_ceil(static_cast<std::uint32_t>(scaled_last_size.y))),
		};
		std::exchange(_texture, tr::texture{new_size});
		_texture.set_region({}, render.sub({{}, scaled_last_size}));
	}
	else {
		_texture.clear({});
		_texture.set_region({}, render.sub({{}, scaled_last_size}));
	}
	tr::renderer_2d::set_default_layer_texture(layer::TOOLTIP, _texture);
}

void tooltip::add_to_renderer(std::string_view text)
{
	if (_last_text != text) {
		render(text);
	}

	constexpr float OUTLINE{2};
	constexpr float PADDING{4};
	const glm::vec2 tl{glm::min(engine::mouse_pos(), 1000.0f - _last_size - 2 * OUTLINE - 2 * PADDING)};

	const tr::simple_color_mesh_ref outline{tr::renderer_2d::new_color_outline(layer::TOOLTIP, 4)};
	tr::fill_rect_outline_vtx(outline.positions, {tl + OUTLINE / 2, _last_size + OUTLINE + 2 * PADDING}, OUTLINE);
	std::ranges::fill(outline.colors, "808080"_rgba8);
	const tr::simple_color_mesh_ref fill{tr::renderer_2d::new_color_fan(layer::TOOLTIP, 4)};
	tr::fill_rect_vtx(fill.positions, {tl + OUTLINE, _last_size + 2 * PADDING});
	std::ranges::fill(fill.colors, "000000"_rgba8);
	const tr::simple_textured_mesh_ref texture{tr::renderer_2d::new_textured_fan(layer::TOOLTIP, 4)};
	tr::fill_rect_vtx(texture.positions, {tl + OUTLINE / 2 + PADDING, _last_size});
	tr::fill_rect_vtx(texture.uvs, {{}, _last_size * engine::render_scale() / glm::vec2{_texture.size()}});
	std::ranges::fill(texture.tints, "FFFFFF"_rgba8);
}