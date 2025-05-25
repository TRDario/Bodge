#include "../../include/state/settings_state.hpp"
#include "../../include/audio.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings screen right-aligned widgets.
constexpr array<const char*, 24> RIGHT_WIDGETS{"window_size_dec",
											   "current_window_size",
											   "window_size_inc",
											   "refresh_rate_dec",
											   "current_refresh_rate",
											   "refresh_rate_inc",
											   "msaa_dec",
											   "current_msaa",
											   "msaa_inc",
											   "primary_hue_dec",
											   "current_primary_hue",
											   "primary_hue_inc",
											   "primary_hue_preview",
											   "secondary_hue_dec",
											   "current_secondary_hue",
											   "secondary_hue_inc",
											   "secondary_hue_preview",
											   "sfx_volume_dec",
											   "current_sfx_volume",
											   "sfx_volume_inc",
											   "music_volume_dec",
											   "current_music_volume",
											   "music_volume_inc",
											   "current_language"};
// Settings screen left-aligned labels.
constexpr array<std::pair<const char*, const char*>, 8> LABELS{{{"window_size", "window_size_tt"},
																{"refresh_rate", "refresh_rate_tt"},
																{"msaa", "msaa_tt"},
																{"primary_hue", "primary_hue_tt"},
																{"secondary_hue", "secondary_hue_tt"},
																{"sfx_volume", nullptr},
																{"music_volume", nullptr},
																{"language", nullptr}}};
// Settings screen bottom buttons.
constexpr array<const char*, 3> BOTTOM_BUTTONS{"exit", "apply", "revert"};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

