#include "../include/fonts.hpp"
#include "../include/graphics.hpp"

namespace engine {
	// Structure containing the standard fonts.
	struct standard_fonts_t {
		// The default font.
		tr::system::ttfont default_font;
		// The fallback font.
		tr::system::ttfont fallback_font;
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
		tr::system::ttfont font;
		// The name of the font.
		std::string name;
	};
	// Janky optional-esque thing for fonts.
	struct optional_font {
		// Storage for the actual font.
		alignas(optional_font_base) std::byte data[sizeof(optional_font_base)]{};
		// The state of the font object.
		optional_font_state state{optional_font_state::USE_DEFAULT};

		optional_font_base& operator*();
		optional_font_base* operator->();
	};

	// Standard fonts.
	std::optional<standard_fonts_t> standard_fonts;
	// Optional additional language-specific font.
	optional_font language_font;
	// Optional additional language name preview font.
	optional_font language_preview_font;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Loads a font.
	tr::system::ttfont load_font(std::string_view name);
	// Turns a font name into an actual font reference.
	tr::system::ttfont& find_font(font font);
} // namespace engine

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

tr::system::ttfont engine::load_font(std::string_view name)
{
	try {
		std::filesystem::path path{cli_settings.datadir / "fonts" / name};
		if (std::filesystem::is_regular_file(path)) {
			tr::system::ttfont font{tr::system::load_ttfont_file(path, 48)};
			LOG(tr::severity::INFO, "Loaded font '{}'.", name);
			LOG_CONTINUE("From: '{}'", path.string());
			return font;
		}
		path = cli_settings.userdir / "fonts" / name;
		if (std::filesystem::is_regular_file(path)) {
			tr::system::ttfont font{tr::system::load_ttfont_file(path, 48)};
			LOG(tr::severity::INFO, "Loaded font '{}'.", name);
			LOG_CONTINUE("From: '{}'", path.string());
			return font;
		}

		LOG(tr::severity::FATAL, "Failed to load font '{}'.", name);
		LOG_CONTINUE("File not found in neither data nor user directory.");
		throw tr::file_not_found{path.string()};
	}
	catch (tr::system::ttfont_load_error& err) {
		LOG(tr::severity::FATAL, "Failed to load font '{}'.", name);
		LOG_CONTINUE("", err.description());
		LOG_CONTINUE("", err.details());
		tr::system::show_fatal_error_message_box(err);
		std::abort();
	}
}

engine::optional_font_base& engine::optional_font::operator*()
{
	return *reinterpret_cast<optional_font_base*>(&data);
}

engine::optional_font_base* engine::optional_font::operator->()
{
	return reinterpret_cast<optional_font_base*>(&data);
}

tr::system::ttfont& engine::find_font(font font)
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

// Splits a string by lines.
std::vector<std::string> split_into_lines(std::string_view text)
{
	std::vector<std::string> lines;
	std::string_view::iterator start{text.begin()};
	std::string_view::iterator end{std::find(start, text.end(), '\n')};
	while (end != text.end()) {
		lines.push_back({start, end});
		start = end + 1;
		end = std::find(start, text.end(), '\n');
	}
	lines.push_back({start, end});
	return lines;
}

std::vector<std::string> split_overlong_lines(std::vector<std::string>&& lines, const tr::system::ttfont& font, float max_w)
{
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
		if (it->empty()) {
			continue;
		}

		const tr::system::ttf_measure_result measure{font.measure_text(*it, static_cast<int>(max_w))};
		if (measure.text != std::string_view{*it}) {
			it = std::prev(lines.emplace(std::next(it), it->begin() + measure.text.size(), it->end()));
			it->erase(it->begin() + measure.text.size(), it->end());
		}
	}
	return std::move(lines);
}

///////////////////////////////////////////////////////////////// LOADING /////////////////////////////////////////////////////////////////

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
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Falling back to linux_biolinum_rb.ttf.");
		LOG_CONTINUE(err);
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
	LOG(tr::severity::INFO, "Unloaded all fonts.");
}

