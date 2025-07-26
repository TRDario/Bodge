#include "../../include/state/settings_state.hpp"
#include "../../include/audio.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Settings screen right-aligned widgets.
constexpr std::array<const char*, 24> RIGHT_WIDGETS{
	"window_size_dec",
	"cur_window_size",
	"window_size_inc",
	"refresh_rate_dec",
	"cur_refresh_rate",
	"refresh_rate_inc",
	"msaa_dec",
	"cur_msaa",
	"msaa_inc",
	"primary_hue_dec",
	"cur_primary_hue",
	"primary_hue_inc",
	"primary_hue_preview",
	"secondary_hue_dec",
	"cur_secondary_hue",
	"secondary_hue_inc",
	"secondary_hue_preview",
	"sfx_volume_dec",
	"cur_sound_volume",
	"sfx_volume_inc",
	"music_volume_dec",
	"cur_music_volume",
	"music_volume_inc",
	"cur_language",
};

// Sentinel string for having no tooltip.
constexpr const char* NO_TOOLTIP_STR{nullptr};
// Settings screen left-aligned labels.
constexpr std::array<label, 8> LABELS{{
	{"window_size", "window_size_tt"},
	{"refresh_rate", "refresh_rate_tt"},
	{"msaa", "msaa_tt"},
	{"primary_hue", "primary_hue_tt"},
	{"secondary_hue", "secondary_hue_tt"},
	{"sfx_volume", NO_TOOLTIP_STR},
	{"music_volume", NO_TOOLTIP_STR},
	{"language", NO_TOOLTIP_STR},
}};

// Settings screen bottom buttons.
constexpr std::array<const char*, 3> BOTTOM_BUTTONS{"revert", "apply", "exit"};
// Shortcuts of the bottom buttons.
constexpr std::array<std::initializer_list<tr::key_chord>, BOTTOM_BUTTONS.size()> BOTTOM_SHORTCUTS{{
	{{tr::keycode::Z, tr::keymod::CTRL}},
	{{tr::keycode::S, tr::keymod::CTRL}},
	{{tr::keycode::ESCAPE}},
}};

// Starting position of the window size widgets.
constexpr glm::vec2 WINDOW_SIZE_START_POS{1050, 196};
// Starting position of the refresh rate widgets.
constexpr glm::vec2 REFRESH_RATE_START_POS{1050, 271};
// Starting position of the MSAA widgets.
constexpr glm::vec2 MSAA_START_POS{1050, 346};
// Starting position of the primary hue widgets.
constexpr glm::vec2 PRIMARY_HUE_START_POS{1050, 421};
// Starting position of the secondary hue widgets.
constexpr glm::vec2 SECONDARY_HUE_START_POS{1050, 496};
// Starting position of the SFX volume widgets.
constexpr glm::vec2 SFX_VOLUME_START_POS{1050, 571};
// Starting position of the music volume widgets.
constexpr glm::vec2 MUSIC_VOLUME_START_POS{1050, 646};
// Starting position of the language widgets.
constexpr glm::vec2 LANGUAGE_START_POS{1050, 721};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

