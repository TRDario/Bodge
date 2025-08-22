#include "../../include/audio.hpp"
#include "../../include/state/state.hpp"
#include "../../include/system.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr tag T_TITLE{"settings"};
constexpr tag T_DISPLAY_MODE{"display_mode"};
constexpr tag T_DISPLAY_MODE_C{"display_mode_c"};
constexpr tag T_WINDOW_SIZE{"window_size"};
constexpr tag T_WINDOW_SIZE_D{"window_size_d"};
constexpr tag T_WINDOW_SIZE_C{"window_size_c"};
constexpr tag T_WINDOW_SIZE_I{"window_size_i"};
constexpr tag T_MSAA{"msaa"};
constexpr tag T_MSAA_D{"msaa_d"};
constexpr tag T_MSAA_C{"msaa_c"};
constexpr tag T_MSAA_I{"msaa_i"};
constexpr tag T_PRIMARY_HUE{"primary_hue"};
constexpr tag T_PRIMARY_HUE_PREVIEW{"primary_hue_preview"};
constexpr tag T_PRIMARY_HUE_D{"primary_hue_d"};
constexpr tag T_PRIMARY_HUE_C{"primary_hue_c"};
constexpr tag T_PRIMARY_HUE_I{"primary_hue_i"};
constexpr tag T_SECONDARY_HUE{"secondary_hue"};
constexpr tag T_SECONDARY_HUE_PREVIEW{"secondary_hue_preview"};
constexpr tag T_SECONDARY_HUE_D{"secondary_hue_d"};
constexpr tag T_SECONDARY_HUE_C{"secondary_hue_c"};
constexpr tag T_SECONDARY_HUE_I{"secondary_hue_i"};
constexpr tag T_SFX_VOLUME{"sfx_volume"};
constexpr tag T_SFX_VOLUME_D{"sfx_volume_d"};
constexpr tag T_SFX_VOLUME_C{"sfx_volume_c"};
constexpr tag T_SFX_VOLUME_I{"sfx_volume_i"};
constexpr tag T_MUSIC_VOLUME{"music_volume"};
constexpr tag T_MUSIC_VOLUME_D{"music_volume_d"};
constexpr tag T_MUSIC_VOLUME_C{"music_volume_c"};
constexpr tag T_MUSIC_VOLUME_I{"music_volume_i"};
constexpr tag T_LANGUAGE{"language"};
constexpr tag T_LANGUAGE_C{"language_c"};
constexpr tag T_REVERT{"revert"};
constexpr tag T_APPLY{"apply"};
constexpr tag T_EXIT{"exit"};

constexpr std::array<tag, 22> RIGHT_WIDGETS{
	T_DISPLAY_MODE_C,
	T_WINDOW_SIZE_D, T_WINDOW_SIZE_C, T_WINDOW_SIZE_I,
	T_MSAA_D, T_MSAA_C, T_MSAA_I,
	T_PRIMARY_HUE_D, T_PRIMARY_HUE_C, T_PRIMARY_HUE_I, T_PRIMARY_HUE_PREVIEW,
	T_SECONDARY_HUE_D, T_SECONDARY_HUE_C, T_SECONDARY_HUE_I, T_SECONDARY_HUE_PREVIEW,
	T_SFX_VOLUME_D, T_SFX_VOLUME_C, T_SFX_VOLUME_I,
	T_MUSIC_VOLUME_D, T_MUSIC_VOLUME_C, T_MUSIC_VOLUME_I,
	T_LANGUAGE_C,
};

constexpr std::array<tag, 3> BOTTOM_BUTTONS{T_REVERT, T_APPLY, T_EXIT};

constexpr const char* NO_TOOLTIP_STR{nullptr};
constexpr std::array<label_info, 8> LABELS{{
	{T_DISPLAY_MODE, "display_mode_tt"},
	{T_WINDOW_SIZE, "window_size_tt"},
	{T_MSAA, "msaa_tt"},
	{T_PRIMARY_HUE, "primary_hue_tt"},
	{T_SECONDARY_HUE, "secondary_hue_tt"},
	{T_SFX_VOLUME, NO_TOOLTIP_STR},
	{T_MUSIC_VOLUME, NO_TOOLTIP_STR},
	{T_LANGUAGE, NO_TOOLTIP_STR},
}};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_DISPLAY_MODE_C},
	selection_tree_row{T_WINDOW_SIZE_D, T_WINDOW_SIZE_C, T_WINDOW_SIZE_I},
	selection_tree_row{T_MSAA_D, T_MSAA_C, T_MSAA_I},
	selection_tree_row{T_PRIMARY_HUE_D, T_PRIMARY_HUE_C, T_PRIMARY_HUE_I},
	selection_tree_row{T_SECONDARY_HUE_D, T_SECONDARY_HUE_C, T_SECONDARY_HUE_I},
	selection_tree_row{T_SFX_VOLUME_D, T_SFX_VOLUME_C, T_SFX_VOLUME_I},
	selection_tree_row{T_MUSIC_VOLUME_D, T_MUSIC_VOLUME_C, T_MUSIC_VOLUME_I},
	selection_tree_row{T_LANGUAGE_C},
	selection_tree_row{T_REVERT},
	selection_tree_row{T_APPLY},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::Z, tr::system::keymod::CTRL}, T_REVERT},
	{{tr::system::keycode::S, tr::system::keymod::CTRL}, T_APPLY},
	{{tr::system::keycode::ESCAPE}, T_EXIT},
};

