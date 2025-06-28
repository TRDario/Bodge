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

// Font manager class.
class font_manager_t {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates the font manager.
	font_manager_t() noexcept = default;

	/////////////////////////////////////////////////////////////// LOADING ///////////////////////////////////////////////////////////////

	// Loads all the fonts necessary at startup.
	void load_fonts();
	// Sets the language font to what the language preview font was.
	void set_language_font();
	// Reloads the language preview font.
	void reload_language_preview_font(const settings_t& pending);
	// Unloads all fonts.
	void unload_all() noexcept;

	///////////////////////////////////////////////////////////// OPERATIONS //////////////////////////////////////////////////////////////

	// Tries to determine if a string can be drawn with the language font, or if it should be delegated to the fallback font.
	font determine_font(std::string_view text) noexcept;
	// Gets the height of a line of text.
	float font_line_skip(font font, float size);
	// Gets the size of a text string in window coordinates.
	glm::vec2 text_size(std::string_view text, font font, tr::ttf_style style, float size, float outline,
						float max_w = tr::UNLIMITED_WIDTH);
	// Gets the number of lines in a text string.
	int count_lines(std::string_view text, font font, tr::ttf_style style, float size, float outline, float max_w = tr::UNLIMITED_WIDTH);
	// Renders text.
	tr::bitmap render_text(std::string_view text, font font, tr::ttf_style style, float size, float outline,
						   float max_w = tr::UNLIMITED_WIDTH, tr::halign align = tr::halign::LEFT);
	// Renders gradient text.
	tr::bitmap render_gradient_text(std::string_view text, font font, tr::ttf_style style, float size, float outline,
									float max_w = tr::UNLIMITED_WIDTH, tr::halign align = tr::halign::LEFT);

  private:
	// Structure containing the standard fonts.
	struct standard_fonts {
		// The default font.
		tr::ttfont default_font;
		// The fallback font.
		tr::ttfont fallback_font;
	};

	enum class optional_font_state {
		USE_STORED,
		USE_DEFAULT,
		USE_FALLBACK,
		USE_LANGUAGE
	};
	// Custom font used by a specific language.
	struct optional_font_base {
		// The actual font.
		tr::ttfont font;
		// The name of the font.
		std::string name;
	};
	// Janky optional-esque thing for fonts.
	struct optional_font {
		// Storage for the actual font.
		alignas(optional_font_base) std::byte data[sizeof(optional_font_base)];
		// The state of the font object.
		optional_font_state state{optional_font_state::USE_DEFAULT};

		optional_font_base& operator*() noexcept;
		optional_font_base* operator->() noexcept;
	};

	// Standard fonts.
	std::optional<standard_fonts> _standard_fonts;
	// Optional additional language-specific font.
	optional_font _language_font;
	// Optional additional language name preview font.
	optional_font _language_preview_font;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Turns a font name into an actual font reference.
	tr::ttfont& find_font(font font) noexcept;
};
// The global font manager.
inline font_manager_t font_manager;