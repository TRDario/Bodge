#include "../include/fonts.hpp"
#include "../include/graphics.hpp"

namespace engine {
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

	std::optional<standard_fonts_t> standard_fonts;
	optional_font language_font;
	optional_font language_preview_font;

	tr::sys::ttfont load_font(std::string_view name);
	tr::sys::ttfont& find_font(font font);
} // namespace engine

//

tr::sys::ttfont engine::load_font(std::string_view name)
{
	try {
		std::filesystem::path path{cli_settings.data_directory / "fonts" / name};
		if (std::filesystem::is_regular_file(path)) {
			tr::sys::ttfont font{tr::sys::load_ttfont_file(path, 48)};
			return font;
		}
		path = cli_settings.user_directory / "fonts" / name;
		if (std::filesystem::is_regular_file(path)) {
			tr::sys::ttfont font{tr::sys::load_ttfont_file(path, 48)};
			return font;
		}
		throw tr::file_not_found{path.string()};
	}
	catch (tr::sys::ttfont_load_error& err) {
		tr::sys::show_fatal_error_message_box(err);
		std::abort();
	}
}

engine::optional_font_base& engine::optional_font::operator*()
{
	return (optional_font_base&)data;
}

engine::optional_font_base* engine::optional_font::operator->()
{
	return (optional_font_base*)&data;
}

tr::sys::ttfont& engine::find_font(font font)
{
	switch (font) {
	case font::DEFAULT:
		return standard_fonts->default_font;
	case font::FALLBACK:
		return standard_fonts->fallback_font;
	case font::LANGUAGE:
		switch (language_font.state) {
		case optional_font_state::USE_STORED:
			return language_font->font;
		case optional_font_state::USE_DEFAULT:
			return standard_fonts->default_font;
		case optional_font_state::USE_LANGUAGE:
		case optional_font_state::USE_FALLBACK:
			return standard_fonts->fallback_font;
		}
	case font::LANGUAGE_PREVIEW:
		switch (language_preview_font.state) {
		case optional_font_state::USE_STORED:
			return language_preview_font->font;
		case optional_font_state::USE_DEFAULT:
			return standard_fonts->default_font;
		case optional_font_state::USE_FALLBACK:
			return standard_fonts->fallback_font;
		case optional_font_state::USE_LANGUAGE:
			return find_font(font::LANGUAGE);
		}
	}
}

//

void engine::load_fonts()
{
	standard_fonts.emplace(load_font("charge_vector_b.otf"), load_font("linux_biolinum_rb.ttf"));
	try {
		std::string language_font_file{languages.contains(settings.language) ? languages.at(settings.language).font : std::string{}};
		if (language_font_file.empty() || language_font_file == "charge_vector_b.otf") {
			language_font.state = optional_font_state::USE_DEFAULT;
		}
		else if (language_font_file == "linux_biolinum_rb.ttf") {
			language_font.state = optional_font_state::USE_FALLBACK;
		}
		else {
			new (language_font.data) optional_font_base{load_font(language_font_file), std::move(language_font_file)};
			language_font.state = optional_font_state::USE_STORED;
		}
		language_preview_font.state = optional_font_state::USE_LANGUAGE;
	}
	catch (std::exception&) {
		language_font.state = optional_font_state::USE_FALLBACK;
	}
}

void engine::set_language_font()
{
	if (language_preview_font.state == optional_font_state::USE_LANGUAGE) {
		return;
	}

	if (language_font.state == optional_font_state::USE_STORED) {
		language_font->~optional_font_base();
	}
	language_font.state = language_preview_font.state;
	if (language_font.state == optional_font_state::USE_STORED) {
		new (language_font.data) optional_font_base{std::move(*language_preview_font)};
		language_preview_font->~optional_font_base();
		language_preview_font.state = optional_font_state::USE_LANGUAGE;
	}
}