settings_state::settings_state(std::unique_ptr<game>&& game)
	: _substate{substate::IN_SETTINGS}, _timer{0}, _game{std::move(game)}, _pending{settings}
{
	// STATUS CALLBACKS

	const status_callback window_size_dec_status_cb{[this] {
		return _substate != substate::ENTERING_TITLE && _pending.window_size != FULLSCREEN && _pending.window_size > MIN_WINDOW_SIZE;
	}};
	const status_callback window_size_inc_status_cb{[this] {
		return _substate != substate::ENTERING_TITLE && _pending.window_size != FULLSCREEN && _pending.window_size < max_window_size();
	}};
	const status_callback cur_window_size_status_cb{[this] { return _substate != substate::ENTERING_TITLE; }};
	const status_callback refresh_rate_dec_status_cb{[this] {
		return _substate != substate::ENTERING_TITLE && _pending.refresh_rate != NATIVE_REFRESH_RATE &&
			   _pending.refresh_rate > MIN_REFRESH_RATE;
	}};
	const status_callback refresh_rate_inc_status_cb{[this] {
		return _substate != substate::ENTERING_TITLE && _pending.refresh_rate != NATIVE_REFRESH_RATE &&
			   _pending.refresh_rate < max_refresh_rate();
	}};
	const status_callback cur_refresh_rate_status_cb{[this] { return _substate != substate::ENTERING_TITLE; }};
	const status_callback msaa_dec_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.msaa != NO_MSAA; }};
	const status_callback msaa_inc_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.msaa != tr::max_msaa(); }};
	const status_callback hue_arrow_status_cb{[this] { return _substate != substate::ENTERING_TITLE; }};
	const status_callback sfx_volume_dec_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.sfx_volume > 0; }};
	const status_callback sfx_volume_inc_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.sfx_volume < 100; }};
	const status_callback music_volume_dec_status_cb{[this] { return _substate != substate::ENTERING_TITLE && _pending.music_volume > 0; }};
	const status_callback music_volume_inc_status_cb{
		[this] { return _substate != substate::ENTERING_TITLE && _pending.music_volume < 100; }};
	const status_callback cur_language_status_cb{[this] { return _substate != substate::ENTERING_TITLE && languages.size() > 1; }};
	const std::array<status_callback, BOTTOM_BUTTONS.size()> bottom_status_cbs{
		[this] { return _substate != substate::ENTERING_TITLE && _pending != settings; },
		[this] { return _substate != substate::ENTERING_TITLE && _pending != settings; },
		[this] { return _substate != substate::ENTERING_TITLE && _pending == settings; },
	};

	// ACTION CALLBACKS

	const action_callback window_size_dec_action_cb{[&window_size = _pending.window_size] {
		window_size = std::max(MIN_WINDOW_SIZE, window_size - engine::keymods_choose(1, 10, 100));
	}};
	const action_callback window_size_inc_action_cb{[&window_size = _pending.window_size] {
		window_size = std::min(max_window_size(), window_size + engine::keymods_choose(1, 10, 100));
	}};
	const action_callback cur_window_size_action_cb{[this] {
		if (_pending.window_size == FULLSCREEN) {
			_pending.window_size = 500;
		}
		else {
			_pending.window_size = FULLSCREEN;
		}
		update_window_size_buttons();
	}};
	const action_callback refresh_rate_dec_action_cb{[&refresh_rate = _pending.refresh_rate] {
		refresh_rate = static_cast<std::uint16_t>(std::max(15, refresh_rate - engine::keymods_choose(1, 10, 25)));
	}};
	const action_callback refresh_rate_inc_action_cb{[&refresh_rate = _pending.refresh_rate] {
		refresh_rate = static_cast<std::uint16_t>(std::max(15, refresh_rate + engine::keymods_choose(1, 10, 25)));
	}};
	const action_callback cur_refresh_rate_action_cb{[this] {
		if (_pending.refresh_rate == NATIVE_REFRESH_RATE) {
			_pending.refresh_rate = max_refresh_rate();
		}
		else {
			_pending.refresh_rate = NATIVE_REFRESH_RATE;
		}
		update_refresh_rate_buttons();
	}};
	const action_callback msaa_dec_action_cb{[&msaa = _pending.msaa] { msaa = msaa == 2 ? NO_MSAA : static_cast<std::uint8_t>(msaa / 2); }};
	const action_callback msaa_inc_action_cb{[&msaa = _pending.msaa] { msaa = msaa == NO_MSAA ? 2 : static_cast<std::uint8_t>(msaa * 2); }};
	const action_callback primary_hue_dec_action_cb{[&primary_hue = _pending.primary_hue] {
		primary_hue = static_cast<std::uint16_t>((primary_hue - engine::keymods_choose(1, 10, 100) + 360) % 360);
	}};
	const action_callback primary_hue_inc_action_cb{[&primary_hue = _pending.primary_hue] {
		primary_hue = static_cast<std::uint16_t>((primary_hue + engine::keymods_choose(1, 10, 100)) % 360);
	}};
	const action_callback secondary_hue_dec_action_cb{[&secondary_hue = _pending.secondary_hue] {
		secondary_hue = static_cast<std::uint16_t>((secondary_hue - engine::keymods_choose(1, 10, 100) + 360) % 360);
	}};
	const action_callback secondary_hue_inc_action_cb{[&secondary_hue = _pending.secondary_hue] {
		secondary_hue = static_cast<std::uint16_t>((secondary_hue + engine::keymods_choose(1, 10, 100)) % 360);
	}};
	const action_callback sfx_volume_dec_action_cb{[&sfx_volume = _pending.sfx_volume] {
		sfx_volume = static_cast<std::uint8_t>(std::max(sfx_volume - engine::keymods_choose(1, 10, 25), 0));
	}};
	const action_callback sfx_volume_inc_action_cb{[&sfx_volume = _pending.sfx_volume] {
		sfx_volume = static_cast<std::uint8_t>(std::min(sfx_volume + engine::keymods_choose(1, 10, 25), 100));
	}};
	const action_callback music_volume_dec_action_cb{[&music_volume = _pending.music_volume] {
		music_volume = static_cast<std::uint8_t>(std::max(music_volume - engine::keymods_choose(1, 10, 25), 0));
	}};
	const action_callback music_volume_inc_action_cb{[&music_volume = _pending.music_volume] {
		music_volume = static_cast<std::uint8_t>(std::min(music_volume + engine::keymods_choose(1, 10, 25), 100));
	}};
	const action_callback cur_language_action_cb{[this] {
		std::map<language_code, language>::iterator it{std::next(languages.find(_pending.language))};
		if (it == languages.end()) {
			it = languages.begin();
		}
		_pending.language = it->first;
		font_manager.reload_language_preview_font(_pending);
	}};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_action_cbs{
		[this] {
			_pending = settings;
			font_manager.reload_language_preview_font(_pending);
			update_window_size_buttons();
			update_refresh_rate_buttons();
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
			_substate = substate::ENTERING_TITLE;
			_timer = 0;
			set_up_exit_animation();
		},
	};

	// TEXT CALLBACKS

	const text_callback cur_window_size_text_cb{[&window_size = _pending.window_size](auto&) {
		return window_size == FULLSCREEN ? std::string{localization["fullscreen"]} : std::to_string(window_size);
	}};
	const text_callback cur_refresh_rate_text_cb{[&refresh_rate = _pending.refresh_rate](auto&) {
		return refresh_rate == NATIVE_REFRESH_RATE ? std::string{localization["native"]} : std::to_string(refresh_rate);
	}};
	const text_callback cur_msaa_text_cb{[this](auto&) { return _pending.msaa == NO_MSAA ? "--" : std::format("x{}", _pending.msaa); }};
	const text_callback cur_primary_hue_text_cb{[this](auto&) { return std::to_string(_pending.primary_hue); }};
	const text_callback cur_secondary_hue_text_cb{[this](auto&) { return std::to_string(_pending.secondary_hue); }};
	const text_callback cur_sound_volume_text_cb{[this](auto&) { return std::format("{}%", _pending.sfx_volume); }};
	const text_callback cur_music_volume_text_cb{[this](auto&) { return std::format("{}%", _pending.music_volume); }};
	const text_callback cur_language_text_cb{
		[this](auto&) { return languages.contains(_pending.language) ? languages[_pending.language].name : "???"; }};

	//

	widget& title{_ui.emplace<text_widget>("settings", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change(TITLE_POS, 0.5_s);
	title.unhide(0.5_s);

	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const label& label{LABELS[i]};
		const glm::vec2 pos{-50, 196 + i * 75};
		widget& widget{label.tooltip != NO_TOOLTIP_STR
						   ? _ui.emplace<text_widget>(label.tag, pos, tr::align::CENTER_LEFT, LABELS[i].tooltip, font::LANGUAGE,
													  tr::ttf_style::NORMAL, 48)
						   : _ui.emplace<text_widget>(label.tag, pos, tr::align::CENTER_LEFT, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
		widget.pos.change({15, 196 + i * 75}, 0.5_s);
		widget.unhide(0.5_s);
	}

	const tr::align cur_window_size_alignment{settings.window_size == FULLSCREEN ? tr::align::CENTER_RIGHT : tr::align::CENTER};
	widget& window_size_dec{_ui.emplace<arrow_widget>("window_size_dec", WINDOW_SIZE_START_POS, tr::align::CENTER_LEFT, false,
													  window_size_dec_status_cb, window_size_dec_action_cb)};
	widget& window_size_inc{_ui.emplace<arrow_widget>("window_size_inc", WINDOW_SIZE_START_POS, tr::align::CENTER_RIGHT, true,
													  window_size_inc_status_cb, window_size_inc_action_cb)};
	widget& cur_window_size{_ui.emplace<clickable_text_widget>("cur_window_size", WINDOW_SIZE_START_POS, cur_window_size_alignment,
															   font::LANGUAGE, 48, cur_window_size_text_cb, cur_window_size_status_cb,
															   cur_window_size_action_cb)};
	cur_window_size.pos.change({settings.window_size == FULLSCREEN ? 985 : 875, WINDOW_SIZE_START_POS.y}, 0.5_s);
	cur_window_size.unhide(0.5_s);
	window_size_dec.pos.change({765, WINDOW_SIZE_START_POS.y}, 0.5_s);
	window_size_inc.pos.change({985, WINDOW_SIZE_START_POS.y}, 0.5_s);
	if (_pending.window_size != FULLSCREEN) {
		window_size_dec.unhide(0.5_s);
		window_size_inc.unhide(0.5_s);
	}

	const tr::align cur_refresh_rate_alignment{settings.refresh_rate == NATIVE_REFRESH_RATE ? tr::align::CENTER_RIGHT : tr::align::CENTER};
	widget& refresh_rate_dec{_ui.emplace<arrow_widget>("refresh_rate_dec", REFRESH_RATE_START_POS, tr::align::CENTER_LEFT, false,
													   refresh_rate_dec_status_cb, refresh_rate_dec_action_cb)};
	widget& refresh_rate_inc{_ui.emplace<arrow_widget>("refresh_rate_inc", REFRESH_RATE_START_POS, tr::align::CENTER_RIGHT, true,
													   refresh_rate_inc_status_cb, refresh_rate_inc_action_cb)};
	widget& cur_refresh_rate{_ui.emplace<clickable_text_widget>("cur_refresh_rate", REFRESH_RATE_START_POS, cur_refresh_rate_alignment,
																font::LANGUAGE, 48, cur_refresh_rate_text_cb, cur_refresh_rate_status_cb,
																cur_refresh_rate_action_cb)};
	cur_refresh_rate.pos.change({settings.refresh_rate == NATIVE_REFRESH_RATE ? 985 : 892.5, REFRESH_RATE_START_POS.y}, 0.5_s);
	cur_refresh_rate.unhide(0.5_s);
	refresh_rate_dec.pos.change({800, REFRESH_RATE_START_POS.y}, 0.5_s);
	refresh_rate_inc.pos.change({985, REFRESH_RATE_START_POS.y}, 0.5_s);
	if (_pending.refresh_rate != NATIVE_REFRESH_RATE) {
		refresh_rate_dec.unhide(0.5_s);
		refresh_rate_inc.unhide(0.5_s);
	}

	widget& msaa_dec{
		_ui.emplace<arrow_widget>("msaa_dec", MSAA_START_POS, tr::align::CENTER_LEFT, false, msaa_dec_status_cb, msaa_dec_action_cb)};
	widget& msaa_inc{
		_ui.emplace<arrow_widget>("msaa_inc", MSAA_START_POS, tr::align::CENTER_RIGHT, true, msaa_inc_status_cb, msaa_inc_action_cb)};
	widget& cur_msaa{_ui.emplace<text_widget>("cur_msaa", MSAA_START_POS, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 48,
											  cur_msaa_text_cb)};
	msaa_dec.pos.change({830, MSAA_START_POS.y}, 0.5_s);
	msaa_inc.pos.change({985, MSAA_START_POS.y}, 0.5_s);
	cur_msaa.pos.change({907.5, MSAA_START_POS.y}, 0.5_s);
	msaa_dec.unhide(0.5_s);
	msaa_inc.unhide(0.5_s);
	cur_msaa.unhide(0.5_s);

	widget& primary_hue_dec{_ui.emplace<arrow_widget>("primary_hue_dec", PRIMARY_HUE_START_POS, tr::align::CENTER_LEFT, false,
													  hue_arrow_status_cb, primary_hue_dec_action_cb)};
	widget& primary_hue_inc{_ui.emplace<arrow_widget>("primary_hue_inc", PRIMARY_HUE_START_POS, tr::align::CENTER_RIGHT, true,
													  hue_arrow_status_cb, primary_hue_inc_action_cb)};
	widget& cur_primary_hue{_ui.emplace<text_widget>("cur_primary_hue", PRIMARY_HUE_START_POS, tr::align::CENTER, font::LANGUAGE,
													 tr::ttf_style::NORMAL, 48, cur_primary_hue_text_cb)};
	widget& primary_hue_preview{
		_ui.emplace<color_preview_widget>("primary_hue_preview", PRIMARY_HUE_START_POS, tr::align::CENTER_RIGHT, _pending.primary_hue)};
	primary_hue_dec.pos.change({745, PRIMARY_HUE_START_POS.y}, 0.5_s);
	primary_hue_inc.pos.change({930, PRIMARY_HUE_START_POS.y}, 0.5_s);
	cur_primary_hue.pos.change({837.5, PRIMARY_HUE_START_POS.y}, 0.5_s);
	primary_hue_preview.pos.change({985, PRIMARY_HUE_START_POS.y}, 0.5_s);
	primary_hue_dec.unhide(0.5_s);
	primary_hue_inc.unhide(0.5_s);
	cur_primary_hue.unhide(0.5_s);
	primary_hue_preview.unhide(0.5_s);

	widget& secondary_hue_dec{_ui.emplace<arrow_widget>("secondary_hue_dec", SECONDARY_HUE_START_POS, tr::align::CENTER_LEFT, false,
														hue_arrow_status_cb, secondary_hue_dec_action_cb)};
	widget& secondary_hue_inc{_ui.emplace<arrow_widget>("secondary_hue_inc", SECONDARY_HUE_START_POS, tr::align::CENTER_RIGHT, true,
														hue_arrow_status_cb, secondary_hue_inc_action_cb)};
	widget& cur_secondary_hue{_ui.emplace<text_widget>("cur_secondary_hue", SECONDARY_HUE_START_POS, tr::align::CENTER, font::LANGUAGE,
													   tr::ttf_style::NORMAL, 48, cur_secondary_hue_text_cb)};
	widget& secondary_hue_preview{_ui.emplace<color_preview_widget>("secondary_hue_preview", SECONDARY_HUE_START_POS,
																	tr::align::CENTER_RIGHT, _pending.secondary_hue)};
	secondary_hue_dec.pos.change({745, SECONDARY_HUE_START_POS.y}, 0.5_s);
	secondary_hue_inc.pos.change({930, SECONDARY_HUE_START_POS.y}, 0.5_s);
	cur_secondary_hue.pos.change({837.5, SECONDARY_HUE_START_POS.y}, 0.5_s);
	secondary_hue_preview.pos.change({985, SECONDARY_HUE_START_POS.y}, 0.5_s);
	secondary_hue_dec.unhide(0.5_s);
	secondary_hue_inc.unhide(0.5_s);
	cur_secondary_hue.unhide(0.5_s);
	secondary_hue_preview.unhide(0.5_s);

	widget& sfx_volume_dec{_ui.emplace<arrow_widget>("sfx_volume_dec", SFX_VOLUME_START_POS, tr::align::CENTER_LEFT, false,
													 sfx_volume_dec_status_cb, sfx_volume_dec_action_cb)};
	widget& sfx_volume_inc{_ui.emplace<arrow_widget>("sfx_volume_inc", SFX_VOLUME_START_POS, tr::align::CENTER_RIGHT, true,
													 sfx_volume_inc_status_cb, sfx_volume_inc_action_cb)};
	widget& cur_sound_volume{_ui.emplace<text_widget>("cur_sound_volume", SFX_VOLUME_START_POS, tr::align::CENTER, font::LANGUAGE,
													  tr::ttf_style::NORMAL, 48, cur_sound_volume_text_cb)};
	sfx_volume_dec.pos.change({765, SFX_VOLUME_START_POS.y}, 0.5_s);
	sfx_volume_inc.pos.change({985, SFX_VOLUME_START_POS.y}, 0.5_s);
	cur_sound_volume.pos.change({875, SFX_VOLUME_START_POS.y}, 0.5_s);
	sfx_volume_dec.unhide(0.5_s);
	sfx_volume_inc.unhide(0.5_s);
	cur_sound_volume.unhide(0.5_s);

	widget& music_volume_dec{_ui.emplace<arrow_widget>("music_volume_dec", MUSIC_VOLUME_START_POS, tr::align::CENTER_LEFT, false,
													   music_volume_dec_status_cb, music_volume_dec_action_cb)};
	widget& music_volume_inc{_ui.emplace<arrow_widget>("music_volume_inc", MUSIC_VOLUME_START_POS, tr::align::CENTER_RIGHT, true,
													   music_volume_inc_status_cb, music_volume_inc_action_cb)};
	widget& cur_music_volume{_ui.emplace<text_widget>("cur_music_volume", MUSIC_VOLUME_START_POS, tr::align::CENTER, font::LANGUAGE,
													  tr::ttf_style::NORMAL, 48, cur_music_volume_text_cb)};
	music_volume_dec.pos.change({765, MUSIC_VOLUME_START_POS.y}, 0.5_s);
	music_volume_inc.pos.change({985, MUSIC_VOLUME_START_POS.y}, 0.5_s);
	cur_music_volume.pos.change({875, MUSIC_VOLUME_START_POS.y}, 0.5_s);
	music_volume_dec.unhide(0.5_s);
	music_volume_inc.unhide(0.5_s);
	cur_music_volume.unhide(0.5_s);

	widget& cur_language{_ui.emplace<clickable_text_widget>("cur_language", LANGUAGE_START_POS, tr::align::CENTER_RIGHT,
															font::LANGUAGE_PREVIEW, 48, cur_language_text_cb, cur_language_status_cb,
															cur_language_action_cb)};
	cur_language.pos.change({985, LANGUAGE_START_POS.y}, 0.5_s);
	cur_language.unhide(0.5_s);

	for (std::size_t i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sound sound{i == 1 ? sound::CONFIRM : sound::CANCEL};
		widget& widget{_ui.emplace<clickable_text_widget>(BOTTOM_BUTTONS[i], BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
														  DEFAULT_TEXT_CALLBACK, bottom_status_cbs[i], bottom_action_cbs[i], NO_TOOLTIP,
														  BOTTOM_SHORTCUTS[i], sound)};
		widget.pos.change({500, 1000 - 50 * BOTTOM_BUTTONS.size() + (i + 1) * 50}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> settings_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> settings_state::update(tr::duration)
{
	++_timer;
	_game->update({});
	_ui.update();

	switch (_substate) {
	case substate::IN_SETTINGS:
		return nullptr;
	case substate::ENTERING_TITLE:
		return _timer >= 0.5_s ? std::make_unique<title_state>(std::move(_game)) : nullptr;
	}
}

void settings_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void settings_state::update_window_size_buttons() noexcept
{
	widget& cur_window_size{_ui.get("cur_window_size")};
	if (_pending.window_size == FULLSCREEN) {
		cur_window_size.pos = glm::vec2{985, 196};
		cur_window_size.alignment = tr::align::CENTER_RIGHT;
		_ui.get("window_size_dec").hide();
		_ui.get("window_size_inc").hide();
	}
	else {
		cur_window_size.pos = glm::vec2{875, 196};
		cur_window_size.alignment = tr::align::CENTER;
		_ui.get("window_size_dec").unhide();
		_ui.get("window_size_inc").unhide();
	}
}

void settings_state::update_refresh_rate_buttons() noexcept
{
	widget& cur_refresh_rate{_ui.get("cur_refresh_rate")};
	if (_pending.refresh_rate == NATIVE_REFRESH_RATE) {
		cur_refresh_rate.pos = glm::vec2{985, 271};
		cur_refresh_rate.alignment = tr::align::CENTER_RIGHT;
		_ui.get("refresh_rate_dec").hide();
		_ui.get("refresh_rate_inc").hide();
	}
	else {
		cur_refresh_rate.pos = glm::vec2{892.5, 271};
		cur_refresh_rate.alignment = tr::align::CENTER;
		_ui.get("refresh_rate_dec").unhide();
		_ui.get("refresh_rate_inc").unhide();
	}
}

void settings_state::set_up_exit_animation() noexcept
{
	_ui.get("settings").pos.change(TOP_START_POS, 0.5_s);
	for (const char* tag : BOTTOM_BUTTONS) {
		_ui.get(tag).pos.change(BOTTOM_START_POS, 0.5_s);
	}
	for (const char* tag : tr::project(LABELS, &label::tag)) {
		widget& widget{_ui.get(tag)};
		widget.pos.change({-50, glm::vec2{widget.pos}.y}, 0.5_s);
	}
	for (const char* tag : RIGHT_WIDGETS) {
		widget& widget{_ui.get(tag)};
		widget.pos.change({1050, glm::vec2{widget.pos}.y}, 0.5_s);
	}
	_ui.hide_all(0.5_s);
}