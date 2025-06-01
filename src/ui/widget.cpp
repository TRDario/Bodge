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

widget::widget(string_view name, vec2 pos, align alignment, bool hoverable, tooltip_callback tooltip_cb, bool writable,
			   vector<key_chord>&& shortcuts) noexcept
	: name{name}
	, alignment{alignment}
	, pos{pos}
	, tooltip_cb{std::move(tooltip_cb)}
	, _opacity{0}
	, _hoverable{hoverable}
	, _writable{writable}
	, _shortcuts{std::move(shortcuts)}
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

bool widget::hoverable() const noexcept
{
	return _hoverable;
}

bool widget::writable() const noexcept
{
	return _writable;
}

bool widget::active() const noexcept
{
	return false;
}

bool widget::is_shortcut(const key_chord& chord) const noexcept
{
	vector<key_chord>::const_iterator it{rs::find(_shortcuts, chord)};
	return it != _shortcuts.end();
}

void widget::update()
{
	pos.update();
	_opacity.update();
}

/////////////////////////////////////////////////////////////// TEXT_WIDGET ///////////////////////////////////////////////////////////////

text_widget::text_widget(string_view name, vec2 pos, align alignment, bool hoverable, tooltip_callback tooltip_cb, bool writable,
						 vector<key_chord>&& shortcuts, font font, ttf_style style, halign text_alignment, float font_size, int max_width,
						 rgba8 color, text_callback text_cb)
	: widget{name, pos, alignment, hoverable, std::move(tooltip_cb), writable, std::move(shortcuts)}
	, color{color}
	, text_cb{std::move(text_cb)}
	, _font{font}
	, _style{style}
	, _text_alignment{text_alignment}
	, _font_size{font_size}
	, _max_width{max_width}
{
}

text_widget::text_widget(string_view name, vec2 pos, align alignment, font font, ttf_style style, float font_size, text_callback text_cb,
						 rgba8 color)
	: text_widget{name, pos,   alignment,      false,     NO_TOOLTIP,      false, {},
				  font, style, halign::CENTER, font_size, UNLIMITED_WIDTH, color, std::move(text_cb)}
{
}

text_widget::text_widget(string_view name, vec2 pos, align alignment, string_view tooltip_key, font font, ttf_style style, float font_size,
						 text_callback text_cb)
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  [=] { return string{localization[tooltip_key]}; },
				  false,
				  {},
				  font,
				  style,
				  halign::CENTER,
				  font_size,
				  UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  std::move(text_cb)}
{
}

vec2 text_widget::size() const
{
	if (!_cached.has_value()) {
		update_cache();
	}
	return _cached->size;
}

void text_widget::update() noexcept
{
	widget::update();
	color.update();
}

void text_widget::release_graphical_resources() noexcept
{
	_cached.reset();
}

void text_widget::add_to_renderer()
{
	update_cache();

	rgba8 color{this->color};
	color.a = static_cast<u8>(color.a * opacity());

	array<tintvtx, 4> quad;
	fill_rect_vtx(positions(quad), {tl(), text_widget::size()});
	fill_rect_vtx(uvs(quad), {{}, _cached->size / vec2{_cached->texture.size()}});
	rs::fill(colors(quad), rgba8{color});
	engine::batched_renderer().add_tex_quad(quad, 0, _cached->texture, TRANSFORM);
}

void text_widget::update_cache() const
{
	string text{text_cb(name)};
	if (!_cached.has_value() || _cached->text != text) {
		bitmap render{font_manager.render_text(text, _font, _style, _font_size, _font_size / 12, _max_width, _text_alignment)};
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

////////////////////////////////////////////////////////// CLICKABLE_TEXT_WIDGET //////////////////////////////////////////////////////////

clickable_text_widget::clickable_text_widget(string_view name, vec2 pos, align alignment, font font, float font_size, text_callback text_cb,
											 status_callback status_cb, action_callback action_cb, tooltip_callback tooltip_cb,
											 vector<key_chord>&& shortcuts) noexcept
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  std::move(tooltip_cb),
				  false,
				  std::move(shortcuts),
				  font,
				  ttf_style::NORMAL,
				  halign::CENTER,
				  font_size,
				  UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  std::move(text_cb)}
	, _status_cb{std::move(status_cb)}
	, _action_cb{std::move(action_cb)}
{
}

void clickable_text_widget::add_to_renderer()
{
	interpolated_rgba8 real_color{color};
	if (!active()) {
		color = {80, 80, 80, 160};
	}
	text_widget::add_to_renderer();
	color = real_color;
}

bool clickable_text_widget::active() const noexcept
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
	if (active()) {
		_action_cb();
	}
}

