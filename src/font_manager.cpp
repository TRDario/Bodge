#include "../include/font_manager.hpp"
#include "../include/engine.hpp"

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

tr::ttfont load_font(std::string_view name)
{
	try {
		std::filesystem::path path{cli_settings.datadir / "fonts" / name};
		if (std::filesystem::is_regular_file(path)) {
			tr::ttfont font{tr::load_ttfont_file(path, 48)};
			LOG(tr::severity::INFO, "Loaded font '{}'.", name);
			LOG_CONTINUE("From: '{}'", path.string());
			return font;
		}
		path = cli_settings.userdir / "fonts" / name;
		if (std::filesystem::is_regular_file(path)) {
			tr::ttfont font{tr::load_ttfont_file(path, 48)};
			LOG(tr::severity::INFO, "Loaded font '{}'.", name);
			LOG_CONTINUE("From: '{}'", path.string());
			return font;
		}

		LOG(tr::severity::FATAL, "Failed to load font '{}'.", name);
		LOG_CONTINUE("File not found in neither data nor user directory.");
		tr::terminate("Font not found", path.string());
	}
	catch (tr::ttfont_load_error& err) {
		LOG(tr::severity::FATAL, "Failed to load font '{}'.", name);
		LOG_CONTINUE("", err.description());
		LOG_CONTINUE("", err.details());
		tr::terminate("Font loading failure", err.description(), err.details());
	}
}

font_manager_t::optional_font_base& font_manager_t::optional_font::operator*() noexcept
{
	return *reinterpret_cast<optional_font_base*>(&data);
}

font_manager_t::optional_font_base* font_manager_t::optional_font::operator->() noexcept
{
	return reinterpret_cast<optional_font_base*>(&data);
}

