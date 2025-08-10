#include "../../include/state/settings_state.hpp"
#include "../../include/audio.hpp"
#include "../../include/state/title_state.hpp"
#include "../../include/system.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag TAG_TITLE{"settings"};
constexpr tag TAG_WINDOW_SIZE{"window_size"};
constexpr tag TAG_WINDOW_SIZE_DEC{"window_size_dec"};
constexpr tag TAG_CUR_WINDOW_SIZE{"cur_window_size"};
constexpr tag TAG_WINDOW_SIZE_INC{"window_size_inc"};
constexpr tag TAG_REFRESH_RATE{"refresh_rate"};
constexpr tag TAG_REFRESH_RATE_DEC{"refresh_rate_dec"};
constexpr tag TAG_CUR_REFRESH_RATE{"cur_refresh_rate"};
constexpr tag TAG_REFRESH_RATE_INC{"refresh_rate_inc"};
constexpr tag TAG_MSAA{"msaa"};
constexpr tag TAG_MSAA_DEC{"msaa_dec"};
constexpr tag TAG_CUR_MSAA{"cur_msaa"};
constexpr tag TAG_MSAA_INC{"msaa_inc"};
constexpr tag TAG_PRIMARY_HUE{"primary_hue"};
constexpr tag TAG_PRIMARY_HUE_PREVIEW{"primary_hue_preview"};
constexpr tag TAG_PRIMARY_HUE_DEC{"primary_hue_dec"};
constexpr tag TAG_CUR_PRIMARY_HUE{"cur_primary_hue"};
constexpr tag TAG_PRIMARY_HUE_INC{"primary_hue_inc"};
constexpr tag TAG_SECONDARY_HUE{"secondary_hue"};
constexpr tag TAG_SECONDARY_HUE_PREVIEW{"secondary_hue_preview"};
constexpr tag TAG_SECONDARY_HUE_DEC{"secondary_hue_dec"};
constexpr tag TAG_CUR_SECONDARY_HUE{"cur_secondary_hue"};
constexpr tag TAG_SECONDARY_HUE_INC{"secondary_hue_inc"};
constexpr tag TAG_SFX_VOLUME{"sfx_volume"};
constexpr tag TAG_SFX_VOLUME_DEC{"sfx_volume_dec"};
constexpr tag TAG_CUR_SFX_VOLUME{"cur_sfx_volume"};
constexpr tag TAG_SFX_VOLUME_INC{"sfx_volume_inc"};
constexpr tag TAG_MUSIC_VOLUME{"music_volume"};
constexpr tag TAG_MUSIC_VOLUME_DEC{"music_volume_dec"};
constexpr tag TAG_CUR_MUSIC_VOLUME{"cur_music_volume"};
constexpr tag TAG_MUSIC_VOLUME_INC{"music_volume_inc"};
constexpr tag TAG_LANGUAGE{"language"};
constexpr tag TAG_CUR_LANGUAGE{"cur_language"};
constexpr tag TAG_REVERT{"revert"};
constexpr tag TAG_APPLY{"apply"};
constexpr tag TAG_EXIT{"exit"};

// Right-aligned widgets.
constexpr std::array<const char*, 24> RIGHT_WIDGETS{
	TAG_WINDOW_SIZE_DEC,
	TAG_CUR_WINDOW_SIZE,
	TAG_WINDOW_SIZE_INC,
	TAG_REFRESH_RATE_DEC,
	TAG_CUR_REFRESH_RATE,
	TAG_REFRESH_RATE_INC,
	TAG_MSAA_DEC,
	TAG_CUR_MSAA,
	TAG_MSAA_INC,
	TAG_PRIMARY_HUE_DEC,
	TAG_CUR_PRIMARY_HUE,
	TAG_PRIMARY_HUE_INC,
	TAG_PRIMARY_HUE_PREVIEW,
	TAG_SECONDARY_HUE_DEC,
	TAG_CUR_SECONDARY_HUE,
	TAG_SECONDARY_HUE_INC,
	TAG_SECONDARY_HUE_PREVIEW,
	TAG_SFX_VOLUME_DEC,
	TAG_CUR_SFX_VOLUME,
	TAG_SFX_VOLUME_INC,
	TAG_MUSIC_VOLUME_DEC,
	TAG_CUR_MUSIC_VOLUME,
	TAG_MUSIC_VOLUME_INC,
	TAG_CUR_LANGUAGE,
};
// Bottom set of buttons.
constexpr std::array<const char*, 3> BOTTOM_BUTTONS{TAG_REVERT, TAG_APPLY, TAG_EXIT};

