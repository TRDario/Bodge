///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements player_skin_preview_widget from ui/widget.hpp. //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////// PLAYER SKIN PREVIEW WIDGET ///////////////////////////////////////////////////////

player_skin_preview_widget::player_skin_preview_widget(tweened_position pos, tr::align alignment, ticks unhide_time,
													   settings& pending_settings)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP}, m_pending_settings{pending_settings}, m_rotation{}
{
	update_skin();
}

glm::vec2 player_skin_preview_widget::size() const
{
	return {48, 48};
}

//

void player_skin_preview_widget::update_skin()
{
	if (m_pending_settings.player_skin.empty()) {
		m_texture.emplace<no_skin>();
		return;
	}

	try {
		m_texture.emplace<tr::bitmap>(tr::load_bitmap_file(g_cli_settings.user_directory / "skins" / m_pending_settings.player_skin));
	}
	catch (...) {
		m_texture.emplace<unavailable_skin>();
	}
}

//

void player_skin_preview_widget::tick()
{
	widget::tick();
	m_rotation += 0.25_turns / SECOND_TICKS;
}

void player_skin_preview_widget::add_to_renderer()
{
	if (std::holds_alternative<tr::bitmap>(m_texture)) {
		const tr::bitmap source{tr::get<tr::bitmap>(m_texture)};
		tr::gfx::texture texture{source, true};
		texture.set_filtering(tr::gfx::min_filter::LMIPS_LINEAR, tr::gfx::mag_filter::LINEAR);
		m_texture = std::move(texture);
	}

	if (std::holds_alternative<tr::gfx::texture>(m_texture)) {
		const tr::gfx::simple_textured_mesh_ref skin{
			g_renderer->basic.new_textured_fan(layer::UI, 4, std::get<tr::gfx::texture>(m_texture))};
		tr::fill_rectangle_vertices(skin.positions.begin(), tl() + 24.0f, glm::vec2{24}, glm::vec2{48}, m_rotation);
		tr::fill_rectangle_vertices(skin.uvs.begin(), {{0, 0}, {1, 1}});
		std::ranges::fill(skin.tints, tr::rgba8{255, 255, 255, tr::norm_cast<u8>(opacity())});
	}
	else if (std::holds_alternative<no_skin>(m_texture)) {
		const tr::circle circle{tl() + 24.0f, 24.0f};
		tr::rgba8 color{tr::color_cast<tr::rgba8>(tr::hsv{float(m_pending_settings.primary_hue), 1, 1})};
		color.a = tr::norm_cast<u8>(opacity());

		const tr::gfx::simple_color_mesh_ref fill{g_renderer->basic.new_color_fan(layer::UI, 6)};
		tr::fill_regular_polygon_vertices(fill.positions, circle, m_rotation);
		std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, color.a});
		const tr::gfx::simple_color_mesh_ref outline{g_renderer->basic.new_color_outline(layer::UI, 6)};
		tr::fill_regular_polygon_outline_vertices(outline.positions, circle, m_rotation, 4.0f);
		std::fill_n(outline.colors.begin(), 6, color);
		std::fill_n(outline.colors.begin() + 6, 6, tr::rgba8{0, 0, 0, color.a});
	}
}