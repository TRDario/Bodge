#include "../../include/ui/widget.hpp"

color_preview_widget::color_preview_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, std::uint16_t& hue_ref)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP, false}, m_hue{hue_ref}
{
}

glm::vec2 color_preview_widget::size() const
{
	return {48, 48};
}

void color_preview_widget::add_to_renderer()
{
	const tr::rgba8 color{color_cast<tr::rgb8>(tr::hsv{float(m_hue), 1, 1}), tr::norm_cast<std::uint8_t>(opacity())};
	const tr::rgba8 outline_color{std::uint8_t(color.r / 2), std::uint8_t(color.g / 2), std::uint8_t(color.b / 2),
								  std::uint8_t(color.a / 2)};

	const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::UI, 4)};
	tr::fill_rect_outline_vtx(outline.positions, {tl() + 2.0f, size() - 4.0f}, 4.0f);
	std::ranges::fill(outline.colors, outline_color);
	const tr::gfx::simple_color_mesh_ref fill{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(fill.positions, {tl() + 4.0f, size() - 8.0f});
	std::ranges::fill(fill.colors, color);
}