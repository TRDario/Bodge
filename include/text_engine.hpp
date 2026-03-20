///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides an interface for shaping and rendering text.                                                                                 //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "settings.hpp"

/////////////////////////////////////////////////////////////// TEXT ENGINE ///////////////////////////////////////////////////////////////

// Font slots. Multiple may actually point to the same font.
enum class font : u8 {
	// The default font (Charge Vector Bold).
	DEFAULT,
	// Fallback font used in case a wanted font is missing (Linux Biolinum).
	FALLBACK,
	// Font used for the current language.
	LANGUAGE,
	// Font used for the pending language's name in the settings.
	LANGUAGE_PREVIEW
};

// Text string passed along with its visual properties.
struct text {
	// The text string.
	std::string_view string;
	// The font of the text.
	font font;
	// The font style of the text.
	tr::sys::ttf_style style;
	// The font size of the text.
	float size;
	// The thickness of the outline of the text.
	float outline;
	// The maximum width of a line of the text or UNLIMITED_WIDTH.
	float max_width{tr::sys::UNLIMITED_WIDTH};
};

// Text engine class.
inline class text_engine {
  public:
	// Loads all required fonts.
	void load_fonts();
	// Sets the previous language preview font as the current language font.
	void set_language_font();
	// Reloads the language preview font if necessary.
	void reload_language_preview_font(const ::settings& pending);
	// Unloads all loaded fonts.
	void unload_fonts();

	// Determines an appropriate font for a string of text.
	font determine_font(std::string_view text, font preferred = font::LANGUAGE);
	// Gets the line skip for a certain size of a certain font.
	float line_skip(font font, float size);
	// Determines the size of a string of text when drawn.
	glm::vec2 text_size(const text& text);
	// Counts the number of lines in a string of text.
	usize count_lines(const text& text);
	// Renders a string of text.
	tr::bitmap render_text(const text& text, tr::halign align = tr::halign::LEFT);
	// Renders a gradient-shaded glyph.
	tr::bitmap render_gradient_glyph(u32 glyph, font font, tr::sys::ttf_style style, float size, float outline);

  private:
	// Structure holding the standard fonts.
	struct standard_fonts {
		// Corresponds to font::DEFAULT.
		tr::sys::ttfont default_font;
		// Corresponds to font::FALLBACK.
		tr::sys::ttfont fallback_font;
	};
	// Optional font data.
	struct optional_font {
		enum class state {
			USE_STORED,
			USE_DEFAULT,
			USE_FALLBACK,
			USE_LANGUAGE
		};
		// The base font data object.
		struct base {
			// The font object.
			tr::sys::ttfont font;
			// The name of the font.
			std::string name;
		};

		// Storage for storing the font data when state == state::USE_STORED.
		alignas(base) std::byte data[sizeof(base)]{};
		// The optional's state.
		state state{state::USE_DEFAULT};

		// Dereferences the optional.
		base& operator*();
		// Dereferences the optional.
		base* operator->();
	};

	// Text engine mutex.
	std::mutex m_mutex;
	// Fonts that are always loaded.
	std::optional<standard_fonts> m_standard_fonts;
	// Additional language font.
	optional_font m_language_font;
	// Additional language preview font.
	optional_font m_language_preview_font;

	// Converts a font name into an actual font reference.
	tr::sys::ttfont& find_font(font font);
} g_text_engine; // Global text engine.