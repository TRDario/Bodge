#include "../../include/engine.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Mesh used for left arrows.
constexpr array<clrvtx, 15> LEFT_ARROW_MESH{{
	{{30, 0}, {127, 127, 127, 127}},
	{{16, 0}, {127, 127, 127, 127}},
	{{0, 24}, {127, 127, 127, 127}},
	{{16, 48}, {127, 127, 127, 127}},
	{{30, 48}, {127, 127, 127, 127}},
	{{26, 4}, {127, 127, 127, 127}},
	{{18.30940108, 4}, {127, 127, 127, 127}},
	{{4.61880215, 24}, {127, 127, 127, 127}},
	{{18.30940108, 44}, {127, 127, 127, 127}},
	{{26, 44}, {127, 127, 127, 127}},
	{{26, 4}, {255, 255, 255, 255}},
	{{18.30940108, 4}, {255, 255, 255, 255}},
	{{4.61880215, 24}, {213, 213, 213, 255}},
	{{18.30940108, 44}, {192, 192, 192, 255}},
	{{26, 44}, {192, 192, 192, 255}},
}};

// Mesh used for right arrows.
constexpr array<clrvtx, 15> RIGHT_ARROW_MESH{{
	{{0, 0}, {127, 127, 127, 127}},
	{{14, 0}, {127, 127, 127, 127}},
	{{30, 24}, {127, 127, 127, 127}},
	{{14, 48}, {127, 127, 127, 127}},
	{{0, 48}, {127, 127, 127, 127}},
	{{4, 4}, {127, 127, 127, 127}},
	{{11.69059892, 4}, {127, 127, 127, 127}},
	{{25.38119785, 24}, {127, 127, 127, 127}},
	{{11.69059892, 44}, {127, 127, 127, 127}},
	{{4, 44}, {127, 127, 127, 127}},
	{{4, 4}, {255, 255, 255, 255}},
	{{11.69059892, 4}, {255, 255, 255, 255}},
	{{25.38119785, 24}, {213, 213, 213, 255}},
	{{11.69059892, 44}, {192, 192, 192, 255}},
	{{4, 44}, {192, 192, 192, 255}},
}};

constexpr array<clrvtx, 9> NORMAL_SPEED_MESH{{
	{{12, 0}, {95, 95, 95, 95}},
	{{48, 24}, {95, 95, 95, 95}},
	{{12, 48}, {95, 95, 95, 95}},
	{{16, 7.47406836}, {95, 95, 95, 95}},
	{{40.78889744, 24}, {95, 95, 95, 95}},
	{{16, 40.52593164}, {95, 95, 95, 95}},
	{{16, 7.47406836}, {192, 192, 192, 192}},
	{{40.78889744, 24}, {168, 168, 168, 192}},
	{{16, 40.52593164}, {144, 144, 144, 192}},
}};

constexpr array<clrvtx, 9> SLOW_SPEED_MESH{{
	{{48, 0}, {95, 95, 95, 95}},
	{{12, 24}, {95, 95, 95, 95}},
	{{48, 48}, {95, 95, 95, 95}},
	{{44, 7.47406836}, {95, 95, 95, 95}},
	{{19.21110256, 24}, {95, 95, 95, 95}},
	{{44, 40.52593164}, {95, 95, 95, 95}},
	{{44, 7.47406836}, {192, 192, 192, 192}},
	{{19.21110256, 24}, {168, 168, 168, 192}},
	{{44, 40.52593164}, {144, 144, 144, 192}},
}};

constexpr array<clrvtx, 19> FAST_SPEED_MESH{{
	{{0, 0}, {95, 95, 95, 95}},
	{{20, 14.5364}, {95, 95, 95, 95}},
	{{20, 0}, {95, 95, 95, 95}},
	{{48, 24}, {95, 95, 95, 95}},
	{{20, 48}, {95, 95, 95, 95}},
	{{20, 33.4636}, {95, 95, 95, 95}},
	{{0, 48}, {95, 95, 95, 95}},
	{{4, 9.5409}, {95, 95, 95, 95}},
	{{24, 24}, {95, 95, 95, 95}},
	{{24, 9.5409}, {95, 95, 95, 95}},
	{{41.5364, 24}, {95, 95, 95, 95}},
	{{24, 38.4591}, {95, 95, 95, 95}},
	{{4, 38.4591}, {95, 95, 95, 95}},
	{{4, 9.5409}, {192, 192, 192, 192}},
	{{24, 24}, {168, 168, 168, 168}},
	{{4, 38.4591}, {144, 144, 144, 144}},
	{{24, 9.5409}, {192, 192, 192, 192}},
	{{41.5364, 24}, {168, 168, 168, 168}},
	{{24, 38.4591}, {144, 144, 144, 144}},
}};

