#include "../include/font_manager.hpp"
#include "../include/engine.hpp"

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

tr::ttfont load_font(std::string_view name)
{
	try {
		std::filesystem::path path{cli_settings.datadir / "fonts" / name};
		if (is_regular_file(path)) {
			tr::ttfont font{tr::load_ttfont_file(path, 48)};
			LOG(tr::severity::INFO, "Loaded font '{}'.", name);
			return font;
		}
		path = cli_settings.userdir / "fonts" / name;
		if (is_regular_file(path)) {
			tr::ttfont font{tr::load_ttfont_file(path, 48)};
			LOG(tr::severity::INFO, "Loaded font '{}'.", name);
			return font;
		}
		LOG(tr::severity::ERROR, "Failed to load font '{}': File not found.", name);
		throw std::runtime_error{std::format("Failed to load font '{}': File not found", name)};
	}
	catch (tr::ttfont_file_load_error& err) {
		LOG(tr::severity::ERROR, "Failed to load font '{}': {}.", name, err.what());
		throw std::runtime_error{std::format("Failed to load font '{}': {}", name, err.what())};
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
		switch (_language_preview_font.state) {
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

///////////////////////////////////////////////////////////////// LOADING /////////////////////////////////////////////////////////////////

void font_manager_t::load_fonts()
{
	_standard_fonts.emplace(load_font("charge_vector_b.otf"), load_font("linux_biolinum_rb.ttf"));
	try {
		std::string language_font{get_language_font(settings.language)};
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
		new_font = get_language_font(pending.language);
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
	auto& font{find_font(font::LANGUAGE)};
	if (std::ranges::all_of(tr::utf8::range(text), [&](tr::codepoint chr) { return font.contains(chr); })) {
		return font::LANGUAGE;
	}
	else {
		return font::FALLBACK;
	}
}

glm::vec2 font_manager_t::text_size(std::string_view text, font font, float size, int outline, int max_w)
{
	outline = std::max(static_cast<int>(outline * engine::render_scale()), 1);

	tr::ttfont& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	return static_cast<glm::vec2>(font_ref.text_size(text, max_w, tr::ttf_style::NORMAL, outline)) / engine::render_scale();
}

tr::bitmap font_manager_t::render_text(std::string_view text, font font, float size, int outline, int max_w, tr::halign align)
{
	const int scaled_outline{static_cast<int>(outline * engine::render_scale())};
	if (max_w != tr::UNLIMITED_WIDTH) {
		max_w -= 2 * scaled_outline;
	}

	auto& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	tr::bitmap render{font_ref.draw(text, max_w + 2 * scaled_outline, align, {127, 127, 127, 127}, tr::ttf_style::NORMAL, scaled_outline)};
	const tr::bitmap fill{font_ref.draw(text, max_w, align, {255, 255, 255, 255})};
	render.blit(glm::ivec2{scaled_outline}, fill.sub({{}, render.size() - scaled_outline * 2}));
	return render;
}

tr::bitmap font_manager_t::render_gradient_text(std::string_view text, font font, float size, int outline, int max_w, tr::halign align)
{
	const int scaled_outline{static_cast<int>(outline * engine::render_scale())};
	if (max_w != tr::UNLIMITED_WIDTH) {
		max_w -= 2 * scaled_outline;
	}

	auto& font_ref{find_font(font)};
	font_ref.resize(size * engine::render_scale());
	tr::bitmap render{font_ref.draw(text, max_w + 2 * scaled_outline, align, {0, 0, 0, 127}, tr::ttf_style::NORMAL, scaled_outline)};
	tr::bitmap fill{font_ref.draw(text, max_w, align, {255, 255, 255, 255})};
	for (auto it = fill.begin(); it != fill.end(); ++it) {
		const tr::rgba8 value{*it};
		std::uint8_t shade{static_cast<std::uint8_t>(value.r / 4 + value.r * 3 / 4 * (fill.size().y - it.pos().y) / fill.size().y)};
		*it = tr::rgba8{shade, shade, shade, value.a};
	}
	render.blit(glm::ivec2{outline}, fill.sub({{}, render.size() - 2 * outline}));
	return render;
}