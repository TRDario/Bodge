#pragma once
#include "settings.hpp"

// Font slots. Multiple may actually point to the same font.
enum class font : u8 {
	DEFAULT,
	FALLBACK,
	LANGUAGE,
	LANGUAGE_PREVIEW
};

namespace engine {
	void load_fonts();
	void set_language_font();
	void reload_language_preview_font(const ::settings& pending);
	void unload_fonts();

	font determine_font(std::string_view text, font preferred = font::LANGUAGE);
	float line_skip(font font, float size);
	glm::vec2 text_size(std::string_view text, font font, tr::system::ttf_style style, float size, float outline,
						float max_w = tr::system::UNLIMITED_WIDTH);
	usize count_lines(std::string_view text, font font, tr::system::ttf_style style, float size, float outline,
					  float max_w = tr::system::UNLIMITED_WIDTH);
	tr::bitmap render_text(std::string_view text, font font, tr::system::ttf_style style, float size, float outline,
						   float max_w = tr::system::UNLIMITED_WIDTH, tr::halign align = tr::halign::LEFT);
	tr::bitmap render_gradient_glyph(u32 glyph, font font, tr::system::ttf_style style, float size, float outline);
}; // namespace engine