constexpr array<u16, 48> FAST_SPEED_MESH_INDICES{
	0, 1,  7, 1, 7,  8, 1, 8,  9, 1, 9,  2, 2, 3,  9, 3, 10, 9, 3,  10, 4,  10, 11, 4,
	4, 11, 5, 5, 11, 8, 8, 12, 5, 5, 12, 6, 6, 12, 0, 0, 12, 7, 13, 14, 15, 16, 17, 18,
};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

// Generates the "exited prematurely" icon.
array<clrvtx, 20> generate_exited_prematurely_icon(vec2 pos, rgba8 color, float opacity)
{
	array<clrvtx, 20> mesh{};
	fill_rect_vtx(positions(mesh), {pos + vec2{2, 2}, {16, 16}});
	fill_rect_outline_vtx(positions(mesh) | vs::drop(4), {pos + vec2{1, 1}, {18, 18}}, 2.0f);
	tr::fill_rotated_rect_vtx(positions(mesh) | vs::drop(12), pos + vec2{10, 10}, {7, 1}, {14, 2}, 45_degf);
	tr::fill_rotated_rect_vtx(positions(mesh) | vs::drop(16), pos + vec2{10, 10}, {7, 1}, {14, 2}, -45_degf);
	rs::fill(colors(mesh) | vs::take(4), rgba8{0, 0, 0, norm_cast<u8>(opacity)});
	rs::fill(colors(mesh) | vs::drop(4), rgba8{color.r, color.g, color.b, static_cast<u8>(color.a * opacity)});
	return mesh;
};

// Generates the "modified game speed" icon.
array<clrvtx, 32> generate_modified_game_speed_icon(vec2 pos, rgba8 color, float opacity)
{
	array<clrvtx, 32> mesh{};
	fill_rect_vtx(positions(mesh), {pos + vec2{2, 2}, {16, 16}});
	fill_rect_outline_vtx(positions(mesh) | vs::drop(4), {pos + vec2{1, 1}, {18, 18}}, 2.0f);
	tr::fill_poly_outline_vtx(positions(mesh) | vs::drop(12), 8, {pos + vec2{10, 10}, 7}, 25_degf, 2);
	fill_rect_vtx(positions(mesh) | vs::drop(28), {pos + vec2{9, 5}, {2, 5}});
	rs::fill(colors(mesh) | vs::take(4), rgba8{0, 0, 0, static_cast<u8>(color.a * opacity)});
	rs::fill(colors(mesh) | vs::drop(4), rgba8{color.r, color.g, color.b, static_cast<u8>(color.a * opacity)});
	return mesh;
};

///////////////////////////////////////////////////////////////// WIDGET //////////////////////////////////////////////////////////////////

widget::widget(string&& name, vec2 pos, align alignment) noexcept
	: name{std::move(name)}, pos{pos}, alignment{alignment}, _opacity{0}
{
}

vec2 widget::tl() const noexcept
{
	return tr::tl(vec2{pos}, size(), alignment);
}

float widget::opacity() const noexcept
{
	return _opacity;
}

void widget::hide() noexcept
{
	_opacity = 0;
}

void widget::hide(u16 time) noexcept
{
	_opacity.change(0, time);
}

void widget::unhide() noexcept
{
	_opacity = 1;
}

void widget::unhide(u16 time) noexcept
{
	_opacity.change(1, time);
}

void widget::update()
{
	pos.update();
	_opacity.update();
}

void widget::release_graphical_resources() noexcept {}

//////////////////////////////////////////////////////////////// MOUSABLE /////////////////////////////////////////////////////////////////

mousable::mousable(tooltip_callback tooltip_cb) noexcept
	: tooltip_cb{std::move(tooltip_cb)}
{
}

////////////////////////////////////////////////////////////// SHORTCUTABLE ///////////////////////////////////////////////////////////////

shortcutable::shortcutable(vector<key_chord>&& chords) noexcept
	: chords{std::move(chords)}
{
}

