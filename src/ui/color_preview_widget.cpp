#include "../../include/ui/widget.hpp"

color_preview_widget::color_preview_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, u16& hue_ref)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP, false}, m_hue{hue_ref}
{
}

glm::vec2 color_preview_widget::size() const
{
	return {48, 48};
}

void color_preview_widget::add_to_renderer()
{
	const tr::rgba8 color{color_cast<tr::rgb8>(tr::hsv{float(m_hue), 1, 1}), tr::norm_cast<u8>(opacity())};
	const tr::rgba8 outline_color{u8(color.r / 2), u8(color.g / 2), u8(color.b / 2), u8(color.a / 2)};

	const tr::gfx::simple_color_mesh_ref outline{g_graphics->basic_renderer.new_color_outline(layer::UI, 4)};
	tr::fill_rectangle_outline_vertices(outline.positions, {tl() + 2.0f, size() - 4.0f}, 4.0f);
	std::ranges::fill(outline.colors, outline_color);
	const tr::gfx::simple_color_mesh_ref fill{g_graphics->basic_renderer.new_color_fan(layer::UI, 4)};
	tr::fill_rectangle_vertices(fill.positions, {tl() + 4.0f, size() - 8.0f});
	std::ranges::fill(fill.colors, color);
}