// Sentinel string for having no tooltip.
constexpr const char* NO_TOOLTIP_STR{nullptr};
// Left-aligned label widgets.
constexpr std::array<label, 8> LABELS{{
	{TAG_WINDOW_SIZE, "window_size_tt"},
	{TAG_REFRESH_RATE, "refresh_rate_tt"},
	{TAG_MSAA, "msaa_tt"},
	{TAG_PRIMARY_HUE, "primary_hue_tt"},
	{TAG_SECONDARY_HUE, "secondary_hue_tt"},
	{TAG_SFX_VOLUME, NO_TOOLTIP_STR},
	{TAG_MUSIC_VOLUME, NO_TOOLTIP_STR},
	{TAG_LANGUAGE, NO_TOOLTIP_STR},
}};

// Shortcuts of the bottom buttons.
constexpr std::array<std::initializer_list<tr::system::key_chord>, BOTTOM_BUTTONS.size()> BOTTOM_SHORTCUTS{{
	{{tr::system::keycode::Z, tr::system::keymod::CTRL}},
	{{tr::system::keycode::S, tr::system::keymod::CTRL}},
	{{tr::system::keycode::ESCAPE}},
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
	: m_substate{substate::IN_SETTINGS}, m_timer{0}, m_background_game{std::move(game)}, m_pending{engine::settings}
{
	// STATUS CALLBACKS

	const status_callback window_size_dec_status_cb{[this] {
		return m_substate != substate::ENTERING_TITLE && m_pending.window_size != FULLSCREEN && m_pending.window_size > MIN_WINDOW_SIZE;
	}};
	const status_callback window_size_inc_status_cb{[this] {
		return m_substate != substate::ENTERING_TITLE && m_pending.window_size != FULLSCREEN && m_pending.window_size < max_window_size();
	}};
	const status_callback cur_window_size_status_cb{[this] { return m_substate != substate::ENTERING_TITLE; }};
	const status_callback refresh_rate_dec_status_cb{[this] {
		return m_substate != substate::ENTERING_TITLE && m_pending.refresh_rate != NATIVE_REFRESH_RATE &&
			   m_pending.refresh_rate > MIN_REFRESH_RATE;
	}};
	const status_callback refresh_rate_inc_status_cb{[this] {
		return m_substate != substate::ENTERING_TITLE && m_pending.refresh_rate != NATIVE_REFRESH_RATE &&
			   m_pending.refresh_rate < max_refresh_rate();
	}};
	const status_callback cur_refresh_rate_status_cb{[this] { return m_substate != substate::ENTERING_TITLE; }};
	const status_callback msaa_dec_status_cb{[this] { return m_substate != substate::ENTERING_TITLE && m_pending.msaa != NO_MSAA; }};
	const status_callback msaa_inc_status_cb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.msaa != tr::system::max_msaa(); }};
	const status_callback hue_arrow_status_cb{[this] { return m_substate != substate::ENTERING_TITLE; }};
	const status_callback sfx_volume_dec_status_cb{[this] { return m_substate != substate::ENTERING_TITLE && m_pending.sfx_volume > 0; }};
	const status_callback sfx_volume_inc_status_cb{[this] { return m_substate != substate::ENTERING_TITLE && m_pending.sfx_volume < 100; }};
	const status_callback music_volume_dec_status_cb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.music_volume > 0; }};
	const status_callback music_volume_inc_status_cb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.music_volume < 100; }};
	const status_callback cur_language_status_cb{[this] { return m_substate != substate::ENTERING_TITLE && engine::languages.size() > 1; }};
	const std::array<status_callback, BOTTOM_BUTTONS.size()> bottom_status_cbs{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending != engine::settings; },
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending != engine::settings; },
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending == engine::settings; },
	};

	// ACTION CALLBACKS

	const action_callback window_size_dec_action_cb{[&window_size = m_pending.window_size] {
		window_size = std::max(MIN_WINDOW_SIZE, window_size - engine::keymods_choose(1, 10, 100));
	}};
	const action_callback window_size_inc_action_cb{[&window_size = m_pending.window_size] {
		window_size = std::min(max_window_size(), window_size + engine::keymods_choose(1, 10, 100));
	}};
	const action_callback cur_window_size_action_cb{[this] {
		if (m_pending.window_size == FULLSCREEN) {
			m_pending.window_size = 500;
		}
		else {
			m_pending.window_size = FULLSCREEN;
		}
		update_window_size_buttons();
	}};
	const action_callback refresh_rate_dec_action_cb{[&refresh_rate = m_pending.refresh_rate] {
		refresh_rate = static_cast<std::uint16_t>(std::max(15, refresh_rate - engine::keymods_choose(1, 10, 25)));
	}};
	const action_callback refresh_rate_inc_action_cb{[&refresh_rate = m_pending.refresh_rate] {
		refresh_rate = static_cast<std::uint16_t>(std::max(15, refresh_rate + engine::keymods_choose(1, 10, 25)));
	}};
	const action_callback cur_refresh_rate_action_cb{[this] {
		if (m_pending.refresh_rate == NATIVE_REFRESH_RATE) {
			m_pending.refresh_rate = max_refresh_rate();
		}
		else {
			m_pending.refresh_rate = NATIVE_REFRESH_RATE;
		}
		update_refresh_rate_buttons();
	}};
	const action_callback msaa_dec_action_cb{
		[&msaa = m_pending.msaa] { msaa = msaa == 2 ? NO_MSAA : static_cast<std::uint8_t>(msaa / 2); }};
	const action_callback msaa_inc_action_cb{
		[&msaa = m_pending.msaa] { msaa = msaa == NO_MSAA ? 2 : static_cast<std::uint8_t>(msaa * 2); }};
	const action_callback primary_hue_dec_action_cb{[&primary_hue = m_pending.primary_hue] {
		primary_hue = static_cast<std::uint16_t>((primary_hue - engine::keymods_choose(1, 10, 100) + 360) % 360);
	}};
	const action_callback primary_hue_inc_action_cb{[&primary_hue = m_pending.primary_hue] {
		primary_hue = static_cast<std::uint16_t>((primary_hue + engine::keymods_choose(1, 10, 100)) % 360);
	}};
	const action_callback secondary_hue_dec_action_cb{[&secondary_hue = m_pending.secondary_hue] {
		secondary_hue = static_cast<std::uint16_t>((secondary_hue - engine::keymods_choose(1, 10, 100) + 360) % 360);
	}};
	const action_callback secondary_hue_inc_action_cb{[&secondary_hue = m_pending.secondary_hue] {
		secondary_hue = static_cast<std::uint16_t>((secondary_hue + engine::keymods_choose(1, 10, 100)) % 360);
	}};
	const action_callback sfx_volume_dec_action_cb{[&sfx_volume = m_pending.sfx_volume] {
		sfx_volume = static_cast<std::uint8_t>(std::max(sfx_volume - engine::keymods_choose(1, 10, 25), 0));
	}};
	const action_callback sfx_volume_inc_action_cb{[&sfx_volume = m_pending.sfx_volume] {
		sfx_volume = static_cast<std::uint8_t>(std::min(sfx_volume + engine::keymods_choose(1, 10, 25), 100));
	}};
	const action_callback music_volume_dec_action_cb{[&music_volume = m_pending.music_volume] {
		music_volume = static_cast<std::uint8_t>(std::max(music_volume - engine::keymods_choose(1, 10, 25), 0));
	}};
	const action_callback music_volume_inc_action_cb{[&music_volume = m_pending.music_volume] {
		music_volume = static_cast<std::uint8_t>(std::min(music_volume + engine::keymods_choose(1, 10, 25), 100));
	}};
	const action_callback cur_language_action_cb{[this] {
		std::map<language_code, language>::iterator it{std::next(engine::languages.find(m_pending.language))};
		if (it == engine::languages.end()) {
			it = engine::languages.begin();
		}
		m_pending.language = it->first;
		engine::reload_language_preview_font(m_pending);
	}};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_action_cbs{
		[this] {
			m_pending = engine::settings;
			engine::reload_language_preview_font(m_pending);
			update_window_size_buttons();
			update_refresh_rate_buttons();
		},
		[this] {
			const settings old{engine::settings};
			engine::settings = m_pending;
			if (old.window_size != engine::settings.window_size || old.msaa != engine::settings.msaa) {
				m_ui.release_graphical_resources();
			}
			if (old.language != engine::settings.language) {
				engine::load_localization();
			}
			if (engine::languages[old.language].font != engine::languages[engine::settings.language].font) {
				m_ui.release_graphical_resources();
				engine::set_language_font();
			}
			engine::apply_settings(old);
		},
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			set_up_exit_animation();
		},
	};

	// TEXT CALLBACKS

	const text_callback cur_window_size_text_cb{[&window_size = m_pending.window_size](auto&) {
		return window_size == FULLSCREEN ? std::string{engine::loc["fullscreen"]} : std::to_string(window_size);
	}};
	const text_callback cur_refresh_rate_text_cb{[&refresh_rate = m_pending.refresh_rate](auto&) {
		return refresh_rate == NATIVE_REFRESH_RATE ? std::string{engine::loc["native"]} : std::to_string(refresh_rate);
	}};
	const text_callback cur_msaa_text_cb{[this](auto&) { return m_pending.msaa == NO_MSAA ? "--" : std::format("x{}", m_pending.msaa); }};
	const text_callback cur_primary_hue_text_cb{[this](auto&) { return std::to_string(m_pending.primary_hue); }};
	const text_callback cur_secondary_hue_text_cb{[this](auto&) { return std::to_string(m_pending.secondary_hue); }};
	const text_callback cur_sound_volume_text_cb{[this](auto&) { return std::format("{}%", m_pending.sfx_volume); }};
	const text_callback cur_music_volume_text_cb{[this](auto&) { return std::format("{}%", m_pending.music_volume); }};
	const text_callback cur_language_text_cb{
		[this](auto&) { return engine::languages.contains(m_pending.language) ? engine::languages[m_pending.language].name : "???"; }};

	//

	widget& title{
		m_ui.emplace<text_widget>(TAG_TITLE, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL, 64)};
	title.pos.change(interp_mode::CUBE, TITLE_POS, 0.5_s);
	title.unhide(0.5_s);

	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const label& label{LABELS[i]};
		const glm::vec2 pos{-50, 196 + i * 75};
		widget& widget{
			label.tooltip != NO_TOOLTIP_STR
				? m_ui.emplace<text_widget>(label.tag, pos, tr::align::CENTER_LEFT, LABELS[i].tooltip, font::LANGUAGE,
											tr::system::ttf_style::NORMAL, 48)
				: m_ui.emplace<text_widget>(label.tag, pos, tr::align::CENTER_LEFT, font::LANGUAGE, tr::system::ttf_style::NORMAL, 48)};
		widget.pos.change(interp_mode::CUBE, {15, 196 + i * 75}, 0.5_s);
		widget.unhide(0.5_s);
	}

	const tr::align cur_window_size_alignment{engine::settings.window_size == FULLSCREEN ? tr::align::CENTER_RIGHT : tr::align::CENTER};
	widget& window_size_dec{m_ui.emplace<arrow_widget>(TAG_WINDOW_SIZE_DEC, WINDOW_SIZE_START_POS, tr::align::CENTER_LEFT, false,
													   window_size_dec_status_cb, window_size_dec_action_cb)};
	widget& window_size_inc{m_ui.emplace<arrow_widget>(TAG_WINDOW_SIZE_INC, WINDOW_SIZE_START_POS, tr::align::CENTER_RIGHT, true,
													   window_size_inc_status_cb, window_size_inc_action_cb)};
	widget& cur_window_size{m_ui.emplace<clickable_text_widget>(TAG_CUR_WINDOW_SIZE, WINDOW_SIZE_START_POS, cur_window_size_alignment,
																font::LANGUAGE, 48, cur_window_size_text_cb, cur_window_size_status_cb,
																cur_window_size_action_cb)};
	cur_window_size.pos.change(interp_mode::CUBE, {engine::settings.window_size == FULLSCREEN ? 985 : 875, WINDOW_SIZE_START_POS.y}, 0.5_s);
	cur_window_size.unhide(0.5_s);
	window_size_dec.pos.change(interp_mode::CUBE, {765, WINDOW_SIZE_START_POS.y}, 0.5_s);
	window_size_inc.pos.change(interp_mode::CUBE, {985, WINDOW_SIZE_START_POS.y}, 0.5_s);
	if (m_pending.window_size != FULLSCREEN) {
		window_size_dec.unhide(0.5_s);
		window_size_inc.unhide(0.5_s);
	}

	const tr::align cur_refresh_rate_alignment{engine::settings.refresh_rate == NATIVE_REFRESH_RATE ? tr::align::CENTER_RIGHT
																									: tr::align::CENTER};
	widget& refresh_rate_dec{m_ui.emplace<arrow_widget>(TAG_REFRESH_RATE_DEC, REFRESH_RATE_START_POS, tr::align::CENTER_LEFT, false,
														refresh_rate_dec_status_cb, refresh_rate_dec_action_cb)};
	widget& refresh_rate_inc{m_ui.emplace<arrow_widget>(TAG_REFRESH_RATE_INC, REFRESH_RATE_START_POS, tr::align::CENTER_RIGHT, true,
														refresh_rate_inc_status_cb, refresh_rate_inc_action_cb)};
	widget& cur_refresh_rate{m_ui.emplace<clickable_text_widget>(TAG_CUR_REFRESH_RATE, REFRESH_RATE_START_POS, cur_refresh_rate_alignment,
																 font::LANGUAGE, 48, cur_refresh_rate_text_cb, cur_refresh_rate_status_cb,
																 cur_refresh_rate_action_cb)};
	cur_refresh_rate.pos.change(interp_mode::CUBE,
								{engine::settings.refresh_rate == NATIVE_REFRESH_RATE ? 985 : 892.5, REFRESH_RATE_START_POS.y}, 0.5_s);
	cur_refresh_rate.unhide(0.5_s);
	refresh_rate_dec.pos.change(interp_mode::CUBE, {800, REFRESH_RATE_START_POS.y}, 0.5_s);
	refresh_rate_inc.pos.change(interp_mode::CUBE, {985, REFRESH_RATE_START_POS.y}, 0.5_s);
	if (m_pending.refresh_rate != NATIVE_REFRESH_RATE) {
		refresh_rate_dec.unhide(0.5_s);
		refresh_rate_inc.unhide(0.5_s);
	}

	widget& msaa_dec{
		m_ui.emplace<arrow_widget>(TAG_MSAA_DEC, MSAA_START_POS, tr::align::CENTER_LEFT, false, msaa_dec_status_cb, msaa_dec_action_cb)};
	widget& msaa_inc{
		m_ui.emplace<arrow_widget>(TAG_MSAA_INC, MSAA_START_POS, tr::align::CENTER_RIGHT, true, msaa_inc_status_cb, msaa_inc_action_cb)};
	widget& cur_msaa{m_ui.emplace<text_widget>(TAG_CUR_MSAA, MSAA_START_POS, tr::align::CENTER, font::LANGUAGE,
											   tr::system::ttf_style::NORMAL, 48, cur_msaa_text_cb)};
	msaa_dec.pos.change(interp_mode::CUBE, {830, MSAA_START_POS.y}, 0.5_s);
	msaa_inc.pos.change(interp_mode::CUBE, {985, MSAA_START_POS.y}, 0.5_s);
	cur_msaa.pos.change(interp_mode::CUBE, {907.5, MSAA_START_POS.y}, 0.5_s);
	msaa_dec.unhide(0.5_s);
	msaa_inc.unhide(0.5_s);
	cur_msaa.unhide(0.5_s);

	widget& primary_hue_dec{m_ui.emplace<arrow_widget>(TAG_PRIMARY_HUE_DEC, PRIMARY_HUE_START_POS, tr::align::CENTER_LEFT, false,
													   hue_arrow_status_cb, primary_hue_dec_action_cb)};
	widget& primary_hue_inc{m_ui.emplace<arrow_widget>(TAG_PRIMARY_HUE_INC, PRIMARY_HUE_START_POS, tr::align::CENTER_RIGHT, true,
													   hue_arrow_status_cb, primary_hue_inc_action_cb)};
	widget& cur_primary_hue{m_ui.emplace<text_widget>(TAG_CUR_PRIMARY_HUE, PRIMARY_HUE_START_POS, tr::align::CENTER, font::LANGUAGE,
													  tr::system::ttf_style::NORMAL, 48, cur_primary_hue_text_cb)};
	widget& primary_hue_preview{
		m_ui.emplace<color_preview_widget>(TAG_PRIMARY_HUE_PREVIEW, PRIMARY_HUE_START_POS, tr::align::CENTER_RIGHT, m_pending.primary_hue)};
	primary_hue_dec.pos.change(interp_mode::CUBE, {745, PRIMARY_HUE_START_POS.y}, 0.5_s);
	primary_hue_inc.pos.change(interp_mode::CUBE, {930, PRIMARY_HUE_START_POS.y}, 0.5_s);
	cur_primary_hue.pos.change(interp_mode::CUBE, {837.5, PRIMARY_HUE_START_POS.y}, 0.5_s);
	primary_hue_preview.pos.change(interp_mode::CUBE, {985, PRIMARY_HUE_START_POS.y}, 0.5_s);
	primary_hue_dec.unhide(0.5_s);
	primary_hue_inc.unhide(0.5_s);
	cur_primary_hue.unhide(0.5_s);
	primary_hue_preview.unhide(0.5_s);

	widget& secondary_hue_dec{m_ui.emplace<arrow_widget>(TAG_SECONDARY_HUE_DEC, SECONDARY_HUE_START_POS, tr::align::CENTER_LEFT, false,
														 hue_arrow_status_cb, secondary_hue_dec_action_cb)};
	widget& secondary_hue_inc{m_ui.emplace<arrow_widget>(TAG_SECONDARY_HUE_INC, SECONDARY_HUE_START_POS, tr::align::CENTER_RIGHT, true,
														 hue_arrow_status_cb, secondary_hue_inc_action_cb)};
	widget& cur_secondary_hue{m_ui.emplace<text_widget>(TAG_CUR_SECONDARY_HUE, SECONDARY_HUE_START_POS, tr::align::CENTER, font::LANGUAGE,
														tr::system::ttf_style::NORMAL, 48, cur_secondary_hue_text_cb)};
	widget& secondary_hue_preview{m_ui.emplace<color_preview_widget>(TAG_SECONDARY_HUE_PREVIEW, SECONDARY_HUE_START_POS,
																	 tr::align::CENTER_RIGHT, m_pending.secondary_hue)};
	secondary_hue_dec.pos.change(interp_mode::CUBE, {745, SECONDARY_HUE_START_POS.y}, 0.5_s);
	secondary_hue_inc.pos.change(interp_mode::CUBE, {930, SECONDARY_HUE_START_POS.y}, 0.5_s);
	cur_secondary_hue.pos.change(interp_mode::CUBE, {837.5, SECONDARY_HUE_START_POS.y}, 0.5_s);
	secondary_hue_preview.pos.change(interp_mode::CUBE, {985, SECONDARY_HUE_START_POS.y}, 0.5_s);
	secondary_hue_dec.unhide(0.5_s);
	secondary_hue_inc.unhide(0.5_s);
	cur_secondary_hue.unhide(0.5_s);
	secondary_hue_preview.unhide(0.5_s);

	widget& sfx_volume_dec{m_ui.emplace<arrow_widget>(TAG_SFX_VOLUME_DEC, SFX_VOLUME_START_POS, tr::align::CENTER_LEFT, false,
													  sfx_volume_dec_status_cb, sfx_volume_dec_action_cb)};
	widget& sfx_volume_inc{m_ui.emplace<arrow_widget>(TAG_SFX_VOLUME_INC, SFX_VOLUME_START_POS, tr::align::CENTER_RIGHT, true,
													  sfx_volume_inc_status_cb, sfx_volume_inc_action_cb)};
	widget& cur_sound_volume{m_ui.emplace<text_widget>(TAG_CUR_SFX_VOLUME, SFX_VOLUME_START_POS, tr::align::CENTER, font::LANGUAGE,
													   tr::system::ttf_style::NORMAL, 48, cur_sound_volume_text_cb)};
	sfx_volume_dec.pos.change(interp_mode::CUBE, {765, SFX_VOLUME_START_POS.y}, 0.5_s);
	sfx_volume_inc.pos.change(interp_mode::CUBE, {985, SFX_VOLUME_START_POS.y}, 0.5_s);
	cur_sound_volume.pos.change(interp_mode::CUBE, {875, SFX_VOLUME_START_POS.y}, 0.5_s);
	sfx_volume_dec.unhide(0.5_s);
	sfx_volume_inc.unhide(0.5_s);
	cur_sound_volume.unhide(0.5_s);

	widget& music_volume_dec{m_ui.emplace<arrow_widget>(TAG_MUSIC_VOLUME_DEC, MUSIC_VOLUME_START_POS, tr::align::CENTER_LEFT, false,
														music_volume_dec_status_cb, music_volume_dec_action_cb)};
	widget& music_volume_inc{m_ui.emplace<arrow_widget>(TAG_MUSIC_VOLUME_INC, MUSIC_VOLUME_START_POS, tr::align::CENTER_RIGHT, true,
														music_volume_inc_status_cb, music_volume_inc_action_cb)};
	widget& cur_music_volume{m_ui.emplace<text_widget>(TAG_CUR_MUSIC_VOLUME, MUSIC_VOLUME_START_POS, tr::align::CENTER, font::LANGUAGE,
													   tr::system::ttf_style::NORMAL, 48, cur_music_volume_text_cb)};
	music_volume_dec.pos.change(interp_mode::CUBE, {765, MUSIC_VOLUME_START_POS.y}, 0.5_s);
	music_volume_inc.pos.change(interp_mode::CUBE, {985, MUSIC_VOLUME_START_POS.y}, 0.5_s);
	cur_music_volume.pos.change(interp_mode::CUBE, {875, MUSIC_VOLUME_START_POS.y}, 0.5_s);
	music_volume_dec.unhide(0.5_s);
	music_volume_inc.unhide(0.5_s);
	cur_music_volume.unhide(0.5_s);

	widget& cur_language{m_ui.emplace<clickable_text_widget>(TAG_CUR_LANGUAGE, LANGUAGE_START_POS, tr::align::CENTER_RIGHT,
															 font::LANGUAGE_PREVIEW, 48, cur_language_text_cb, cur_language_status_cb,
															 cur_language_action_cb)};
	cur_language.pos.change(interp_mode::CUBE, {985, LANGUAGE_START_POS.y}, 0.5_s);
	cur_language.unhide(0.5_s);

	for (std::size_t i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sound sound{i == 1 ? sound::CONFIRM : sound::CANCEL};
		widget& widget{m_ui.emplace<clickable_text_widget>(BOTTOM_BUTTONS[i], BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE,
														   48, DEFAULT_TEXT_CALLBACK, bottom_status_cbs[i], bottom_action_cbs[i],
														   NO_TOOLTIP, BOTTOM_SHORTCUTS[i], sound)};
		widget.pos.change(interp_mode::CUBE, {500, 1000 - 50 * BOTTOM_BUTTONS.size() + (i + 1) * 50}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> settings_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> settings_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

	switch (m_substate) {
	case substate::IN_SETTINGS:
		return nullptr;
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? std::make_unique<title_state>(std::move(m_background_game)) : nullptr;
	}
}

void settings_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void settings_state::update_window_size_buttons()
{
	widget& cur_window_size{m_ui.get(TAG_CUR_WINDOW_SIZE)};
	if (m_pending.window_size == FULLSCREEN) {
		cur_window_size.pos = glm::vec2{985, 196};
		cur_window_size.alignment = tr::align::CENTER_RIGHT;
		m_ui.get(TAG_WINDOW_SIZE_DEC).hide();
		m_ui.get(TAG_WINDOW_SIZE_INC).hide();
	}
	else {
		cur_window_size.pos = glm::vec2{875, 196};
		cur_window_size.alignment = tr::align::CENTER;
		m_ui.get(TAG_WINDOW_SIZE_DEC).unhide();
		m_ui.get(TAG_WINDOW_SIZE_INC).unhide();
	}
}

void settings_state::update_refresh_rate_buttons()
{
	widget& cur_refresh_rate{m_ui.get(TAG_CUR_REFRESH_RATE)};
	if (m_pending.refresh_rate == NATIVE_REFRESH_RATE) {
		cur_refresh_rate.pos = glm::vec2{985, 271};
		cur_refresh_rate.alignment = tr::align::CENTER_RIGHT;
		m_ui.get(TAG_REFRESH_RATE_DEC).hide();
		m_ui.get(TAG_REFRESH_RATE_INC).hide();
	}
	else {
		cur_refresh_rate.pos = glm::vec2{892.5, 271};
		cur_refresh_rate.alignment = tr::align::CENTER;
		m_ui.get(TAG_REFRESH_RATE_DEC).unhide();
		m_ui.get(TAG_REFRESH_RATE_INC).unhide();
	}
}

void settings_state::set_up_exit_animation()
{
	m_ui.get(TAG_TITLE).pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	for (const char* tag : BOTTOM_BUTTONS) {
		m_ui.get(tag).pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	}
	for (const char* tag : tr::project(LABELS, &label::tag)) {
		widget& widget{m_ui.get(tag)};
		widget.pos.change(interp_mode::CUBE, {-50, glm::vec2{widget.pos}.y}, 0.5_s);
	}
	for (const char* tag : RIGHT_WIDGETS) {
		widget& widget{m_ui.get(tag)};
		widget.pos.change(interp_mode::CUBE, {1050, glm::vec2{widget.pos}.y}, 0.5_s);
	}
	m_ui.hide_all(0.5_s);
}