//////////////////////////////////////////////////////////// BASIC_TEXT_WIDGET ////////////////////////////////////////////////////////////

basic_text_widget::basic_text_widget(string&& name, vec2 pos, align alignment, font font, float font_size, text_callback text_cb,
									 rgba8 color, ttf_style style) noexcept
	: widget{std::move(name), pos, alignment}, color{color}, _font{font}, _style{style}, _font_size{font_size}, _text_cb{std::move(text_cb)}
{
}

void basic_text_widget::set_text_callback(text_callback text_cb) noexcept
{
	_text_cb = std::move(text_cb);
}

vec2 basic_text_widget::size() const noexcept
{
	return _cached.has_value() ? _cached->size / engine::render_scale() : vec2{0};
}

void basic_text_widget::update() noexcept
{
	widget::update();
	color.update();
}

void basic_text_widget::release_graphical_resources() noexcept
{
	_cached.reset();
}

void basic_text_widget::add_to_renderer()
{
	update_cache();

	rgba8 color{this->color};
	color.a = static_cast<u8>(color.a * opacity());

	array<tintvtx, 4> quad;
	fill_rect_vtx(positions(quad), {tl(), basic_text_widget::size()});
	fill_rect_vtx(uvs(quad), {{}, _cached->size / vec2{_cached->texture.size()}});
	rs::fill(colors(quad), rgba8{color});
	engine::batched_renderer().add_tex_quad(quad, 0, _cached->texture, TRANSFORM);
}

void basic_text_widget::update_cache()
{
	string text{_text_cb(name)};
	if (!_cached.has_value() || _cached->text != text) {
		bitmap render{font_manager.render_text(text, _font, _style, _font_size, _font_size / 12, UNLIMITED_WIDTH, tr::halign::CENTER)};
		if (!_cached || _cached->texture.size().x < render.size().x || _cached->texture.size().y < render.size().y) {
			_cached.emplace(texture{render}, render.size(), std::move(text));
			if (cli_settings.debug_mode) {
				_cached->texture.set_label(format("(Bodge) Widget texture - \"{}\"", name));
			}
		}
		else {
			_cached->texture.clear({});
			_cached->texture.set_region({}, render);
			_cached->size = render.size();
			_cached->text = std::move(text);
		}
	}
}

//////////////////////////////////////////////////////// TOOLTIPPABLE_TEXT_WIDGET /////////////////////////////////////////////////////////

tooltippable_text_widget::tooltippable_text_widget(string&& name, vec2 pos, align alignment, string_view tooltip_key,
												   float font_size) noexcept
	: basic_text_widget{std::move(name), pos, alignment, font::LANGUAGE, font_size}
	, mousable{[=] { return string{localization[tooltip_key]}; }}
{
}

bool tooltippable_text_widget::holdable() const noexcept
{
	return false;
}

////////////////////////////////////////////////////////// CLICKABLE_TEXT_WIDGET //////////////////////////////////////////////////////////

clickable_text_widget::clickable_text_widget(string&& name, vec2 pos, align alignment, float font_size, status_callback status_cb,
											 action_callback action_cb, vector<key_chord>&& chords, tooltip_callback tooltip_cb) noexcept
	: basic_text_widget{std::move(name), pos, alignment, font::LANGUAGE, font_size}
	, mousable{std::move(tooltip_cb)}
	, shortcutable{std::move(chords)}
	, _status_cb{std::move(status_cb)}
	, _action_cb{std::move(action_cb)}
{
}

clickable_text_widget::clickable_text_widget(string&& name, vec2 pos, align alignment, font font, float font_size,
											 status_callback status_cb, action_callback action_cb, text_callback text_cb,
											 vector<key_chord>&& chords, tooltip_callback tooltip_cb) noexcept
	: basic_text_widget{std::move(name), pos, alignment, font, font_size, std::move(text_cb)}
	, mousable{std::move(tooltip_cb)}
	, shortcutable{std::move(chords)}
	, _status_cb{std::move(status_cb)}
	, _action_cb{std::move(action_cb)}
{
}

void clickable_text_widget::add_to_renderer()
{
	interpolated_rgba8 real_color{color};
	if (!holdable()) {
		color = {80, 80, 80, 160};
	}
	basic_text_widget::add_to_renderer();
	color = real_color;
}

bool clickable_text_widget::holdable() const noexcept
{
	return _status_cb();
}