constexpr glm::vec2 DISPLAY_MODE_START_POS{1050, 196};
constexpr glm::vec2 WINDOW_SIZE_START_POS{1050, 271};
constexpr glm::vec2 MSAA_START_POS{1050, 346};
constexpr glm::vec2 PRIMARY_HUE_START_POS{1050, 421};
constexpr glm::vec2 SECONDARY_HUE_START_POS{1050, 496};
constexpr glm::vec2 SFX_VOLUME_START_POS{1050, 571};
constexpr glm::vec2 MUSIC_VOLUME_START_POS{1050, 646};
constexpr glm::vec2 LANGUAGE_START_POS{1050, 721};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> DISPLAY_MODE_C_MOVE_IN{tween::CUBIC, DISPLAY_MODE_START_POS, {985, DISPLAY_MODE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> WINDOW_SIZE_D_MOVE_IN{tween::CUBIC, WINDOW_SIZE_START_POS, {765, WINDOW_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> WINDOW_SIZE_C_MOVE_IN{tween::CUBIC, WINDOW_SIZE_START_POS, {875, WINDOW_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> WINDOW_SIZE_I_MOVE_IN{tween::CUBIC, WINDOW_SIZE_START_POS, {985, WINDOW_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MSAA_D_MOVE_IN{tween::CUBIC, MSAA_START_POS, {830, MSAA_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MSAA_C_MOVE_IN{tween::CUBIC, MSAA_START_POS, {907.5, MSAA_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MSAA_I_MOVE_IN{tween::CUBIC, MSAA_START_POS, {985, MSAA_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> PRIMARY_HUE_D_MOVE_IN{tween::CUBIC, PRIMARY_HUE_START_POS, {745, PRIMARY_HUE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> PRIMARY_HUE_C_MOVE_IN{tween::CUBIC, PRIMARY_HUE_START_POS, {837.5, PRIMARY_HUE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> PRIMARY_HUE_I_MOVE_IN{tween::CUBIC, PRIMARY_HUE_START_POS, {930, PRIMARY_HUE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> PRIMARY_HUE_PREVIEW_MOVE_IN{tween::CUBIC, PRIMARY_HUE_START_POS, {985, PRIMARY_HUE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SECONDARY_HUE_D_MOVE_IN{tween::CUBIC, SECONDARY_HUE_START_POS, {745, SECONDARY_HUE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SECONDARY_HUE_C_MOVE_IN{tween::CUBIC, SECONDARY_HUE_START_POS, {837.5, SECONDARY_HUE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SECONDARY_HUE_I_MOVE_IN{tween::CUBIC, SECONDARY_HUE_START_POS, {930, SECONDARY_HUE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SECONDARY_HUE_PREVIEW_MOVE_IN{tween::CUBIC, SECONDARY_HUE_START_POS, {985, SECONDARY_HUE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SFX_VOLUME_D_MOVE_IN{tween::CUBIC, SFX_VOLUME_START_POS, {765, SFX_VOLUME_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SFX_VOLUME_C_MOVE_IN{tween::CUBIC, SFX_VOLUME_START_POS, {875, SFX_VOLUME_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SFX_VOLUME_I_MOVE_IN{tween::CUBIC, SFX_VOLUME_START_POS, {985, SFX_VOLUME_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MUSIC_VOLUME_D_MOVE_IN{tween::CUBIC, MUSIC_VOLUME_START_POS, {765, MUSIC_VOLUME_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MUSIC_VOLUME_C_MOVE_IN{tween::CUBIC, MUSIC_VOLUME_START_POS, {875, MUSIC_VOLUME_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> MUSIC_VOLUME_I_MOVE_IN{tween::CUBIC, MUSIC_VOLUME_START_POS, {985, MUSIC_VOLUME_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> LANGUAGE_C_MOVE_IN{tween::CUBIC, LANGUAGE_START_POS, {985, LANGUAGE_START_POS.y}, 0.5_s};

// clang-format on

settings_state::settings_state(std::unique_ptr<game>&& game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_SETTINGS}, m_pending{engine::settings}
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate != substate::ENTERING_TITLE; },
	};
	const status_callback window_size_d_scb{
		[this] {
			return m_substate != substate::ENTERING_TITLE && m_pending.display_mode != display_mode::FULLSCREEN &&
				   m_pending.window_size > MIN_WINDOW_SIZE;
		},
	};
	const status_callback window_size_i_scb{
		[this] {
			return m_substate != substate::ENTERING_TITLE && m_pending.display_mode != display_mode::FULLSCREEN &&
				   m_pending.window_size < max_window_size();
		},
	};
	const status_callback window_size_c_scb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.display_mode != display_mode::FULLSCREEN; },
	};
	const status_callback msaa_d_scb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.msaa != NO_MSAA; },
	};
	const status_callback msaa_i_scb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.msaa != tr::system::max_msaa(); },
	};
	const status_callback sfx_volume_d_scb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.sfx_volume > 0; },
	};
	const status_callback sfx_volume_i_scb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.sfx_volume < 100; },
	};
	const status_callback music_volume_d_scb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.music_volume > 0; },
	};
	const status_callback music_volume_i_scb{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending.music_volume < 100; },
	};
	const status_callback language_c_scb{
		[this] {
			return m_substate != substate::ENTERING_TITLE &&
				   (engine::languages.size() >= 2 - (!engine::languages.contains(m_pending.language)));
		},
	};
	const std::array<status_callback, BOTTOM_BUTTONS.size()> bottom_scbs{
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending != engine::settings; },
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending != engine::settings; },
		[this] { return m_substate != substate::ENTERING_TITLE && m_pending == engine::settings; },
	};

	// ACTION CALLBACKS

	const action_callback display_mode_c_acb{
		[&, &dm = m_pending.display_mode] {
			switch (dm) {
			case display_mode::WINDOWED:
				dm = display_mode::FULLSCREEN;
				m_ui.as<label_widget>(T_WINDOW_SIZE).color.change(tween::LERP, "505050A0"_rgba8, 0.1_s);
				break;
			case display_mode::FULLSCREEN:
				dm = display_mode::WINDOWED;
				m_ui.as<label_widget>(T_WINDOW_SIZE).color.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
				break;
			}
		},
	};
	const action_callback window_size_d_acb{
		[&ws = m_pending.window_size] { ws = std::max(MIN_WINDOW_SIZE, std::uint16_t(ws - engine::keymods_choose(1, 10, 100))); },
	};
	const action_callback window_size_i_acb{
		[&ws = m_pending.window_size] { ws = std::min(max_window_size(), std::uint16_t(ws + engine::keymods_choose(1, 10, 100))); },
	};
	const action_callback msaa_d_acb{
		[&msaa = m_pending.msaa] { msaa = msaa == 2 ? NO_MSAA : std::uint8_t(msaa / 2); },
	};
	const action_callback msaa_i_acb{
		[&msaa = m_pending.msaa] { msaa = msaa == NO_MSAA ? 2 : std::uint8_t(msaa * 2); },
	};
	const action_callback primary_hue_d_acb{
		[&ph = m_pending.primary_hue] { ph = std::uint16_t((ph - engine::keymods_choose(1, 10, 100) + 360) % 360); },
	};
	const action_callback primary_hue_i_acb{
		[&ph = m_pending.primary_hue] { ph = std::uint16_t((ph + engine::keymods_choose(1, 10, 100)) % 360); },
	};
	const action_callback secondary_hue_d_acb{
		[&sh = m_pending.secondary_hue] { sh = std::uint16_t((sh - engine::keymods_choose(1, 10, 100) + 360) % 360); },
	};
	const action_callback secondary_hue_i_acb{
		[&sh = m_pending.secondary_hue] { sh = std::uint16_t((sh + engine::keymods_choose(1, 10, 100)) % 360); },
	};
	const action_callback sfx_volume_d_acb{
		[&sv = m_pending.sfx_volume] { sv = std::uint8_t(std::max(sv - engine::keymods_choose(1, 10, 25), 0)); },
	};
	const action_callback sfx_volume_i_acb{
		[&sv = m_pending.sfx_volume] { sv = std::uint8_t(std::min(sv + engine::keymods_choose(1, 10, 25), 100)); },
	};
	const action_callback music_volume_d_acb{
		[&mv = m_pending.music_volume] { mv = std::uint8_t(std::max(mv - engine::keymods_choose(1, 10, 25), 0)); },
	};
	const action_callback music_volume_i_acb{
		[&mv = m_pending.music_volume] { mv = std::uint8_t(std::min(mv + engine::keymods_choose(1, 10, 25), 100)); },
	};
	const action_callback language_c_acb{
		[this] {
			std::map<language_code, language_info>::iterator it{engine::languages.find(m_pending.language)};
			if (it == engine::languages.end() || ++it == engine::languages.end()) {
				it = engine::languages.begin();
			}
			m_pending.language = it->first;
			engine::reload_language_preview_font(m_pending);
		},
	};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_acbs{
		[this] {
			m_pending = engine::settings;
			engine::reload_language_preview_font(m_pending);
			const tr::rgba8 window_size_color{m_pending.display_mode == display_mode::WINDOWED ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8};
			m_ui.as<label_widget>(T_WINDOW_SIZE).color.change(tween::LERP, window_size_color, 0.1_s);
		},
		[this] {
			const settings old{engine::settings};
			engine::settings = m_pending;
			if (engine::restart_required(old)) {
				m_ui.release_graphical_resources();
			}
			if (old.language != engine::settings.language) {
				engine::load_localization();
			}
			if (!engine::languages.contains(old.language) ||
				engine::languages[old.language].font != engine::languages[engine::settings.language].font) {
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

	// VALIDATION CALLBACKS

	const validation_callback<std::uint16_t> window_size_c_vcb{
		[](std::uint16_t v) { return std::clamp(v, MIN_WINDOW_SIZE, max_window_size()); },
	};
	const validation_callback<std::uint16_t> hue_c_vcb{
		[](std::uint16_t v) { return v % 360; },
	};
	const validation_callback<std::uint8_t> volume_c_vcb{
		[](std::uint8_t v) { return std::min(v, std::uint8_t(100)); },
	};

	// TEXT CALLBACKS

	const text_callback display_mode_c_tcb{
		[&dm = m_pending.display_mode] { return std::string{engine::loc[dm == display_mode::FULLSCREEN ? "fullscreen" : "windowed"]}; },
	};
	const text_callback msaa_c_tcb{
		[this] { return m_pending.msaa == NO_MSAA ? "--" : std::format("x{}", m_pending.msaa); },
	};
	const text_callback language_c_tcb{
		[this] { return engine::languages.contains(m_pending.language) ? engine::languages[m_pending.language].name : "???"; },
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const label_info& label{LABELS[i]};
		const tweener<glm::vec2> move_in{tween::CUBIC, {-50, 196 + i * 75}, {15, 196 + i * 75}, 0.5_s};
		const tr::rgba8 color{label.tag == T_WINDOW_SIZE && m_pending.display_mode == display_mode::FULLSCREEN ? "505050A0"_rgba8
																											   : "A0A0A0A0"_rgba8};
		m_ui.emplace<label_widget>(label.tag, move_in, tr::align::CENTER_LEFT, 0.5_s, tooltip_loc_text_callback{LABELS[i].tooltip},
								   loc_text_callback{label.tag}, tr::system::ttf_style::NORMAL, 48, color);
	}

	m_ui.emplace<text_button_widget>(T_DISPLAY_MODE_C, DISPLAY_MODE_C_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, NO_TOOLTIP,
									 display_mode_c_tcb, font::LANGUAGE, 48, scb, display_mode_c_acb, sound::CONFIRM);
	m_ui.emplace<arrow_widget>(T_WINDOW_SIZE_D, WINDOW_SIZE_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, window_size_d_scb,
							   window_size_d_acb);
	m_ui.emplace<numeric_input_widget<std::uint16_t, 4, "{}", "{}">>(T_WINDOW_SIZE_C, WINDOW_SIZE_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48,
																	 m_ui, m_pending.window_size, window_size_c_scb, window_size_c_vcb);
	m_ui.emplace<arrow_widget>(T_WINDOW_SIZE_I, WINDOW_SIZE_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, window_size_i_scb,
							   window_size_i_acb);
	m_ui.emplace<arrow_widget>(T_MSAA_D, MSAA_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, msaa_d_scb, msaa_d_acb);
	m_ui.emplace<label_widget>(T_MSAA_C, MSAA_C_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, msaa_c_tcb, tr::system::ttf_style::NORMAL,
							   48);
	m_ui.emplace<arrow_widget>(T_MSAA_I, MSAA_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, msaa_i_scb, msaa_i_acb);
	m_ui.emplace<arrow_widget>(T_PRIMARY_HUE_D, PRIMARY_HUE_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, scb, primary_hue_d_acb);
	m_ui.emplace<numeric_input_widget<std::uint16_t, 3, "{}", "{}">>(T_PRIMARY_HUE_C, PRIMARY_HUE_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48,
																	 m_ui, m_pending.primary_hue, scb, hue_c_vcb);
	m_ui.emplace<arrow_widget>(T_PRIMARY_HUE_I, PRIMARY_HUE_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, scb, primary_hue_i_acb);
	m_ui.emplace<color_preview_widget>(T_PRIMARY_HUE_PREVIEW, PRIMARY_HUE_PREVIEW_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s,
									   m_pending.primary_hue);
	m_ui.emplace<arrow_widget>(T_SECONDARY_HUE_D, SECONDARY_HUE_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, scb, secondary_hue_d_acb);
	m_ui.emplace<numeric_input_widget<std::uint16_t, 3, "{}", "{}">>(T_SECONDARY_HUE_C, SECONDARY_HUE_C_MOVE_IN, tr::align::CENTER, 0.5_s,
																	 48, m_ui, m_pending.secondary_hue, scb, hue_c_vcb);
	m_ui.emplace<arrow_widget>(T_SECONDARY_HUE_I, SECONDARY_HUE_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, scb, secondary_hue_i_acb);
	m_ui.emplace<color_preview_widget>(T_SECONDARY_HUE_PREVIEW, SECONDARY_HUE_PREVIEW_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s,
									   m_pending.secondary_hue);
	m_ui.emplace<arrow_widget>(T_SFX_VOLUME_D, SFX_VOLUME_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, sfx_volume_d_scb,
							   sfx_volume_d_acb);
	m_ui.emplace<numeric_input_widget<std::uint8_t, 3, "{}%", "{}%">>(T_SFX_VOLUME_C, SFX_VOLUME_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48,
																	  m_ui, m_pending.sfx_volume, scb, volume_c_vcb);
	m_ui.emplace<arrow_widget>(T_SFX_VOLUME_I, SFX_VOLUME_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, sfx_volume_i_scb,
							   sfx_volume_i_acb);
	m_ui.emplace<arrow_widget>(T_MUSIC_VOLUME_D, MUSIC_VOLUME_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, music_volume_d_scb,
							   music_volume_d_acb);
	m_ui.emplace<numeric_input_widget<std::uint8_t, 3, "{}%", "{}%">>(T_MUSIC_VOLUME_C, MUSIC_VOLUME_C_MOVE_IN, tr::align::CENTER, 0.5_s,
																	  48, m_ui, m_pending.music_volume, scb, volume_c_vcb);
	m_ui.emplace<arrow_widget>(T_MUSIC_VOLUME_I, MUSIC_VOLUME_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, music_volume_i_scb,
							   music_volume_i_acb);
	m_ui.emplace<text_button_widget>(T_LANGUAGE_C, LANGUAGE_C_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, NO_TOOLTIP, language_c_tcb,
									 font::LANGUAGE_PREVIEW, 48, language_c_scb, language_c_acb, sound::CONFIRM);
	for (std::size_t i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const tweener<glm::vec2> move_in{tween::CUBIC, BOTTOM_START_POS, {500, 1000 - 50 * BOTTOM_BUTTONS.size() + (i + 1) * 50}, 0.5_s};
		const sound sound{i == 1 ? sound::CONFIRM : sound::CANCEL};
		m_ui.emplace<text_button_widget>(BOTTOM_BUTTONS[i], move_in, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP,
										 loc_text_callback{BOTTOM_BUTTONS[i]}, font::LANGUAGE, 48, bottom_scbs[i], bottom_acbs[i], sound);
	}
}

//

std::unique_ptr<tr::state> settings_state::update(tr::duration)
{
	main_menu_state::update({});
	switch (m_substate) {
	case substate::IN_SETTINGS:
		return nullptr;
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? std::make_unique<title_state>(release_game()) : nullptr;
	}
}

//

void settings_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	}
	for (tag tag : tr::project(LABELS, &label_info::tag)) {
		widget& widget{m_ui[tag]};
		widget.pos.change(tween::CUBIC, {-50, glm::vec2{widget.pos}.y}, 0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		widget& widget{m_ui[tag]};
		widget.pos.change(tween::CUBIC, {1050, glm::vec2{widget.pos}.y}, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
}