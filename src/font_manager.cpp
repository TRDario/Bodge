#include "../include/font_manager.hpp"
#include "../include/engine.hpp"

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

ttfont load_font(string_view name)
{
	try {
		path path{cli_settings.datadir / "fonts" / name};
		if (is_regular_file(path)) {
			ttfont font{tr::load_ttfont_file(path, 48)};
			LOG(INFO, "Loaded font '{}'.", name);
			return font;
		}
		path = cli_settings.userdir / "fonts" / name;
		if (is_regular_file(path)) {
			ttfont font{tr::load_ttfont_file(path, 48)};
			LOG(INFO, "Loaded font '{}'.", name);
			return font;
		}
		LOG(ERROR, "Failed to load font '{}': File not found.", name);
		throw std::runtime_error{format("Failed to load font '{}': File not found", name)};
	}
	catch (tr::ttfont_file_load_error& err) {
		LOG(ERROR, "Failed to load font '{}': {}.", name, err.what());
		throw std::runtime_error{format("Failed to load font '{}': {}", name, err.what())};
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

ttfont& font_manager_t::find_font(font font) noexcept
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
vector<string> split_into_lines(string_view text)
{
	vector<string> lines;
	string_view::iterator start{text.begin()};
	string_view::iterator end{std::find(start, text.end(), '\n')};
	while (end != text.end()) {
		lines.push_back({start, end});
		start = end + 1;
		end = std::find(start, text.end(), '\n');
	}
	lines.push_back({start, end});
	return lines;
}

///////////////////////////////////////////////////////////////// LOADING /////////////////////////////////////////////////////////////////

void font_manager_t::load_fonts()
{
	_standard_fonts.emplace(load_font("charge_vector_b.otf"), load_font("linux_biolinum_rb.ttf"));
	try {
		string language_font{languages.contains(settings.language) ? languages.at(settings.language).font : string{}};
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
		LOG(ERROR, "Falling back to linux_biolinum_rb.ttf.", err.what());
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
	string new_font;
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
	LOG(INFO, "Unloaded all fonts.");
}

/////////////////////////////////////////////////////////////// OPERATIONS ////////////////////////////////////////////////////////////////

font font_manager_t::determine_font(string_view text) noexcept
{
	ttfont& font{find_font(font::LANGUAGE)};
	if (rs::all_of(tr::utf8::range(text), [&](tr::codepoint chr) { return chr == '\n' || font.contains(chr); })) {
		return font::LANGUAGE;
	}
	else {
		return font::FALLBACK;
	}
}

vec2 font_manager_t::text_size(string_view text, font font, ttf_style style, float size, float outline, float max_w)
{
	outline = max(static_cast<int>(outline * engine::render_scale()), 1);
	if (max_w != UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * engine::render_scale();
	}

	ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	glm::ivec2 text_size{0, font_ref.text_size(text, max_w, style, outline).y};
	for (string_view line : split_into_lines(text)) {
		tr::ttf_measure_result result{font_ref.measure_text(line, max_w, style, outline)};
		if (result.text != line) {
			text_size.x = max_w;
			break;
		}
		else {
			text_size.x = max(text_size.x, result.size);
		}
	}
	return static_cast<vec2>(text_size) / engine::render_scale();
}

bitmap font_manager_t::render_text(string_view text, font font, ttf_style style, float size, float outline, float max_w, tr::halign align)
{
	const int scaled_outline{static_cast<int>(outline * engine::render_scale())};
	if (max_w != UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * engine::render_scale();
	}
	const float outline_max_w{max_w != UNLIMITED_WIDTH ? max_w + 2 * scaled_outline : UNLIMITED_WIDTH};

	ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	bitmap render{font_ref.draw(text, outline_max_w, align, {127, 127, 127, 127}, style, scaled_outline)};
	const bitmap fill{font_ref.draw(text, max_w, align, {255, 255, 255, 255}, style)};
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - scaled_outline * 2}));
	return render;
}

bitmap font_manager_t::render_gradient_text(string_view text, font font, ttf_style style, float size, float outline, float max_w,
											tr::halign align)
{
	const int scaled_outline{static_cast<int>(outline * engine::render_scale())};
	if (max_w != UNLIMITED_WIDTH) {
		max_w = (max_w - 2 * outline) * engine::render_scale();
	}
	const float outline_max_w{max_w != UNLIMITED_WIDTH ? max_w + 2 * scaled_outline : UNLIMITED_WIDTH};

	ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	bitmap render{font_ref.draw(text, outline_max_w, align, {0, 0, 0, 127}, style, scaled_outline)};
	bitmap fill{font_ref.draw(text, max_w, align, {255, 255, 255, 255}, style)};
	for (bitmap::mut_it it = fill.begin(); it != fill.end(); ++it) {
		const rgba8 value{*it};
		u8 shade{static_cast<u8>(value.r / 4 + value.r * 3 / 4 * (fill.size().y - it.pos().y) / fill.size().y)};
		*it = rgba8{shade, shade, shade, value.a};
	}
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - 2 * scaled_outline}));
	return render;
}