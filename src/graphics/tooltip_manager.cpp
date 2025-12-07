#include "../../include/graphics/tooltip_manager.hpp"
#include "../../include/fonts.hpp"
#include "../../include/graphics/graphics.hpp"

///////////////////////////////////////////////////////////// TOOLTIP MANAGER /////////////////////////////////////////////////////////////

tooltip_manager::tooltip_manager(tr::gfx::renderer_2d& basic)
{
	basic.set_default_layer_texture(layer::TOOLTIP, m_texture);
}

void tooltip_manager::render_text(std::string_view text)
{
	const tr::bitmap render{g_text_engine.render_text(text, g_text_engine.determine_font(text), tr::sys::ttf_style::NORMAL, 20, 0, 800)};
	m_last_text = text;
	m_last_size = g_text_engine.text_size(text, g_text_engine.determine_font(text), tr::sys::ttf_style::NORMAL, 20, 0, 800);
	const glm::vec2 scaled_last_size{m_last_size * g_renderer->scale()};

	if (m_texture.size().x < scaled_last_size.x || m_texture.size().y < scaled_last_size.y) {
		const glm::ivec2 new_size{int(std::bit_ceil(u32(scaled_last_size.x))), int(std::bit_ceil(u32(scaled_last_size.y)))};
		m_texture.reallocate(new_size);
	}
	m_texture.clear({});
	m_texture.set_region({}, render.sub({{}, scaled_last_size}));
}

void tooltip_manager::add(std::string_view text)
{
	if (m_last_text != text) {
		render_text(text);
	}

	constexpr float BORDER{2};
	constexpr float PADDING{4};
	const glm::vec2 tl{glm::min(g_mouse_pos, 1000.0f - m_last_size - 2 * BORDER - 2 * PADDING)};

	const tr::gfx::simple_color_mesh_ref outline{g_renderer->basic.new_color_outline(layer::TOOLTIP, 4)};
	tr::fill_rectangle_outline_vertices(outline.positions, {tl + BORDER / 2, m_last_size + BORDER + 2 * PADDING}, BORDER);
	std::ranges::fill(outline.colors, "808080"_rgba8);
	const tr::gfx::simple_color_mesh_ref fill{g_renderer->basic.new_color_fan(layer::TOOLTIP, 4)};
	tr::fill_rectangle_vertices(fill.positions, {tl + BORDER, m_last_size + 2 * PADDING});
	std::ranges::fill(fill.colors, "000000"_rgba8);
	const tr::gfx::simple_textured_mesh_ref uv{g_renderer->basic.new_textured_fan(layer::TOOLTIP, 4)};
	tr::fill_rectangle_vertices(uv.positions, {tl + BORDER / 2 + PADDING, m_last_size});
	tr::fill_rectangle_vertices(uv.uvs, {{}, m_last_size * g_renderer->scale() / glm::vec2{m_texture.size()}});
	std::ranges::fill(uv.tints, WHITE);
}