void clickable_text_widget::on_hover() noexcept
{
	color.change({255, 255, 255, 255}, 0.2_s);
}

void clickable_text_widget::on_unhover() noexcept
{
	color.change({160, 160, 160, 160}, 0.2_s);
}

void clickable_text_widget::on_hold_begin() noexcept
{
	color = {32, 32, 32, 255};
}

void clickable_text_widget::on_hold_transfer_in() noexcept
{
	color = {32, 32, 32, 255};
}

void clickable_text_widget::on_hold_transfer_out() noexcept
{
	color.change({160, 160, 160, 160}, 0.2_s);
}

void clickable_text_widget::on_hold_end() noexcept
{
	color.change({255, 255, 255, 255}, 0.2_s);
	_action_cb();
}

void clickable_text_widget::on_shortcut() noexcept
{
	if (holdable()) {
		_action_cb();
	}
}

////////////////////////////////////////////////////////// TEXT_LINE_INPUT_WIDGET /////////////////////////////////////////////////////////

text_line_input_widget::text_line_input_widget(string&& name, vec2 pos, align alignment, float font_size, status_callback status_cb,
											   action_callback enter_cb, u8 max_size, string&& starting_text)
	: basic_text_widget{std::move(name), pos,
						alignment,       font::LANGUAGE,
						font_size,       [this](const string&) { return buffer.empty() ? std::string{localization["empty"]} : buffer; }}
	, buffer{std::move(starting_text)}
	, _status_cb{std::move(status_cb)}
	, _enter_cb{std::move(enter_cb)}
	, _max_size{max_size}
{
}

void text_line_input_widget::add_to_renderer()
{
	interpolated_rgba8 real_color{color};
	if (!holdable()) {
		color = {80, 80, 80, 160};
	}
	else if (buffer.empty()) {
		rgba8 real{real_color};
		color = {static_cast<u8>(real.r / 2), static_cast<u8>(real.g / 2), static_cast<u8>(real.b / 2), real.a};
	}
	basic_text_widget::add_to_renderer();
	color = real_color;
}

bool text_line_input_widget::holdable() const noexcept
{
	return _status_cb();
}

void text_line_input_widget::on_hover() noexcept
{
	if (!_has_focus) {
		color.change({220, 220, 220, 220}, 0.2_s);
	}
}

void text_line_input_widget::on_unhover() noexcept
{
	if (!_has_focus) {
		color.change({160, 160, 160, 160}, 0.2_s);
	}
}

void text_line_input_widget::on_hold_begin() noexcept
{
	color = {32, 32, 32, 255};
}

void text_line_input_widget::on_hold_transfer_in() noexcept
{
	color = {32, 32, 32, 255};
}

void text_line_input_widget::on_hold_transfer_out() noexcept
{
	color = rgba8{160, 160, 160, 160};
}

void text_line_input_widget::on_hold_end() noexcept
{
	_has_focus = true;
	color = {255, 255, 255, 255};
}

void text_line_input_widget::on_gain_focus()
{
	_has_focus = true;
	color.change({255, 255, 255, 255}, 0.2_s);
}

void text_line_input_widget::on_lose_focus()
{
	_has_focus = false;
	color.change({160, 160, 160, 160}, 0.2_s);
}

void text_line_input_widget::on_write(string_view input)
{
	buffer += input;
	size_t length{tr::utf8::length(buffer)};
	while (length-- > _max_size) {
		tr::utf8::pop_back(buffer);
	}
}

void text_line_input_widget::on_enter()
{
	_enter_cb();
}

void text_line_input_widget::on_erase()
{
	if (!buffer.empty()) {
		tr::utf8::pop_back(buffer);
	}
}

void text_line_input_widget::on_clear()
{
	buffer.clear();
}

void text_line_input_widget::on_copy()
{
	keyboard::set_clipboard_text(buffer);
}

void text_line_input_widget::on_paste()
{
	if (keyboard::clipboard_has_text()) {
		buffer += keyboard::clipboard_text();
		size_t length{tr::utf8::length(buffer)};
		while (length-- > _max_size) {
			tr::utf8::pop_back(buffer);
		}
	}
}

/////////////////////////////////////////////////////////// COLOR_PREVIEW_WIDGET //////////////////////////////////////////////////////////