tr::ttfont& font_manager_t::find_font(font font) noexcept
{
	switch (font) {
	case font::DEFAULT:
		return _standard_fonts->default_font;
	case font::FALLBACK:
		return _standard_fonts->fallback_font;
	case font::LANGUAGE:
		switch (_language_font.state) {
		case optional_font_state::USE_STORED:
			return _language_font->font;
		case optional_font_state::USE_DEFAULT:
			return _standard_fonts->default_font;
		case optional_font_state::USE_LANGUAGE:
		case optional_font_state::USE_FALLBACK:
			return _standard_fonts->fallback_font;
		}
	case font::LANGUAGE_PREVIEW:
		switch (_language_preview_font.state) {
		case optional_font_state::USE_STORED:
			return _language_preview_font->font;
		case optional_font_state::USE_DEFAULT:
			return _standard_fonts->default_font;
		case optional_font_state::USE_FALLBACK:
			return _standard_fonts->fallback_font;
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

std::vector<std::string> split_overlong_lines(std::vector<std::string>&& lines, const tr::ttfont& font, float max_w)
{
	for (std::vector<std::string>::iterator it = lines.begin(); it != lines.end(); ++it) {
		if (it->empty()) {
			continue;
		}

		const tr::ttf_measure_result measure{font.measure_text(*it, static_cast<int>(max_w))};
		if (measure.text != std::string_view{*it}) {
			it = std::prev(lines.emplace(std::next(it), it->begin() + measure.text.size(), it->end()));
			it->erase(it->begin() + measure.text.size(), it->end());
		}
	}
	return std::move(lines);
}

///////////////////////////////////////////////////////////////// LOADING /////////////////////////////////////////////////////////////////

void font_manager_t::load_fonts()
{
	_standard_fonts.emplace(load_font("charge_vector_b.otf"), load_font("linux_biolinum_rb.ttf"));
	try {
		std::string language_font{languages.contains(settings.language) ? languages.at(settings.language).font : std::string{}};
		if (language_font.empty() || language_font == "charge_vector_b.otf") {
			_language_font.state = optional_font_state::USE_DEFAULT;
		}
		else if (language_font == "linux_biolinum_rb.ttf") {
			_language_font.state = optional_font_state::USE_FALLBACK;
		}
		else {
			new (_language_font.data) optional_font_base{load_font(language_font), std::move(language_font)};
			_language_font.state = optional_font_state::USE_STORED;
		}
		_language_preview_font.state = optional_font_state::USE_LANGUAGE;
	}
	catch (std::exception& err) {
		LOG(tr::severity::ERROR, "Falling back to linux_biolinum_rb.ttf.", err.what());
		_language_font.state = optional_font_state::USE_FALLBACK;
	}
}

void font_manager_t::set_language_font()
{
	if (_language_preview_font.state == optional_font_state::USE_LANGUAGE) {
		return;
	}

	if (_language_font.state == optional_font_state::USE_STORED) {
		_language_font->~optional_font_base();
	}
	_language_font.state = _language_preview_font.state;
	if (_language_font.state == optional_font_state::USE_STORED) {
		new (_language_font.data) optional_font_base{std::move(*_language_preview_font)};
		_language_preview_font->~optional_font_base();
		_language_preview_font.state = optional_font_state::USE_LANGUAGE;
	}
}

void font_manager_t::reload_language_preview_font(const settings_t& pending)
{
	const bool had_value{_language_preview_font.state == optional_font_state::USE_STORED};
	std::string new_font;
	try {
		new_font = languages.at(pending.language).font;
		if (new_font.empty() || new_font == "charge_vector_b.otf") {
			_language_preview_font.state = optional_font_state::USE_DEFAULT;
		}
		else if (new_font == "linux_biolinum_rb.ttf") {
			_language_preview_font.state = optional_font_state::USE_FALLBACK;
		}
		else if (_language_font.state == optional_font_state::USE_STORED && new_font == _language_font->name) {
			_language_preview_font.state = optional_font_state::USE_LANGUAGE;
		}
		else {
			_language_preview_font.state = optional_font_state::USE_STORED;
		}
	}
	catch (...) {
		_language_preview_font.state = optional_font_state::USE_FALLBACK;
	}

	if (had_value && (_language_preview_font.state != optional_font_state::USE_STORED || new_font != _language_preview_font->name)) {
		_language_preview_font->~optional_font_base();
		if (_language_preview_font.state == optional_font_state::USE_STORED) {
			new (_language_preview_font.data) optional_font_base{load_font(new_font), std::move(new_font)};
		}
	}
}

void font_manager_t::unload_all() noexcept
{
	_standard_fonts.reset();
	if (_language_font.state == optional_font_state::USE_STORED) {
		_language_font->~optional_font_base();
	}
	if (_language_preview_font.state == optional_font_state::USE_STORED) {
		_language_preview_font->~optional_font_base();
	}
	LOG(tr::severity::INFO, "Unloaded all fonts.");
}

/////////////////////////////////////////////////////////////// OPERATIONS ////////////////////////////////////////////////////////////////

font font_manager_t::determine_font(std::string_view text) noexcept
{
	tr::ttfont& font{find_font(font::LANGUAGE)};
	if (std::ranges::all_of(tr::utf8::range(text), [&](tr::codepoint chr) { return chr == '\n' || font.contains(chr); })) {
		return font::LANGUAGE;
	}
	else {
		return font::FALLBACK;
	}
}

float font_manager_t::font_line_skip(font font, float size)
{
	tr::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	return font_ref.line_skip() / engine::render_scale();
}

glm::vec2 font_manager_t::text_size(std::string_view text, font font, tr::ttf_style style, float size, float outline, float max_w)
{
	const int scaled_outline{static_cast<int>(outline * engine::render_scale())};
	if (max_w != tr::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * engine::render_scale();
	}
	const int outline_max_w{max_w != tr::UNLIMITED_WIDTH ? static_cast<int>(max_w + 2 * scaled_outline) : tr::UNLIMITED_WIDTH};

	tr::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	glm::ivec2 text_size{0, font_ref.text_size(text, outline_max_w).y};
	for (std::string_view line : split_into_lines(text)) {
		tr::ttf_measure_result result{font_ref.measure_text(line, outline_max_w)};
		if (result.text != line) {
			text_size.x = outline_max_w;
			break;
		}
		else {
			text_size.x = std::max(text_size.x, result.size);
		}
	}
	return static_cast<glm::vec2>(text_size) / engine::render_scale();
}

std::size_t font_manager_t::count_lines(std::string_view text, font font, tr::ttf_style style, float size, float outline, float max_w)
{
	const int scaled_outline{static_cast<int>(outline * engine::render_scale())};
	if (max_w != tr::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * engine::render_scale();
	}
	const float outline_max_w{max_w != tr::UNLIMITED_WIDTH ? max_w + 2 * scaled_outline : tr::UNLIMITED_WIDTH};

	tr::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	return split_overlong_lines(split_into_lines(text), font_ref, outline_max_w).size();
}

tr::bitmap font_manager_t::render_text(std::string_view text, font font, tr::ttf_style style, float size, float outline, float max_w,
									   tr::halign align)
{
	const int scaled_outline{static_cast<int>(outline * engine::render_scale())};
	if (max_w != tr::UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * engine::render_scale();
	}
	const int outline_max_w{max_w != tr::UNLIMITED_WIDTH ? static_cast<int>(max_w + 2 * scaled_outline) : tr::UNLIMITED_WIDTH};

	tr::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	font_ref.set_style(style);
	font_ref.set_outline(scaled_outline);
	tr::bitmap render{font_ref.render(text, outline_max_w, align, "80808080"_rgba8)};
	font_ref.set_outline(0);
	const tr::bitmap fill{font_ref.render(text, static_cast<int>(max_w), align, "FFFFFF"_rgba8)};
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - scaled_outline * 2}));
	return render;
}

tr::bitmap font_manager_t::render_gradient_glyph(std::uint32_t glyph, font font, tr::ttf_style style, float size, float outline)
{
	const int scaled_outline{static_cast<int>(outline * engine::render_scale())};

	tr::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
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