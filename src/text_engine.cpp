#include "../include/text_engine.hpp"
#include "../include/renderer.hpp"

///////////////////////////////////////////////////////////// INTERNAL HELPERS ////////////////////////////////////////////////////////////

// Loads a font given a filename.
static tr::sys::ttfont load_font(std::string_view name)
{
	std::filesystem::path path{g_cli_settings.data_directory / "fonts" / name};
	if (std::filesystem::is_regular_file(path)) {
		tr::sys::ttfont font{tr::sys::load_ttfont_file(path, 48)};
		return font;
	}
	path = g_cli_settings.user_directory / "fonts" / name;
	if (std::filesystem::is_regular_file(path)) {
		tr::sys::ttfont font{tr::sys::load_ttfont_file(path, 48)};
		return font;
	}
	throw tr::file_not_found{path.string()};
}

/////////////////////////////////////////////////////////////// TEXT ENGINE ///////////////////////////////////////////////////////////////

text_engine::optional_font::base& text_engine::optional_font::operator*()
{
	return (base&)data;
}

text_engine::optional_font::base* text_engine::optional_font::operator->()
{
	return (base*)&data;
}

tr::sys::ttfont& text_engine::find_font(font font)
{
	switch (font) {
	case font::DEFAULT:
		return m_standard_fonts->default_font;
	case font::FALLBACK:
		return m_standard_fonts->fallback_font;
	case font::LANGUAGE:
		switch (m_language_font.state) {
		case optional_font::state::USE_STORED:
			return m_language_font->font;
		case optional_font::state::USE_DEFAULT:
			return m_standard_fonts->default_font;
		case optional_font::state::USE_LANGUAGE:
		case optional_font::state::USE_FALLBACK:
			return m_standard_fonts->fallback_font;
		}
	case font::LANGUAGE_PREVIEW:
		switch (m_language_preview_font.state) {
		case optional_font::state::USE_STORED:
			return m_language_preview_font->font;
		case optional_font::state::USE_DEFAULT:
			return m_standard_fonts->default_font;
		case optional_font::state::USE_FALLBACK:
			return m_standard_fonts->fallback_font;
		case optional_font::state::USE_LANGUAGE:
			return find_font(font::LANGUAGE);
		}
	}
}

//

void text_engine::load_fonts()
{
	m_standard_fonts.emplace(load_font("charge_vector_b.otf"), load_font("linux_biolinum_rb.ttf"));
	try {
		std::string language_font_file{g_languages.contains(g_settings.language) ? g_languages.at(g_settings.language).font
																				 : std::string{}};
		if (language_font_file.empty() || language_font_file == "charge_vector_b.otf") {
			m_language_font.state = optional_font::state::USE_DEFAULT;
		}
		else if (language_font_file == "linux_biolinum_rb.ttf") {
			m_language_font.state = optional_font::state::USE_FALLBACK;
		}
		else {
			new (m_language_font.data) optional_font::base{load_font(language_font_file), std::move(language_font_file)};
			m_language_font.state = optional_font::state::USE_STORED;
		}
		m_language_preview_font.state = optional_font::state::USE_LANGUAGE;
	}
	catch (std::exception&) {
		m_language_font.state = optional_font::state::USE_FALLBACK;
	}
}

void text_engine::set_language_font()
{
	std::lock_guard font_lock{m_mutex};

	if (m_language_preview_font.state == optional_font::state::USE_LANGUAGE) {
		return;
	}

	if (m_language_font.state == optional_font::state::USE_STORED) {
		m_language_font->~base();
	}
	m_language_font.state = m_language_preview_font.state;
	if (m_language_font.state == optional_font::state::USE_STORED) {
		new (m_language_font.data) optional_font::base{std::move(*m_language_preview_font)};
		m_language_preview_font->~base();
		m_language_preview_font.state = optional_font::state::USE_LANGUAGE;
	}
}

void text_engine::reload_language_preview_font(const ::settings& pending)
{
	std::lock_guard font_lock{m_mutex};

	const bool had_value{m_language_preview_font.state == optional_font::state::USE_STORED};
	std::string new_font;
	try {
		new_font = g_languages.at(pending.language).font;
		if (new_font.empty() || new_font == "charge_vector_b.otf") {
			m_language_preview_font.state = optional_font::state::USE_DEFAULT;
		}
		else if (new_font == "linux_biolinum_rb.ttf") {
			m_language_preview_font.state = optional_font::state::USE_FALLBACK;
		}
		else if (m_language_font.state == optional_font::state::USE_STORED && new_font == m_language_font->name) {
			m_language_preview_font.state = optional_font::state::USE_LANGUAGE;
		}
		else {
			m_language_preview_font.state = optional_font::state::USE_STORED;
		}
	}
	catch (...) {
		m_language_preview_font.state = optional_font::state::USE_FALLBACK;
	}

	if (had_value && (m_language_preview_font.state != optional_font::state::USE_STORED || new_font != m_language_preview_font->name)) {
		m_language_preview_font->~base();
		if (m_language_preview_font.state == optional_font::state::USE_STORED) {
			new (m_language_preview_font.data) optional_font::base{load_font(new_font), std::move(new_font)};
		}
	}
}

