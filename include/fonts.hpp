#pragma once
#include "settings.hpp"

// Font slots. Multiple may actually point to the same font.
enum class font : std::uint8_t {
	// The default font.
	DEFAULT,
	// A fallback font with wider unicode support.
	FALLBACK,
	// Font requested by the current language.
	LANGUAGE,
	// Font requested by the language being previewed.
	LANGUAGE_PREVIEW
};

// Font functionality.
namespace fonts {
	/////////////////////////////////////////////////////////////// LOADING ///////////////////////////////////////////////////////////////

	// Loads all the fonts necessary at startup.
	void load();
	// Sets the language font to what the language preview font was.
	void set_language_font();
	// Reloads the language preview font.
	void reload_language_preview_font(const settings_t& pending);
	// Unloads all fonts.
	void unload_all();

	///////////////////////////////////////////////////////////// OPERATIONS //////////////////////////////////////////////////////////////

	// Tries to determine if a string can be drawn with the language font, or if it should be delegated to the fallback font.
	font determine_font(std::string_view text);
	// Gets the height of a line of text.
	float line_skip(font font, float size);
	// Gets the size of a text string in window coordinates.
	glm::vec2 text_size(std::string_view text, font font, tr::ttf_style style, float size, float outline,
						float max_w = tr::UNLIMITED_WIDTH);
	// Gets the number of lines in a text string.
	std::size_t count_lines(std::string_view text, font font, tr::ttf_style style, float size, float outline,
							float max_w = tr::UNLIMITED_WIDTH);
	// Renders text.
	tr::bitmap render_text(std::string_view text, font font, tr::ttf_style style, float size, float outline,
						   float max_w = tr::UNLIMITED_WIDTH, tr::halign align = tr::halign::LEFT);
	// Renders a gradient glyph.
	tr::bitmap render_gradient_glyph(std::uint32_t glyph, font font, tr::ttf_style style, float size, float outline);
}; // namespace fonts