color_preview_widget::color_preview_widget(string&& name, vec2 pos, align alignment, u16& hue_ref) noexcept
	: widget{std::move(name), pos, alignment}, _hue_ref{hue_ref}
{
}

vec2 color_preview_widget::size() const noexcept
{
	return {48, 48};
}

void color_preview_widget::add_to_renderer()
{
	const rgba8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(_hue_ref), 1, 1}), norm_cast<u8>(opacity())};
	const rgba8 outline_color{static_cast<u8>(color.r / 2), static_cast<u8>(color.g / 2), static_cast<u8>(color.b / 2),
							  static_cast<u8>(color.a / 2)};

	array<clrvtx, 12> buffer;
	vector<u16> indices(poly_outline_idx(4) + poly_idx(4));
	fill_rect_outline_vtx(positions(buffer).begin(), {tl() + 2.0f, size() - 4.0f}, 4.0f);
	fill_rect_vtx(positions(buffer).begin() + 8, {tl() + 4.0f, size() - 8.0f});
	rs::fill(vs::take(colors(buffer), 8), outline_color);
	rs::fill(vs::drop(colors(buffer), 8), color);
	fill_poly_outline_idx(indices.begin(), 4, 0);
	fill_poly_idx(indices.begin() + poly_outline_idx(4), 4, 8);
	engine::layered_renderer().add_color_mesh(layer::UI, buffer, std::move(indices));
}

/////////////////////////////////////////////////////////////// ARROW_WIDGET //////////////////////////////////////////////////////////////

arrow_widget::arrow_widget(string&& name, vec2 pos, align alignment, bool right_arrow, status_callback status_cb, action_callback action_cb,
						   vector<key_chord>&& chords) noexcept
	: widget{std::move(name), pos, alignment}
	, shortcutable{std::move(chords)}
	, _right{right_arrow}
	, _color{{160, 160, 160, 160}}
	, _status_cb{std::move(status_cb)}
	, _action_cb{std::move(action_cb)}
{
}

vec2 arrow_widget::size() const noexcept
{
	return {30, 48};
}

void arrow_widget::add_to_renderer()
{
	const rgba8 color{holdable() ? rgba8{_color} : rgba8{80, 80, 80, 160}};
	array<clrvtx, 15> buffer{_right ? RIGHT_ARROW_MESH : LEFT_ARROW_MESH};
	for (clrvtx& vtx : buffer) {
		vtx.pos += tl();
		vtx.color.r = (vtx.color.r / 255.0f) * color.r;
		vtx.color.g = (vtx.color.g / 255.0f) * color.g;
		vtx.color.b = (vtx.color.b / 255.0f) * color.b;
		vtx.color.a = (vtx.color.a / 255.0f) * color.a * opacity();
	}
	vector<u16> indices(poly_outline_idx(5) + poly_idx(5));
	fill_poly_outline_idx(indices.begin(), 5, 0);
	fill_poly_idx(indices.begin() + poly_outline_idx(5), 5, 10);
	engine::layered_renderer().add_color_mesh(layer::UI, buffer, std::move(indices));
}

void arrow_widget::update() noexcept
{
	_color.update();
	widget::update();
}

bool arrow_widget::holdable() const noexcept
{
	return _status_cb();
}

void arrow_widget::on_hover() noexcept
{
	_color.change({255, 255, 255, 255}, 0.2_s);
}

void arrow_widget::on_unhover() noexcept
{
	_color.change({160, 160, 160, 160}, 0.2_s);
}

void arrow_widget::on_hold_begin() noexcept
{
	_color = {32, 32, 32, 255};
}

void arrow_widget::on_hold_transfer_in() noexcept
{
	_color = {32, 32, 32, 255};
}

void arrow_widget::on_hold_transfer_out() noexcept
{
	_color.change({160, 160, 160, 160}, 0.2_s);
}

void arrow_widget::on_hold_end() noexcept
{
	_color.change({255, 255, 255, 255}, 0.2_s);
	_action_cb();
}

void arrow_widget::on_shortcut() noexcept
{
	if (holdable()) {
		_action_cb();
	}
}

////////////////////////////////////////////////////// REPLAY_PLAYBACK_INDICATOR_WIDGET ///////////////////////////////////////////////////

// Creates a replay playback indicator widget.
replay_playback_indicator_widget::replay_playback_indicator_widget(string&& name, vec2 pos, align alignment) noexcept
	: widget{std::move(name), pos, alignment}
{
}

