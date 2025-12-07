#include "../../include/ui/widget.hpp"

///////////////////////////////////////////////////////////// INTERNAL HELPERS ////////////////////////////////////////////////////////////

// Loads an image into a bitmap.
static tr::bitmap load_image(std::string_view texture)
{
	try {
		return tr::load_bitmap_file(g_cli_settings.data_directory / "graphics" / TR_FMT::format("{}.qoi", texture));
	}
	catch (tr::bitmap_load_error&) {
		return tr::create_checkerboard({64, 64});
	}
}

////////////////////////////////////////////////////////////// IMAGE WIDGET ///////////////////////////////////////////////////////////////

image_widget::image_widget(tweened_position pos, tr::align alignment, ticks unhide_time, int priority, std::string_view file,
						   tr::opt_ref<u16> hue)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP}, m_texture{load_image(file)}, m_hue{hue}, m_priority{priority}
{
}

glm::vec2 image_widget::size() const
{
	return glm::vec2{std::visit([](const auto& texture) { return texture.size(); }, m_texture)} / 2.0f;
}

void image_widget::add_to_renderer()
{
	if (std::holds_alternative<tr::bitmap>(m_texture)) {
		const tr::bitmap source{tr::get<tr::bitmap>(m_texture)};
		tr::gfx::texture texture{source, true};
		texture.set_filtering(tr::gfx::min_filter::LMIPS_LINEAR, tr::gfx::mag_filter::LINEAR);
		m_texture = std::move(texture);
	}

	tr::rgba8 color{255, 255, 255, 255};
	if (m_hue.has_ref()) {
		color = tr::color_cast<tr::rgba8>(tr::hsv{float(*m_hue), 1, 1});
	}
	color.a = u8(color.a * opacity());

	const tr::gfx::texture& texture{tr::get<tr::gfx::texture>(m_texture)};
	const tr::gfx::simple_textured_mesh_ref quad{g_renderer->basic.new_textured_fan(layer::UI + m_priority, 4, texture)};
	tr::fill_rectangle_vertices(quad.positions, {tl(), size()});
	tr::fill_rectangle_vertices(quad.uvs, {{0, 0}, {1, 1}});
	std::ranges::fill(quad.tints, tr::rgba8{color});
}