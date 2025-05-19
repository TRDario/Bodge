#include "../../include/ui/non_text_widgets.hpp"
#include "../../include/engine.hpp"

color_preview_widget::color_preview_widget(glm::vec2 pos, tr::align alignment, std::uint16_t& hue_ref) noexcept
	: widget{{}, pos, alignment}, _hue_ref{hue_ref}
{
}

glm::vec2 color_preview_widget::size() const noexcept
{
	return {48, 48};
}

void color_preview_widget::add_to_renderer()
{
	const tr::rgba8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(_hue_ref), 1, 1}), tr::norm_cast<std::uint8_t>(opacity())};
	const tr::rgba8 outline_color{static_cast<std::uint8_t>(color.r / 2), static_cast<std::uint8_t>(color.g / 2),
								  static_cast<std::uint8_t>(color.b / 2), static_cast<std::uint8_t>(color.a / 2)};
	const float outline{4 * engine::render_scale()};

	std::array<tr::clrvtx2, 12> buffer;
	std::vector<std::uint16_t> indices(tr::poly_outline_idx(4) + tr::poly_idx(4));
	tr::fill_rect_outline_vtx(tr::positions(buffer).begin(), {tl(), size() - outline}, outline);
	tr::fill_rect_vtx(tr::positions(buffer).begin() + 8, {tl() + outline / 2, size() - 2 * outline});
	std::ranges::fill(std::views::take(tr::colors(buffer), 8), outline_color);
	std::ranges::fill(std::views::drop(tr::colors(buffer), 8), color);
	tr::fill_poly_outline_idx(indices.begin(), 4, 0);
	tr::fill_poly_idx(indices.begin() + tr::poly_outline_idx(4), 4, 8);
	engine::layered_renderer().add_color_mesh(layer::UI1, buffer, std::move(indices));
}