/////////////////////////////////////////////////////////// COLOR_PREVIEW_WIDGET //////////////////////////////////////////////////////////

color_preview_widget::color_preview_widget(string_view name, vec2 pos, align alignment, u16& hue_ref) noexcept
	: widget{name, pos, alignment, false, NO_TOOLTIP, false, {}}, _hue_ref{hue_ref}
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

arrow_widget::arrow_widget(string_view name, vec2 pos, align alignment, bool right_arrow, status_callback status_cb,
						   action_callback action_cb, vector<key_chord>&& chords) noexcept
	: widget{name, pos, alignment, true, NO_TOOLTIP, false, std::move(chords)}
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
	const rgba8 color{active() ? rgba8{_color} : rgba8{80, 80, 80, 160}};
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

bool arrow_widget::active() const noexcept
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
	_action_cb();
}

////////////////////////////////////////////////////// REPLAY_PLAYBACK_INDICATOR_WIDGET ///////////////////////////////////////////////////

// Creates a replay playback indicator widget.
replay_playback_indicator_widget::replay_playback_indicator_widget(string_view name, vec2 pos, align alignment) noexcept
	: widget{name, pos, alignment, false, NO_TOOLTIP, false, {}}
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

////////////////////////////////////////////////////////////// SCORE WIDGET ///////////////////////////////////////////////////////////////

score_widget::score_widget(string_view name, vec2 pos, align alignment, size_t rank, ::score* score) noexcept
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  [this] { return this->score != nullptr ? this->score->description : string{}; },
				  false,
				  {},
				  font::LANGUAGE,
				  ttf_style::NORMAL,
				  halign::CENTER,
				  48,
				  UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  [this](const static_string<30>&) {
					  if (this->score == nullptr) {
						  return string{"----------------------------------"};
					  }

					  const ticks result{this->score->result};
					  const ch::system_clock::time_point utc_tp{ch::seconds{this->score->timestamp}};
					  const auto tp{std::chrono::current_zone()->ch::time_zone::to_local(utc_tp)};
					  const ch::hh_mm_ss hhmmss{tp - ch::floor<ch::days>(tp)};
					  const ch::year_month_day ymd{ch::floor<ch::days>(tp)};
					  return format("{}) {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", this->rank, result / 60_s, (result % 60_s) / 1_s,
									(result % 1_s) * 100 / 1_s, static_cast<int>(ymd.year()), static_cast<unsigned int>(ymd.month()),
									static_cast<unsigned int>(ymd.day()), hhmmss.hours().count(), hhmmss.minutes().count());
				  }}
	, rank{rank}
	, score{score}
{
}

vec2 score_widget::size() const noexcept
{
	if (score != nullptr) {
		const int icons{score->flags.exited_prematurely + score->flags.modified_game_speed};
		if (icons != 0) {
			return text_widget::size() + vec2{0, 20};
		}
	}
	return text_widget::size();
}

vec2 score_widget::tl() const noexcept
{
	if (score != nullptr) {
		const int icons{score->flags.exited_prematurely + score->flags.modified_game_speed};
		if (icons == 0) {
			return text_widget::tl() + vec2{0, 10};
		}
	}
	return text_widget::tl();
}