void engine::reload_language_preview_font(const ::settings& pending)
{
	const bool had_value{language_preview_font.state == optional_font_state::USE_STORED};
	std::string new_font;
	try {
		new_font = languages.at(pending.language).font;
		if (new_font.empty() || new_font == "charge_vector_b.otf") {
			language_preview_font.state = optional_font_state::USE_DEFAULT;
		}
		else if (new_font == "linux_biolinum_rb.ttf") {
			language_preview_font.state = optional_font_state::USE_FALLBACK;
		}
		else if (language_font.state == optional_font_state::USE_STORED && new_font == language_font->name) {
			language_preview_font.state = optional_font_state::USE_LANGUAGE;
		}
		else {
			language_preview_font.state = optional_font_state::USE_STORED;
		}
	}
	catch (...) {
		language_preview_font.state = optional_font_state::USE_FALLBACK;
	}

	if (had_value && (language_preview_font.state != optional_font_state::USE_STORED || new_font != language_preview_font->name)) {
		language_preview_font->~optional_font_base();
		if (language_preview_font.state == optional_font_state::USE_STORED) {
			new (language_preview_font.data) optional_font_base{load_font(new_font), std::move(new_font)};
		}
	}
}

void engine::unload_fonts()
{
	standard_fonts.reset();
	if (language_font.state == optional_font_state::USE_STORED) {
		language_font->~optional_font_base();
	}
	if (language_preview_font.state == optional_font_state::USE_STORED) {
		language_preview_font->~optional_font_base();
	}
}

//

font engine::determine_font(std::string_view text, font preferred)
{
	tr::sys::ttfont& font{find_font(preferred)};
	if (std::ranges::all_of(tr::utf8::range(text), [&](tr::codepoint chr) { return chr == '\n' || font.contains(chr); })) {
		return preferred;
	}
	else {
		return font::FALLBACK;
	}
}

float engine::line_skip(font font, float size)
{
	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	return font_ref.line_skip() / render_scale();
}

glm::vec2 engine::text_size(std::string_view text, font font, tr::sys::ttf_style style, float size, float outline, float max_w)
{
	const int scaled_outline{int(outline * render_scale())};
	if (max_w != tr::sys::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * render_scale();
	}
	const int outline_max_w{max_w != tr::sys::UNLIMITED_WIDTH ? int(max_w + 2 * scaled_outline) : tr::sys::UNLIMITED_WIDTH};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	glm::ivec2 text_size{0, font_ref.text_size(text, outline_max_w).y};
	for (std::string_view line : split_into_lines(text, font_ref, outline_max_w)) {
		text_size.x = std::max(text_size.x, font_ref.measure_text(line, outline_max_w).size);
	}
	return glm::vec2{text_size} / render_scale();
}

usize engine::count_lines(std::string_view text, font font, tr::sys::ttf_style style, float size, float outline, float max_w)
{
	const int scaled_outline{int(outline * render_scale())};
	if (max_w != tr::sys::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * render_scale();
	}
	const float outline_max_w{max_w != tr::sys::UNLIMITED_WIDTH ? max_w + 2 * scaled_outline : tr::sys::UNLIMITED_WIDTH};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	return tr::sys::split_into_lines(text, font_ref, outline_max_w).size();
}

tr::bitmap engine::render_text(std::string_view text, font font, tr::sys::ttf_style style, float size, float outline, float max_w,
							   tr::halign align)
{
	const int scaled_outline{int(outline * render_scale())};
	if (max_w != tr::sys::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * render_scale();
	}
	const int outline_max_w{max_w != tr::sys::UNLIMITED_WIDTH ? int(max_w + 2 * scaled_outline) : tr::sys::UNLIMITED_WIDTH};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	tr::bitmap render{font_ref.render(text, outline_max_w, align, "80808080"_rgba8)};
	font_ref.set_outline(0);
	const tr::bitmap fill{font_ref.render(text, int(max_w), align, "FFFFFF"_rgba8)};
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - scaled_outline * 2}));
	return render;
}

tr::bitmap engine::render_gradient_glyph(u32 glyph, font font, tr::sys::ttf_style style, float size, float outline)
{
	const int scaled_outline{int(outline * render_scale())};

	tr::sys::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	tr::bitmap render{font_ref.render(glyph, "00000080"_rgba8)};
	font_ref.set_outline(0);
	tr::bitmap fill{font_ref.render(glyph, "FFFFFF"_rgba8)};
	for (tr::bitmap::mut_it it = fill.begin(); it != fill.end(); ++it) {
		const tr::rgba8 value{*it};
		u8 shade{u8(value.r / 4 + value.r * 3 / 4 * (fill.size().y - it.pos().y) / fill.size().y)};
		*it = tr::rgba8{shade, shade, shade, value.a};
	}
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - 2 * scaled_outline}));
	return render;
}