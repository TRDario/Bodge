#pragma once
#include "settings.hpp"

// Font slots. Multiple may actually point to the same font.
enum class font : u8 {
	DEFAULT,
	FALLBACK,
	LANGUAGE,
	LANGUAGE_PREVIEW
};

inline class text_engine {
  public:
	void load_fonts();
	void set_language_font();
	void reload_language_preview_font(const ::settings& pending);
	void unload_fonts();

	font determine_font(std::string_view text, font preferred = font::LANGUAGE);
	float line_skip(font font, float size);
	glm::vec2 text_size(std::string_view text, font font, text_style style, float size, float outline,
						float max_w = tr::sys::UNLIMITED_WIDTH);
	usize count_lines(std::string_view text, font font, text_style style, float size, float outline,
					  float max_w = tr::sys::UNLIMITED_WIDTH);
	tr::bitmap render_text(std::string_view text, font font, text_style style, float size, float outline,
						   float max_w = tr::sys::UNLIMITED_WIDTH, tr::halign align = tr::halign::LEFT);
	tr::bitmap render_gradient_glyph(u32 glyph, font font, text_style style, float size, float outline);

  private:
	struct standard_fonts_t {
		tr::sys::ttfont default_font;
		tr::sys::ttfont fallback_font;
	};

	enum class optional_font_state {
		USE_STORED,
		USE_DEFAULT,
		USE_FALLBACK,
		USE_LANGUAGE
	};
	struct optional_font_base {
		tr::sys::ttfont font;
		std::string name;
	};
	struct optional_font {
		alignas(optional_font_base) std::byte data[sizeof(optional_font_base)]{};
		optional_font_state state{optional_font_state::USE_DEFAULT};

		optional_font_base& operator*();
		optional_font_base* operator->();
	};

	std::mutex m_mutex;
	std::optional<standard_fonts_t> m_standard_fonts;
	optional_font m_language_font;
	optional_font m_language_preview_font;

	tr::sys::ttfont& find_font(font font);
} g_text_engine;