void text_engine::unload_fonts()
{
	std::lock_guard font_lock{m_mutex};

	m_standard_fonts.reset();
	if (m_language_font.state == optional_font::state::USE_STORED) {
		m_language_font->~base();
	}
	if (m_language_preview_font.state == optional_font::state::USE_STORED) {
		m_language_preview_font->~base();
	}
}

//

font text_engine::determine_font(std::string_view text, font preferred)
{
	std::lock_guard font_lock{m_mutex};

	tr::sys::ttfont& font{find_font(preferred)};
	if (std::ranges::all_of(tr::utf8::range(text), [&](tr::codepoint chr) { return chr == '\n' || font.contains(chr); })) {
		return preferred;
	}
	else {
		return font::FALLBACK;
	}
}

float text_engine::line_skip(font font, float size)
{
	std::lock_guard font_lock{m_mutex};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * g_renderer->scale());
	return font_ref.line_skip() / g_renderer->scale();
}

glm::vec2 text_engine::text_size(std::string_view text, font font, tr::sys::ttf_style style, float size, float outline, float max_w)
{
	std::lock_guard font_lock{m_mutex};

	const int scaled_outline{int(outline * g_renderer->scale())};
	if (max_w != tr::sys::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * g_renderer->scale();
	}
	const int outline_max_w{max_w != tr::sys::UNLIMITED_WIDTH ? int(max_w + 2 * scaled_outline) : tr::sys::UNLIMITED_WIDTH};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * g_renderer->scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	glm::ivec2 text_size{0, font_ref.text_size(text, outline_max_w).y};
	for (std::string_view line : split_into_lines(text, font_ref, outline_max_w)) {
		text_size.x = std::max(text_size.x, font_ref.measure_text(line, outline_max_w).size);
	}
	return glm::vec2{text_size} / g_renderer->scale();
}

usize text_engine::count_lines(std::string_view text, font font, tr::sys::ttf_style style, float size, float outline, float max_w)
{
	std::lock_guard font_lock{m_mutex};

	const int scaled_outline{int(outline * g_renderer->scale())};
	if (max_w != tr::sys::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * g_renderer->scale();
	}
	const float outline_max_w{max_w != tr::sys::UNLIMITED_WIDTH ? max_w + 2 * scaled_outline : tr::sys::UNLIMITED_WIDTH};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * g_renderer->scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	return tr::sys::split_into_lines(text, font_ref, outline_max_w).size();
}

tr::bitmap text_engine::render_text(std::string_view text, font font, tr::sys::ttf_style style, float size, float outline, float max_w,
									tr::halign align)
{
	std::lock_guard font_lock{m_mutex};

	const int scaled_outline{int(outline * g_renderer->scale())};
	if (max_w != tr::sys::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * g_renderer->scale();
	}
	const int outline_max_w{max_w != tr::sys::UNLIMITED_WIDTH ? int(max_w + 2 * scaled_outline) : tr::sys::UNLIMITED_WIDTH};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * g_renderer->scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	tr::bitmap render{font_ref.render(text, outline_max_w, align, DARK_GRAY)};
	font_ref.set_outline(0);
	const tr::bitmap fill{font_ref.render(text, int(max_w), align, WHITE)};
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - scaled_outline * 2}));
	return render;
}

tr::bitmap text_engine::render_gradient_glyph(u32 glyph, font font, tr::sys::ttf_style style, float size, float outline)
{
	std::lock_guard font_lock{m_mutex};

	const int scaled_outline{int(outline * g_renderer->scale())};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * g_renderer->scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	tr::bitmap render{font_ref.render(glyph, "00000080"_rgba8)};
	font_ref.set_outline(0);
	tr::bitmap fill{font_ref.render(glyph, WHITE)};
	for (tr::bitmap::mut_it pixel_it = fill.begin(); pixel_it != fill.end(); ++pixel_it) {
		const tr::rgba8 value{*pixel_it};
		u8 shade{u8(value.r / 4 + value.r * 3 / 4 * (fill.size().y - pixel_it.pos().y) / fill.size().y)};
		*pixel_it = tr::rgba8{shade, shade, shade, value.a};
	}
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - 2 * scaled_outline}));
	return render;
}