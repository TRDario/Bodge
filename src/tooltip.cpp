#include "../include/engine.hpp"
#include "../include/fonts.hpp"
#include "../include/tooltip.hpp"

void tooltip_manager::render(std::string_view text)
{
	const tr::bitmap render{fonts::render_text(text, fonts::determine_font(text), tr::ttf_style::NORMAL, 20, 0, 800)};
	last_text = text;
	last_size = fonts::text_size(text, fonts::determine_font(text), tr::ttf_style::NORMAL, 20, 0, 800);
	const glm::vec2 scaled_last_size{last_size * engine::render_scale()};

	if (texture.size().x < scaled_last_size.x || texture.size().y < scaled_last_size.y) {
		const glm::ivec2 new_size{
			static_cast<int>(std::bit_ceil(static_cast<std::uint32_t>(scaled_last_size.x))),
			static_cast<int>(std::bit_ceil(static_cast<std::uint32_t>(scaled_last_size.y))),
		};
		std::exchange(texture, tr::texture{new_size});
		texture.set_region({}, render.sub({{}, scaled_last_size}));
	}
	else {
		texture.clear({});
		texture.set_region({}, render.sub({{}, scaled_last_size}));
	}
	tr::renderer_2d::set_default_layer_texture(layer::TOOLTIP, texture);
}

void tooltip_manager::add_to_renderer(std::string_view text)
{
	if (last_text != text) {
		render(text);
	}

	constexpr float OUTLINE{2};
	constexpr float PADDING{4};
	const glm::vec2 tl{glm::min(engine::mouse_pos(), 1000.0f - last_size - 2 * OUTLINE - 2 * PADDING)};

	const tr::simple_color_mesh_ref outline{tr::renderer_2d::new_color_outline(layer::TOOLTIP, 4)};
	tr::fill_rect_outline_vtx(outline.positions, {tl + OUTLINE / 2, last_size + OUTLINE + 2 * PADDING}, OUTLINE);
	std::ranges::fill(outline.colors, "808080"_rgba8);
	const tr::simple_color_mesh_ref fill{tr::renderer_2d::new_color_fan(layer::TOOLTIP, 4)};
	tr::fill_rect_vtx(fill.positions, {tl + OUTLINE, last_size + 2 * PADDING});
	std::ranges::fill(fill.colors, "000000"_rgba8);
	const tr::simple_textured_mesh_ref uv{tr::renderer_2d::new_textured_fan(layer::TOOLTIP, 4)};
	tr::fill_rect_vtx(uv.positions, {tl + OUTLINE / 2 + PADDING, last_size});
	tr::fill_rect_vtx(uv.uvs, {{}, last_size * engine::render_scale() / glm::vec2{texture.size()}});
	std::ranges::fill(uv.tints, "FFFFFF"_rgba8);
}