void score_widget::add_to_renderer()
{
	interpolated_rgba8 real_color{color};
	if (score == nullptr) {
		color = {80, 80, 80, 160};
	}
	text_widget::add_to_renderer();
	color = real_color;

	if (score != nullptr) {
		const vec2 text_size{text_widget::size()};
		int icons{score->flags.exited_prematurely + score->flags.modified_game_speed};
		int i = 0;

		if (score->flags.exited_prematurely) {
			vector<u16> indices;
			fill_poly_idx(back_inserter(indices), 4, 0);
			fill_poly_outline_idx(back_inserter(indices), 4, 4);
			fill_poly_idx(back_inserter(indices), 4, 12);
			fill_poly_idx(back_inserter(indices), 4, 16);
			engine::layered_renderer().add_color_mesh(
				layer::UI,
				generate_exited_prematurely_icon(tl() + vec2{text_size.x / 2 - 15 * icons + 30 * i, text_size.y}, color, opacity()),
				std::move(indices));
			++i;
		}
		if (score->flags.modified_game_speed) {
			vector<u16> indices;
			fill_poly_idx(back_inserter(indices), 4, 0);
			fill_poly_outline_idx(back_inserter(indices), 4, 4);
			fill_poly_outline_idx(back_inserter(indices), 8, 12);
			fill_poly_idx(back_inserter(indices), 4, 28);
			engine::layered_renderer().add_color_mesh(
				layer::UI,
				generate_modified_game_speed_icon(tl() + vec2{text_size.x / 2 - 15 * icons + 30 * i, text_size.y}, color, opacity()),
				std::move(indices));
			++i;
		}
	}
}

////////////////////////////////////////////////////////////// REPLAY_WIDGET //////////////////////////////////////////////////////////////

vec2 replay_widget::size() const noexcept
{
	if (it.has_value()) {
		const int icons{(*it)->second.flags.exited_prematurely + (*it)->second.flags.modified_game_speed};
		if (icons != 0) {
			return clickable_text_widget::size() + vec2{0, 20};
		}
	}
	return clickable_text_widget::size();
}

vec2 replay_widget::tl() const noexcept
{
	if (it.has_value()) {
		const int icons{(*it)->second.flags.exited_prematurely + (*it)->second.flags.modified_game_speed};
		if (icons == 0) {
			return clickable_text_widget::tl() + vec2{0, 10};
		}
	}
	return clickable_text_widget::tl();
}

void replay_widget::add_to_renderer()
{
	interpolated_rgba8 real_color{color};
	if (!it.has_value()) {
		color = {80, 80, 80, 160};
	}
	text_widget::add_to_renderer();
	color = real_color;

	if (it.has_value()) {
		const vec2 text_size{clickable_text_widget::size()};
		const rgba8 color{active() ? rgba8{this->color} : rgba8{128, 128, 128, 128}};
		int icons{(*it)->second.flags.exited_prematurely + (*it)->second.flags.modified_game_speed};
		int i = 0;

		if ((*it)->second.flags.exited_prematurely) {
			vector<u16> indices;
			fill_poly_idx(back_inserter(indices), 4, 0);
			fill_poly_outline_idx(back_inserter(indices), 4, 4);
			fill_poly_idx(back_inserter(indices), 4, 12);
			fill_poly_idx(back_inserter(indices), 4, 16);
			engine::layered_renderer().add_color_mesh(
				layer::UI,
				generate_exited_prematurely_icon(tl() + vec2{text_size.x / 2 - 15 * icons + 30 * i, text_size.y}, color, opacity()),
				std::move(indices));
			++i;
		}
		if ((*it)->second.flags.modified_game_speed) {
			vector<u16> indices;
			fill_poly_idx(back_inserter(indices), 4, 0);
			fill_poly_outline_idx(back_inserter(indices), 4, 4);
			fill_poly_outline_idx(back_inserter(indices), 8, 12);
			fill_poly_idx(back_inserter(indices), 4, 28);
			engine::layered_renderer().add_color_mesh(
				layer::UI,
				generate_modified_game_speed_icon(tl() + vec2{text_size.x / 2 - 15 * icons + 30 * i, text_size.y}, color, opacity()),
				std::move(indices));
			++i;
		}
	}
}