/////////////////////////////////////////////////////////////// OPERATIONS ////////////////////////////////////////////////////////////////

font engine::determine_font(std::string_view text, font preferred)
{
	tr::system::ttfont& font{find_font(preferred)};
	if (std::ranges::all_of(tr::utf8::range(text), [&](tr::codepoint chr) { return chr == '\n' || font.contains(chr); })) {
		return preferred;
	}
	else {
		return font::FALLBACK;
	}
}

float engine::line_skip(font font, float size)
{
	tr::system::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	return font_ref.line_skip() / render_scale();
}

glm::vec2 engine::text_size(std::string_view text, font font, tr::system::ttf_style style, float size, float outline, float max_w)
{
	const int scaled_outline{static_cast<int>(outline * render_scale())};
	if (max_w != tr::system::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * render_scale();
	}
	const int outline_max_w{max_w != tr::system::UNLIMITED_WIDTH ? static_cast<int>(max_w + 2 * scaled_outline)
																 : tr::system::UNLIMITED_WIDTH};

	tr::system::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	glm::ivec2 text_size{0, font_ref.text_size(text, outline_max_w).y};
	for (std::string_view line : split_into_lines(text)) {
		tr::system::ttf_measure_result result{font_ref.measure_text(line, outline_max_w)};
		if (result.text != line) {
			text_size.x = outline_max_w;
			break;
		}
		else {
			text_size.x = std::max(text_size.x, result.size);
		}
	}
	return static_cast<glm::vec2>(text_size) / render_scale();
}

std::size_t engine::count_lines(std::string_view text, font font, tr::system::ttf_style style, float size, float outline, float max_w)
{
	const int scaled_outline{static_cast<int>(outline * render_scale())};
	if (max_w != tr::system::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * render_scale();
	}
	const float outline_max_w{max_w != tr::system::UNLIMITED_WIDTH ? max_w + 2 * scaled_outline : tr::system::UNLIMITED_WIDTH};

	tr::system::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	return split_overlong_lines(split_into_lines(text), font_ref, outline_max_w).size();
}

tr::bitmap engine::render_text(std::string_view text, font font, tr::system::ttf_style style, float size, float outline, float max_w,
							   tr::halign align)
{
	const int scaled_outline{static_cast<int>(outline * render_scale())};
	if (max_w != tr::system::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * render_scale();
	}
	const int outline_max_w{max_w != tr::system::UNLIMITED_WIDTH ? static_cast<int>(max_w + 2 * scaled_outline)
																 : tr::system::UNLIMITED_WIDTH};

	tr::system::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	tr::bitmap render{font_ref.render(text, outline_max_w, align, "80808080"_rgba8)};
	font_ref.set_outline(0);
	const tr::bitmap fill{font_ref.render(text, static_cast<int>(max_w), align, "FFFFFF"_rgba8)};
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - scaled_outline * 2}));
	return render;
}

tr::bitmap engine::render_gradient_glyph(std::uint32_t glyph, font font, tr::system::ttf_style style, float size, float outline)
{
	const int scaled_outline{static_cast<int>(outline * render_scale())};

	tr::system::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	tr::bitmap render{font_ref.render(glyph, "00000080"_rgba8)};
	font_ref.set_outline(0);
	tr::bitmap fill{font_ref.render(glyph, "FFFFFF"_rgba8)};
	for (tr::bitmap::mut_it it = fill.begin(); it != fill.end(); ++it) {
		const tr::rgba8 value{*it};
		std::uint8_t shade{static_cast<std::uint8_t>(value.r / 4 + value.r * 3 / 4 * (fill.size().y - it.pos().y) / fill.size().y)};
		*it = tr::rgba8{shade, shade, shade, value.a};
	}
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - 2 * scaled_outline}));
	return render;
}