vec2 replay_playback_indicator_widget::size() const noexcept
{
	return {48, 48};
}

void replay_playback_indicator_widget::add_to_renderer()
{
	if (keyboard::held_mods() & mods::SHIFT) {
		array<clrvtx, 9> mesh{SLOW_SPEED_MESH};
		for (clrvtx& vtx : mesh) {
			vtx.pos += tl();
		}
		vector<u16> indices(poly_outline_idx(3) + poly_idx(3));
		fill_poly_outline_idx(indices.begin(), 3, 0);
		fill_poly_idx(indices.begin() + poly_outline_idx(3), 3, 6);
		engine::layered_renderer().add_color_mesh(layer::UI, mesh, std::move(indices));
	}
	else if (keyboard::held_mods() & mods::CTRL) {
		array<clrvtx, 19> mesh{FAST_SPEED_MESH};
		for (clrvtx& vtx : mesh) {
			vtx.pos += tl();
		}
		engine::layered_renderer().add_color_mesh(layer::UI, mesh,
												  vector<u16>{FAST_SPEED_MESH_INDICES.begin(), FAST_SPEED_MESH_INDICES.end()});
	}
	else {
		array<clrvtx, 9> mesh{NORMAL_SPEED_MESH};
		for (clrvtx& vtx : mesh) {
			vtx.pos += tl();
		}
		vector<u16> indices(poly_outline_idx(3) + poly_idx(3));
		fill_poly_outline_idx(indices.begin(), 3, 0);
		fill_poly_idx(indices.begin() + poly_outline_idx(3), 3, 6);
		engine::layered_renderer().add_color_mesh(layer::UI, mesh, std::move(indices));
	}
}

////////////////////////////////////////////////////////////// REPLAY_WIDGET //////////////////////////////////////////////////////////////

void replay_widget::set_iterator(optional<map<string, replay_header>::iterator> it) noexcept
{
	_it = it;
}

vec2 replay_widget::size() const noexcept
{
	if (_it.has_value()) {
		const int icons{(*_it)->second.flags.exited_prematurely + (*_it)->second.flags.modified_game_speed};
		if (icons != 0) {
			return clickable_text_widget::size() + vec2{0, 20};
		}
		else {
			return clickable_text_widget::size();
		}
	}
	else {
		return {};
	}
}

vec2 replay_widget::tl() const noexcept
{
	if (_it.has_value()) {
		const int icons{(*_it)->second.flags.exited_prematurely + (*_it)->second.flags.modified_game_speed};
		if (icons == 0) {
			return clickable_text_widget::tl() + vec2{0, 10};
		}
	}
	return clickable_text_widget::tl();
}

void replay_widget::add_to_renderer()
{
	if (_it.has_value()) {
		clickable_text_widget::add_to_renderer();

		const vec2 text_size{clickable_text_widget::size()};
		const rgba8 color{holdable() ? rgba8{this->color} : rgba8{128, 128, 128, 128}};
		int icons{(*_it)->second.flags.exited_prematurely + (*_it)->second.flags.modified_game_speed};
		int i = 0;

		if ((*_it)->second.flags.exited_prematurely) {
			vector<u16> indices;
			fill_poly_idx(back_inserter(indices), 4, 0);
			fill_poly_outline_idx(back_inserter(indices), 4, 4);
			fill_poly_idx(back_inserter(indices), 4, 12);
			fill_poly_idx(back_inserter(indices), 4, 16);
			engine::layered_renderer().add_color_mesh(
				layer::UI,
				generate_exited_prematurely_icon(tl() + vec2{text_size.x / 2 - 10 * icons + 20 * i, text_size.y}, color, opacity()),
				std::move(indices));
			++i;
		}
		if ((*_it)->second.flags.modified_game_speed) {
			vector<u16> indices;
			fill_poly_idx(back_inserter(indices), 4, 0);
			fill_poly_outline_idx(back_inserter(indices), 4, 4);
			fill_poly_outline_idx(back_inserter(indices), 8, 12);
			fill_poly_idx(back_inserter(indices), 4, 28);
			engine::layered_renderer().add_color_mesh(
				layer::UI,
				generate_modified_game_speed_icon(tl() + vec2{text_size.x / 2 - 10 * icons + 20 * i, text_size.y}, color, opacity()),
				std::move(indices));
			++i;
		}
	}
}