settings_state::settings_state(game&& game)
	: _substate{substate::IN_SETTINGS}, _timer{0}, _game{std::move(game)}, _pending{settings}
{
	basic_text_widget& title{_ui.emplace<basic_text_widget>("settings", vec2{500, -50}, TOP_CENTER, font::LANGUAGE, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	for (int i = 0; i < LABELS.size(); ++i) {
		widget& widget{
			LABELS[i].second != nullptr
				? _ui.emplace<tooltippable_text_widget>(LABELS[i].first, vec2{-200, 196 + i * 75}, CENTER_LEFT, LABELS[i].second, 48)
				: _ui.emplace<basic_text_widget>(LABELS[i].first, vec2{-200, 196 + i * 75}, CENTER_LEFT, font::LANGUAGE, 48)};
		widget.pos.change({15, 196 + i * 75}, 0.5_s);
		widget.unhide(0.5_s);
	}

	float h{196};

	status_callback window_size_dec_status_cb{[this] {
		return _substate != substate::ENTERING_TITLE && _pending.window_size != FULLSCREEN && _pending.window_size > MIN_WINDOW_SIZE;
	}};
	action_callback window_size_dec_action_cb{[this] {
		if (keyboard::held_mods() & mods::SHIFT) {
			_pending.window_size = max(MIN_WINDOW_SIZE, _pending.window_size - 100);
		}
		else {
			_pending.window_size = max(MIN_WINDOW_SIZE, _pending.window_size - 10);
		}
	}};
	arrow_widget& window_size_dec{_ui.emplace<arrow_widget>("window_size_dec", vec2{1200, h}, CENTER_LEFT, false, window_size_dec_status_cb,
															window_size_dec_action_cb)};
	window_size_dec.pos.change({765, h}, 0.5_s);

	status_callback window_size_inc_status_cb{[this] {
		return _substate != substate::ENTERING_TITLE && _pending.window_size != FULLSCREEN && _pending.window_size < max_window_size();
	}};
	action_callback window_size_inc_action_cb{[this] {
		if (keyboard::held_mods() & mods::SHIFT) {
			_pending.window_size = min(max_window_size(), _pending.window_size + 100);
		}
		else {
			_pending.window_size = min(max_window_size(), _pending.window_size + 10);
		}
	}};
	arrow_widget& window_size_inc{_ui.emplace<arrow_widget>("window_size_inc", vec2{1200, h}, CENTER_RIGHT, true, window_size_inc_status_cb,
															window_size_inc_action_cb)};
	window_size_inc.pos.change({985, h}, 0.5_s);
	if (_pending.window_size != FULLSCREEN) {
		window_size_dec.unhide(0.5_s);
		window_size_inc.unhide(0.5_s);
	}

	status_callback current_window_size_status_cb{[this] { return _substate != substate::ENTERING_TITLE; }};
	action_callback current_window_size_action_cb{[=, this] {
		widget& current_window_size{_ui.get("current_window_size")};
		if (_pending.window_size == FULLSCREEN) {
			_pending.window_size = 500;
			current_window_size.pos = vec2{875, h};
			current_window_size.alignment = CENTER;
			_ui.get("window_size_dec").unhide();
			_ui.get("window_size_inc").unhide();
		}
		else {
			_pending.window_size = FULLSCREEN;
			current_window_size.pos = vec2{985, h};
			current_window_size.alignment = CENTER_RIGHT;
			_ui.get("window_size_dec").hide();
			_ui.get("window_size_inc").hide();
		}
	}};
	text_callback current_window_size_text_cb{[this](const string&) {
		if (_pending.window_size == FULLSCREEN) {
			return string{localization["fullscreen"]};
		}
		else {
			return std::to_string(_pending.window_size);
		}
	}};
	clickable_text_widget& current_window_size{_ui.emplace<clickable_text_widget>(
		"current_window_size", vec2{1200, h}, settings.window_size == FULLSCREEN ? CENTER_RIGHT : CENTER, font::LANGUAGE, 48,
		current_window_size_status_cb, current_window_size_action_cb, current_window_size_text_cb)};
	current_window_size.pos.change({settings.window_size == FULLSCREEN ? 985 : 875, h}, 0.5_s);
	current_window_size.unhide(0.5_s);

	h += 75;

	status_callback refresh_rate_dec_status_cb{[this] {
		return _substate != substate::ENTERING_TITLE && _pending.refresh_rate != NATIVE_REFRESH_RATE &&
			   _pending.refresh_rate > MIN_REFRESH_RATE;
	}};
	action_callback refresh_rate_dec_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.refresh_rate = max(15, _pending.refresh_rate - delta);
	}};
	arrow_widget& refresh_rate_dec{_ui.emplace<arrow_widget>("refresh_rate_dec", vec2{1200, h}, CENTER_LEFT, false,
															 refresh_rate_dec_status_cb, refresh_rate_dec_action_cb)};
	refresh_rate_dec.pos.change({800, h}, 0.5_s);

	status_callback refresh_rate_inc_status_cb{[this] {
		return _substate != substate::ENTERING_TITLE && _pending.refresh_rate != NATIVE_REFRESH_RATE &&
			   _pending.refresh_rate < max_refresh_rate();
	}};
	action_callback refresh_rate_inc_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.refresh_rate = min<int>(max_refresh_rate(), _pending.refresh_rate + delta);
	}};
	arrow_widget& refresh_rate_inc{_ui.emplace<arrow_widget>("refresh_rate_inc", vec2{1200, h}, CENTER_RIGHT, true,
															 refresh_rate_inc_status_cb, refresh_rate_inc_action_cb)};
	refresh_rate_inc.pos.change({985, h}, 0.5_s);
	if (_pending.refresh_rate != NATIVE_REFRESH_RATE) {
		refresh_rate_dec.unhide(0.5_s);
		refresh_rate_inc.unhide(0.5_s);
	}

	status_callback current_refresh_rate_status_cb{[this] { return _substate != substate::ENTERING_TITLE; }};
	action_callback current_refresh_rate_action_cb{[=, this] {
		widget& current_refresh_rate{_ui.get("current_refresh_rate")};
		if (_pending.refresh_rate == NATIVE_REFRESH_RATE) {
			_pending.refresh_rate = max_refresh_rate();
			current_refresh_rate.pos = vec2{892.5, h};
			current_refresh_rate.alignment = CENTER;
			_ui.get("refresh_rate_dec").unhide();
			_ui.get("refresh_rate_inc").unhide();
		}
		else {
			_pending.refresh_rate = NATIVE_REFRESH_RATE;
			current_refresh_rate.pos = vec2{985, h};
			current_refresh_rate.alignment = CENTER_RIGHT;
			_ui.get("refresh_rate_dec").hide();
			_ui.get("refresh_rate_inc").hide();
		}
	}};
	text_callback current_refresh_rate_text_cb{[this](const string&) {
		if (_pending.refresh_rate == NATIVE_REFRESH_RATE) {
			return string{localization["native"]};
		}
		else {
			return std::to_string(_pending.refresh_rate);
		}
	}};
	clickable_text_widget& current_refresh_rate{_ui.emplace<clickable_text_widget>(
		"current_refresh_rate", vec2{1200, h}, settings.refresh_rate == NATIVE_REFRESH_RATE ? CENTER_RIGHT : CENTER, font::LANGUAGE, 48,
		current_refresh_rate_status_cb, current_refresh_rate_action_cb, current_refresh_rate_text_cb)};
	current_refresh_rate.pos.change({settings.refresh_rate == NATIVE_REFRESH_RATE ? 985 : 892.5, h}, 0.5_s);
	current_refresh_rate.unhide(0.5_s);

	h += 75;

	status_callback msaa_dec_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.msaa != NO_MSAA; }};
	action_callback msaa_dec_action_cb{[this] { _pending.msaa = _pending.msaa == 2 ? NO_MSAA : _pending.msaa / 2; }};
	arrow_widget& msaa_dec{
		_ui.emplace<arrow_widget>("msaa_dec", vec2{1200, h}, CENTER_LEFT, false, msaa_dec_status_cb, msaa_dec_action_cb)};
	msaa_dec.pos.change({830, h}, 0.5_s);
	msaa_dec.unhide(0.5_s);

	status_callback msaa_inc_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.msaa != max_msaa(); }};
	action_callback msaa_inc_action_cb{[this] { _pending.msaa = _pending.msaa == NO_MSAA ? 2 : _pending.msaa * 2; }};
	arrow_widget& msaa_inc{
		_ui.emplace<arrow_widget>("msaa_inc", vec2{1200, h}, CENTER_RIGHT, true, msaa_inc_status_cb, msaa_inc_action_cb)};
	msaa_inc.pos.change({985, h}, 0.5_s);
	msaa_inc.unhide(0.5_s);

	text_callback current_msaa_text_cb{[this](const string&) {
		if (_pending.msaa == NO_MSAA) {
			return string{"--"};
		}
		else {
			return format("x{}", _pending.msaa);
		}
	}};
	basic_text_widget& current_msaa{
		_ui.emplace<basic_text_widget>("current_msaa", vec2{1200, h}, CENTER, font::LANGUAGE, 48, current_msaa_text_cb)};
	current_msaa.pos.change({907.5, h}, 0.5_s);
	current_msaa.unhide(0.5_s);

	h += 75;

	status_callback hue_arrow_status_cb{[this] { return _substate != substate::ENTERING_TITLE; }};
	action_callback primary_hue_dec_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.primary_hue = (_pending.primary_hue - delta + 360) % 360;
	}};
	arrow_widget& primary_hue_dec{
		_ui.emplace<arrow_widget>("primary_hue_dec", vec2{1200, h}, CENTER_LEFT, false, hue_arrow_status_cb, primary_hue_dec_action_cb)};
	primary_hue_dec.pos.change({745, h}, 0.5_s);
	primary_hue_dec.unhide(0.5_s);

	action_callback primary_hue_inc_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.primary_hue = (_pending.primary_hue + delta) % 360;
	}};
	arrow_widget& primary_hue_inc{
		_ui.emplace<arrow_widget>("primary_hue_inc", vec2{1200, h}, CENTER_RIGHT, true, hue_arrow_status_cb, primary_hue_inc_action_cb)};
	primary_hue_inc.pos.change({930, h}, 0.5_s);
	primary_hue_inc.unhide(0.5_s);

	text_callback current_primary_hue_text_cb{[this](const string&) { return std::to_string(_pending.primary_hue); }};
	basic_text_widget& current_primary_hue{
		_ui.emplace<basic_text_widget>("current_primary_hue", vec2{1200, h}, CENTER, font::LANGUAGE, 48, current_primary_hue_text_cb)};
	current_primary_hue.pos.change({837.5, h}, 0.5_s);
	current_primary_hue.unhide(0.5_s);

	color_preview_widget& primary_hue_preview{
		_ui.emplace<color_preview_widget>("primary_hue_preview", vec2{1200, h}, CENTER_RIGHT, _pending.primary_hue)};
	primary_hue_preview.pos.change({985, h}, 0.5_s);
	primary_hue_preview.unhide(0.5_s);

	h += 75;

	action_callback secondary_hue_dec_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.secondary_hue = (_pending.secondary_hue - delta + 360) % 360;
	}};
	arrow_widget& secondary_hue_dec{_ui.emplace<arrow_widget>("secondary_hue_dec", vec2{1200, h}, CENTER_LEFT, false, hue_arrow_status_cb,
															  secondary_hue_dec_action_cb)};
	secondary_hue_dec.pos.change({745, h}, 0.5_s);
	secondary_hue_dec.unhide(0.5_s);

	action_callback secondary_hue_inc_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.secondary_hue = (_pending.secondary_hue + delta) % 360;
	}};
	arrow_widget& secondary_hue_inc{_ui.emplace<arrow_widget>("secondary_hue_inc", vec2{1200, h}, CENTER_RIGHT, true, hue_arrow_status_cb,
															  secondary_hue_inc_action_cb)};
	secondary_hue_inc.pos.change({930, h}, 0.5_s);
	secondary_hue_inc.unhide(0.5_s);

	text_callback current_secondary_hue_text_cb{[this](const string&) { return std::to_string(_pending.secondary_hue); }};
	basic_text_widget& current_secondary_hue{
		_ui.emplace<basic_text_widget>("current_secondary_hue", vec2{1200, h}, CENTER, font::LANGUAGE, 48, current_secondary_hue_text_cb)};
	current_secondary_hue.pos.change({837.5, h}, 0.5_s);
	current_secondary_hue.unhide(0.5_s);

	color_preview_widget& secondary_hue_preview{
		_ui.emplace<color_preview_widget>("secondary_hue_preview", vec2{1200, h}, CENTER_RIGHT, _pending.secondary_hue)};
	secondary_hue_preview.pos.change({985, h}, 0.5_s);
	secondary_hue_preview.unhide(0.5_s);

	h += 75;

	status_callback sfx_volume_dec_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.sfx_volume > 0; }};
	action_callback sfx_volume_dec_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.sfx_volume = max(_pending.sfx_volume - delta, 0);
	}};
	arrow_widget& sfx_volume_dec{
		_ui.emplace<arrow_widget>("sfx_volume_dec", vec2{1200, h}, CENTER_LEFT, false, sfx_volume_dec_status_cb, sfx_volume_dec_action_cb)};
	sfx_volume_dec.pos.change({765, h}, 0.5_s);
	sfx_volume_dec.unhide(0.5_s);

	status_callback sfx_volume_inc_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.sfx_volume < 100; }};
	action_callback sfx_volume_inc_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.sfx_volume = min(_pending.sfx_volume + delta, 100);
	}};
	arrow_widget& sfx_volume_inc{
		_ui.emplace<arrow_widget>("sfx_volume_inc", vec2{1200, h}, CENTER_RIGHT, true, sfx_volume_inc_status_cb, sfx_volume_inc_action_cb)};
	sfx_volume_inc.pos.change({985, h}, 0.5_s);
	sfx_volume_inc.unhide(0.5_s);

	text_callback current_sfx_volume_text_cb{[this](const string&) { return format("{}%", _pending.sfx_volume); }};
	basic_text_widget& current_sfx_volume{
		_ui.emplace<basic_text_widget>("current_sfx_volume", vec2{1200, h}, CENTER, font::LANGUAGE, 48, current_sfx_volume_text_cb)};
	current_sfx_volume.pos.change({875, h}, 0.5_s);
	current_sfx_volume.unhide(0.5_s);

	h += 75;

	status_callback music_volume_dec_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.music_volume > 0; }};
	action_callback music_volume_dec_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.music_volume = min(_pending.music_volume - delta, 100);
	}};
	arrow_widget& music_volume_dec{_ui.emplace<arrow_widget>("music_volume_dec", vec2{1200, h}, CENTER_LEFT, false,
															 music_volume_dec_status_cb, music_volume_dec_action_cb)};
	music_volume_dec.pos.change({765, h}, 0.5_s);
	music_volume_dec.unhide(0.5_s);

	status_callback music_volume_inc_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.music_volume < 100; }};
	action_callback music_volume_inc_action_cb{[this] {
		int delta{(keyboard::held_mods() & mods::SHIFT) ? 10 : 1};
		_pending.music_volume = min(_pending.music_volume + delta, 100);
	}};
	arrow_widget& music_volume_inc{_ui.emplace<arrow_widget>("music_volume_inc", vec2{1200, h}, CENTER_RIGHT, true,
															 music_volume_inc_status_cb, music_volume_inc_action_cb)};
	music_volume_inc.pos.change({985, h}, 0.5_s);
	music_volume_inc.unhide(0.5_s);

	text_callback current_music_volume_text_cb{[this](const string&) { return format("{}%", _pending.music_volume); }};
	basic_text_widget& current_music_volume{
		_ui.emplace<basic_text_widget>("current_music_volume", vec2{1200, h}, CENTER, font::LANGUAGE, 48, current_music_volume_text_cb)};
	current_music_volume.pos.change({875, h}, 0.5_s);
	current_music_volume.unhide(0.5_s);

	h += 75;

	status_callback current_language_status_cb{[this] { return _substate != substate::ENTERING_TITLE && languages.size() > 1; }};
	action_callback current_language_action_cb{[this] {
		map<language_code, language>::iterator it{std::next(languages.find(_pending.language))};
		if (it == languages.end()) {
			it = languages.begin();
		}
		_pending.language = it->first;
		font_manager.reload_language_preview_font(_pending);
	}};
	text_callback current_language_text_cb{
		[this](const string&) { return languages.contains(_pending.language) ? languages[_pending.language].name : "???"; }};
	clickable_text_widget& current_language{_ui.emplace<clickable_text_widget>("current_language", vec2{1200, h}, CENTER_RIGHT,
																			   font::LANGUAGE_PREVIEW, 48, current_language_status_cb,
																			   current_language_action_cb, current_language_text_cb)};
	current_language.pos.change({985, h}, 0.5_s);
	current_language.unhide(0.5_s);

	array<vector<key_chord>, BOTTOM_BUTTONS.size()> bottom_chords{
		vector<key_chord>{{key::ESCAPE, mods::NONE}},
		vector<key_chord>{{key::S, mods::CTRL}},
		vector<key_chord>{{key::Z, mods::CTRL}},
	};
	array<status_callback, BOTTOM_BUTTONS.size()> bottom_status_cbs{
		[this] { return _substate != substate::ENTERING_TITLE && _pending == settings; },
		[this] { return _substate != substate::ENTERING_TITLE && _pending != settings; },
		[this] { return _substate != substate::ENTERING_TITLE && _pending != settings; },
	};
	array<action_callback, BOTTOM_BUTTONS.size()> bottom_action_cbs{
		[this] {
			_substate = substate::ENTERING_TITLE;
			_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			const settings_t old_settings{settings};
			settings = _pending;
			if (old_settings.window_size != settings.window_size || old_settings.msaa != settings.msaa) {
				_ui.release_graphical_resources();
			}
			if (old_settings.language != settings.language) {
				load_localization();
			}
			if (languages[old_settings.language].font != languages[settings.language].font) {
				_ui.release_graphical_resources();
				font_manager.set_language_font();
			}
			if (old_settings.sfx_volume != settings.sfx_volume || old_settings.music_volume != settings.music_volume) {
				audio::apply_settings();
			}
			engine::apply_settings(old_settings);
		},
		[this] {
			_pending = settings;
			font_manager.reload_language_preview_font(_pending);
		},
	};

	for (int i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		clickable_text_widget& widget{_ui.emplace<clickable_text_widget>(BOTTOM_BUTTONS[i], vec2{500, 1050}, BOTTOM_CENTER, 48,
																		 bottom_status_cbs[i], bottom_action_cbs[i],
																		 std::move(bottom_chords[i]))};
		widget.pos.change({500, 1000 - i * 50}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

u32 settings_state::type() const noexcept
{
	return ID;
}

unique_ptr<state> settings_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

unique_ptr<state> settings_state::update(tr::duration)
{
	++_timer;
	_game.update({});
	_ui.update();

	switch (_substate) {
	case substate::IN_SETTINGS:
		return nullptr;
	case substate::ENTERING_TITLE:
		return _timer >= 0.5_s ? make_unique<title_state>(std::move(_game)) : nullptr;
	}
}

void settings_state::draw()
{
	_game.add_to_renderer();
	engine::layered_renderer().add_color_quad(layer::GAME_OVERLAY, MENU_GAME_OVERLAY_QUAD);
	_ui.add_to_renderer();

	engine::layered_renderer().draw_up_to_layer(layer::UI, engine::screen());
	engine::batched_renderer().draw(engine::screen());
	engine::layered_renderer().draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void settings_state::set_up_exit_animation() noexcept
{
	_ui.get<basic_text_widget>("settings").pos.change({500, -50}, 0.5_s);
	for (const char* tag : BOTTOM_BUTTONS) {
		_ui.get(tag).pos.change({500, 1050}, 0.5_s);
	}
	for (const char* tag : vs::keys(LABELS)) {
		widget& widget{_ui.get(tag)};
		widget.pos.change({-200, vec2{widget.pos}.y}, 0.5_s);
	}
	for (const char* tag : RIGHT_WIDGETS) {
		widget& widget{_ui.get(tag)};
		widget.pos.change({1200, vec2{widget.pos}.y}, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}