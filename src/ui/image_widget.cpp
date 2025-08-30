#include "../../include/ui/widget.hpp"

//

namespace engine {
	tr::bitmap load_image(std::string_view texture);
}

tr::bitmap engine::load_image(std::string_view texture)
{
	try {
		const std::filesystem::path path{engine::cli_settings.data_directory / "graphics" / TR_FMT::format("{}.qoi", texture)};
		tr::bitmap image{tr::load_bitmap_file(path)};
		LOG(tr::severity::INFO, "Loaded texture '{}'.", texture);
		LOG_CONTINUE("From: '{}'", path.string());
		return image;
	}
	catch (tr::bitmap_load_error& err) {
		LOG(tr::severity::ERROR, "Failed to load texture '{}'.", texture);
		LOG_CONTINUE("{}", err.description());
		LOG_CONTINUE("{}", err.details());
		return tr::create_checkerboard({64, 64});
	}
}

//

image_widget::image_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, int priority, std::string_view file,
						   tr::opt_ref<u16> hue)
	: widget{pos, alignment, unhide_time, NO_TOOLTIP, false}, m_texture{engine::load_image(file), true}, m_hue{hue}, m_priority{priority}
{
	m_texture.set_filtering(tr::gfx::min_filter::LMIPS_LINEAR, tr::gfx::mag_filter::LINEAR);
}

glm::vec2 image_widget::size() const
{
	return glm::vec2{m_texture.size()} / 2.0f;
}

void image_widget::add_to_renderer()
{
	tr::rgba8 color{255, 255, 255, 255};
	if (m_hue.has_ref()) {
		color = tr::color_cast<tr::rgba8>(tr::hsv{float(*m_hue), 1, 1});
	}
	color.a = u8(color.a * opacity());

	const tr::gfx::simple_textured_mesh_ref quad{tr::gfx::renderer_2d::new_textured_fan(layer::UI + m_priority, 4, m_texture)};
	tr::fill_rect_vtx(quad.positions, {tl(), size()});
	tr::fill_rect_vtx(quad.uvs, {{0, 0}, {1, 1}});
	std::ranges::fill(quad.tints, tr::rgba8{color});
}