///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements settings_state from state.hpp.                                                                                             //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/audio.hpp"
#include "../../include/input.hpp"
#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"settings"};
constexpr tag T_DISPLAY_MODE{"display_mode"};
constexpr tag T_DISPLAY_MODE_C{"display_mode_c"};
constexpr tag T_WINDOW_SIZE{"window_size"};
constexpr tag T_WINDOW_SIZE_D{"window_size_d"};
constexpr tag T_WINDOW_SIZE_C{"window_size_c"};
constexpr tag T_WINDOW_SIZE_I{"window_size_i"};
constexpr tag T_VSYNC{"vsync"};
constexpr tag T_VSYNC_C{"vsync_c"};
constexpr tag T_MOUSE_SENSITIVITY{"mouse_sensitivity"};
constexpr tag T_MOUSE_SENSITIVITY_D{"mouse_sensitivity_d"};
constexpr tag T_MOUSE_SENSITIVITY_C{"mouse_sensitivity_c"};
constexpr tag T_MOUSE_SENSITIVITY_I{"mouse_sensitivity_i"};
constexpr tag T_PLAYER_SKIN{"player_skin"};
constexpr tag T_PLAYER_SKIN_C{"player_skin_c"};
constexpr tag T_PLAYER_SKIN_PREVIEW{"player_skin_preview"};
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

// Left-hand side labels.
constexpr std::array LABELS{
	label_info{T_DISPLAY_MODE, "display_mode_tt"},
	label_info{T_WINDOW_SIZE, "window_size_tt"},
	label_info{T_VSYNC, "vsync_tt"},
	label_info{T_MOUSE_SENSITIVITY, "mouse_sensitivity_tt"},
	label_info{T_PLAYER_SKIN, "player_skin_tt"},
	label_info{T_PRIMARY_HUE, "primary_hue_tt"},
	label_info{T_SECONDARY_HUE, "secondary_hue_tt"},
	label_info{T_SFX_VOLUME, "sfx_volume_tt"},
	label_info{T_MUSIC_VOLUME, "music_volume_tt"},
	label_info{T_LANGUAGE, "language_tt"},
};

// Right-hand side interactible widgets.
constexpr std::array RIGHT_WIDGETS{
	T_DISPLAY_MODE_C,
	T_WINDOW_SIZE_D, T_WINDOW_SIZE_C, T_WINDOW_SIZE_I,
	T_VSYNC_C,
	T_MOUSE_SENSITIVITY_D, T_MOUSE_SENSITIVITY_C, T_MOUSE_SENSITIVITY_I,
	T_PLAYER_SKIN_C, T_PLAYER_SKIN_PREVIEW,
	T_PRIMARY_HUE_D, T_PRIMARY_HUE_C, T_PRIMARY_HUE_I, T_PRIMARY_HUE_PREVIEW,
	T_SECONDARY_HUE_D, T_SECONDARY_HUE_C, T_SECONDARY_HUE_I, T_SECONDARY_HUE_PREVIEW,
	T_SFX_VOLUME_D, T_SFX_VOLUME_C, T_SFX_VOLUME_I,
	T_MUSIC_VOLUME_D, T_MUSIC_VOLUME_C, T_MUSIC_VOLUME_I,
	T_LANGUAGE_C,
};

// Buttons on the bottom of the screen.
constexpr std::array BOTTOM_BUTTONS{T_REVERT, T_APPLY, T_EXIT};

// Selection tree used for the settings menu.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_DISPLAY_MODE_C},
	selection_tree_row{T_WINDOW_SIZE_D, T_WINDOW_SIZE_C, T_WINDOW_SIZE_I},
	selection_tree_row{T_VSYNC_C},
	selection_tree_row{T_MOUSE_SENSITIVITY_D, T_MOUSE_SENSITIVITY_C, T_MOUSE_SENSITIVITY_I},
	selection_tree_row{T_PLAYER_SKIN_C},
	selection_tree_row{T_PRIMARY_HUE_D, T_PRIMARY_HUE_C, T_PRIMARY_HUE_I},
	selection_tree_row{T_SECONDARY_HUE_D, T_SECONDARY_HUE_C, T_SECONDARY_HUE_I},
	selection_tree_row{T_SFX_VOLUME_D, T_SFX_VOLUME_C, T_SFX_VOLUME_I},
	selection_tree_row{T_MUSIC_VOLUME_D, T_MUSIC_VOLUME_C, T_MUSIC_VOLUME_I},
	selection_tree_row{T_LANGUAGE_C},
	selection_tree_row{T_REVERT},
	selection_tree_row{T_APPLY},
	selection_tree_row{T_EXIT},
};

// Shortcut table used for the settings menu.
constexpr shortcut_table SHORTCUTS{
	{"Ctrl+Z"_kc, T_REVERT},
	{"Ctrl+S"_kc, T_APPLY},
	{"Enter"_kc, T_APPLY},
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT},
};

// Starting position for display mode right widgets.
constexpr glm::vec2 DISPLAY_MODE_START_POS{1050, 121};
// Starting position for window size right widgets.
constexpr glm::vec2 WINDOW_SIZE_START_POS{1050, DISPLAY_MODE_START_POS.y + 75};
// Starting position for V-sync right widgets.
constexpr glm::vec2 VSYNC_START_POS{1050, WINDOW_SIZE_START_POS.y + 75};
// Starting position for mouse sensitivity right widgets.
constexpr glm::vec2 MOUSE_SENSITIVITY_START_POS{1050, VSYNC_START_POS.y + 75};
// Starting position for player skin right widgets.
constexpr glm::vec2 PLAYER_SKIN_START_POS{1050, MOUSE_SENSITIVITY_START_POS.y + 75};
// Starting position for primary hue right widgets.
constexpr glm::vec2 PRIMARY_HUE_START_POS{1050, PLAYER_SKIN_START_POS.y + 75};
// Starting position for secondary hue right widgets.
constexpr glm::vec2 SECONDARY_HUE_START_POS{1050, PRIMARY_HUE_START_POS.y + 75};
// Starting position for SFX volume right widgets.
constexpr glm::vec2 SFX_VOLUME_START_POS{1050, SECONDARY_HUE_START_POS.y + 75};
// Starting position for music volume right widgets.
constexpr glm::vec2 MUSIC_VOLUME_START_POS{1050, SFX_VOLUME_START_POS.y + 75};
// Starting position for language right widgets.
constexpr glm::vec2 LANGUAGE_START_POS{1050, MUSIC_VOLUME_START_POS.y + 75};

// clang-format on
///////////////////////////////////////////////////////////// INTERNAL HELPERS ////////////////////////////////////////////////////////////

std::vector<std::string> find_skins()
{
	constexpr std::array SKIN_EXTENSIONS{".bmp", ".qoi", ".png"};

	try {
		std::vector<std::string> skins;
		for (std::filesystem::directory_entry entry :
			 std::filesystem::directory_iterator{debug_settings::instance().user_directory() / "skins"}) {
			if (entry.is_regular_file() && std::ranges::find(SKIN_EXTENSIONS, entry.path().extension()) != SKIN_EXTENSIONS.end()) {
				skins.push_back(entry.path().filename().string());
			}
		}
		return skins;
	}
	catch (...) {
		return {};
	}
}

////////////////////////////////////////////////////////////// SETTINGS STATE /////////////////////////////////////////////////////////////

settings_state::settings_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_SETTINGS}
	, m_pending(active_settings::instance())
	, m_player_skins{find_skins()}
{
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = {TOP_START_POS, TITLE_POS, 0.5_s},
		.alignment = tr::align::TOP_CENTER,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});

	for (usize i = 0; i < LABELS.size(); ++i) {
		m_ui.emplace<label_widget>(LABELS[i].tag, {
			.animation = {{-50, 121 + i * 75}, {15, 121 + i * 75}, 0.5_s},
			.alignment = tr::align::CENTER_LEFT,
			.tooltip_text = localized_text{LABELS[i].tooltip},
			.text = localized_text{LABELS[i].tag},
			.color = LABELS[i].tag == T_WINDOW_SIZE && m_pending.display_mode == display_mode::FULLSCREEN ? DISABLED_GRAY : GRAY
		});
	}

	m_ui.emplace<text_button_widget>(T_DISPLAY_MODE_C, {
		.animation = {DISPLAY_MODE_START_POS, {985, DISPLAY_MODE_START_POS.y}, 0.5_s},
		.alignment = tr::align::CENTER_RIGHT,
		.text = [this] { return std::string{localization::instance()[m_pending.display_mode == display_mode::FULLSCREEN ? "fullscreen" : "windowed"]}; },
		.status = [this] { return m_substate != substate::EXITING; },
		.action = [this] { on_change_display_mode(); }
	});
	m_ui.emplace<arrow_widget>(T_WINDOW_SIZE_D, {
		.animation = {WINDOW_SIZE_START_POS, {765, WINDOW_SIZE_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] {
			return m_substate != substate::EXITING && m_pending.display_mode != display_mode::FULLSCREEN &&
			       m_pending.window_size > MIN_WINDOW_SIZE;
		},
		.action = [&ws = m_pending.window_size] { ws = std::max(MIN_WINDOW_SIZE, u16(ws - input::instance().choose(1, 10, 100))); }
	});
	m_ui.emplace<numeric_input_widget<u16, 4>>(T_WINDOW_SIZE_C, {
		.animation = {WINDOW_SIZE_START_POS, {875, WINDOW_SIZE_START_POS.y}, 0.5_s},
		.ui = m_ui,
		.variable = m_pending.window_size,
		.status = [this] { return m_substate != substate::EXITING && m_pending.display_mode != display_mode::FULLSCREEN; },
		.validation = [](u16 v) { return std::clamp(v, MIN_WINDOW_SIZE, max_window_size()); }
	});
	m_ui.emplace<arrow_widget>(T_WINDOW_SIZE_I, {
		.animation = {WINDOW_SIZE_START_POS, {985, WINDOW_SIZE_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] {
			return m_substate != substate::EXITING && m_pending.display_mode != display_mode::FULLSCREEN &&
				   m_pending.window_size < max_window_size();
		},
		.action = [&ws = m_pending.window_size] { ws = std::min(max_window_size(), u16(ws + input::instance().choose(1, 10, 100))); }
	});
	m_ui.emplace<text_button_widget>(T_VSYNC_C, {
		.animation = {VSYNC_START_POS, {985, VSYNC_START_POS.y}, 0.5_s},
		.alignment = tr::align::CENTER_RIGHT,
		.text = [&vsync = m_pending.vsync] { return std::string{localization::instance()[vsync ? "on" : "off"]}; },
		.status = [this] { return m_substate != substate::EXITING; },
		.action = [&vsync = m_pending.vsync] { vsync = !vsync; },
	});
	m_ui.emplace<arrow_widget>(T_MOUSE_SENSITIVITY_D, {
		.animation = {MOUSE_SENSITIVITY_START_POS, {765, MOUSE_SENSITIVITY_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate != substate::EXITING && m_pending.mouse_sensitivity > 25; },
		.action = [&ms = m_pending.mouse_sensitivity] { ms = u8(std::max(ms - input::instance().choose(1, 10, 25), 25)); }
	});
	m_ui.emplace<numeric_input_widget<u8, 3, "{}%", "{}%">>(T_MOUSE_SENSITIVITY_C, {
		.animation = {MOUSE_SENSITIVITY_START_POS, {875, MOUSE_SENSITIVITY_START_POS.y}, 0.5_s},
		.ui = m_ui,
		.variable = m_pending.mouse_sensitivity,
		.status = [this] { return m_substate != substate::EXITING; },
		.validation = [](int v) { return u8(std::clamp(v, 25, 250)); }
	});
	m_ui.emplace<arrow_widget>(T_MOUSE_SENSITIVITY_I, {
		.animation = {MOUSE_SENSITIVITY_START_POS, {985, MOUSE_SENSITIVITY_START_POS.y}, 0.5_s}, 
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate != substate::EXITING && m_pending.mouse_sensitivity < 250; },
		.action = [&ms = m_pending.mouse_sensitivity] { ms = u8(std::min(ms + input::instance().choose(1, 10, 25), 250)); }
	});
	m_ui.emplace<text_button_widget>(T_PLAYER_SKIN_C, {
		.animation = {PLAYER_SKIN_START_POS, {930, PLAYER_SKIN_START_POS.y}, 0.5_s},
		.alignment = tr::align::CENTER_RIGHT,
		.text = [this] {
			const bool valid_player_skin{std::ranges::find(m_player_skins, m_pending.player_skin) != m_player_skins.end()};
			return valid_player_skin               ? m_pending.player_skin.substr(0, m_pending.player_skin.find_last_of('.'))
				   : m_pending.player_skin.empty() ? std::string{localization::instance()["none"]}
												   : std::string{localization::instance()["unknown"]};
		},
		.status = [this] { return m_substate != substate::EXITING && (m_player_skins.size() > 0 || !m_pending.player_skin.empty()); },
		.action = [this] { on_change_player_skin(); }
	});
	m_ui.emplace<player_skin_preview_widget>(T_PLAYER_SKIN_PREVIEW, {
		.animation = {PLAYER_SKIN_START_POS, {985, PLAYER_SKIN_START_POS.y}, 0.5_s},
		.alignment = tr::align::CENTER_RIGHT,
		.settings = m_pending
	});
	m_ui.emplace<arrow_widget>(T_PRIMARY_HUE_D, {
		.animation = {PRIMARY_HUE_START_POS, {745, PRIMARY_HUE_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate != substate::EXITING; },
		.action = [&ph = m_pending.primary_hue] { ph = u16((ph - input::instance().choose(1, 10, 100) + 360) % 360); }
	});
	m_ui.emplace<numeric_input_widget<u16, 3>>(T_PRIMARY_HUE_C, {
		.animation = {PRIMARY_HUE_START_POS, {837.5, PRIMARY_HUE_START_POS.y}, 0.5_s},
		.ui = m_ui,
		.variable = m_pending.primary_hue,
		.status = [this] { return m_substate != substate::EXITING; },
		.validation = [](int v) { return u16(v % 360); }
	});
	m_ui.emplace<arrow_widget>(T_PRIMARY_HUE_I, {
		.animation = {PRIMARY_HUE_START_POS, {930, PRIMARY_HUE_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate != substate::EXITING; },
		.action = [&ph = m_pending.primary_hue] { ph = u16((ph + input::instance().choose(1, 10, 100)) % 360); }
	});
	m_ui.emplace<color_preview_widget>(T_PRIMARY_HUE_PREVIEW, {
		.animation = {PRIMARY_HUE_START_POS, {985, PRIMARY_HUE_START_POS.y}, 0.5_s},
		.alignment = tr::align::CENTER_RIGHT,
		.hue = m_pending.primary_hue
	});
	m_ui.emplace<arrow_widget>(T_SECONDARY_HUE_D, {
		.animation = {SECONDARY_HUE_START_POS, {745, SECONDARY_HUE_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate != substate::EXITING; },
		.action = [&sh = m_pending.secondary_hue] { sh = u16((sh - input::instance().choose(1, 10, 100) + 360) % 360); }
	});
	m_ui.emplace<numeric_input_widget<u16, 3>>(T_SECONDARY_HUE_C, {
		.animation = {SECONDARY_HUE_START_POS, {837.5, SECONDARY_HUE_START_POS.y}, 0.5_s},
		.ui = m_ui,
		.variable = m_pending.secondary_hue,
		.status = [this] { return m_substate != substate::EXITING; },
		.validation = [](int v) { return u16(v % 360); }
	});
	m_ui.emplace<arrow_widget>(T_SECONDARY_HUE_I, {
		.animation = {SECONDARY_HUE_START_POS, {930, SECONDARY_HUE_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate != substate::EXITING; },
		.action = [&sh = m_pending.secondary_hue] { sh = u16((sh + input::instance().choose(1, 10, 100)) % 360); }
	});
	m_ui.emplace<color_preview_widget>(T_SECONDARY_HUE_PREVIEW, {
		.animation = {SECONDARY_HUE_START_POS, {985, SECONDARY_HUE_START_POS.y}, 0.5_s},
		.alignment = tr::align::CENTER_RIGHT,
		.hue = m_pending.secondary_hue
	});
	m_ui.emplace<arrow_widget>(T_SFX_VOLUME_D, {
		.animation = {SFX_VOLUME_START_POS, {765, SFX_VOLUME_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate != substate::EXITING && m_pending.sfx_volume > 0; },
		.action = [&sv = m_pending.sfx_volume] { sv = u8(std::max(sv - input::instance().choose(1, 10, 25), 0)); }
	});
	m_ui.emplace<numeric_input_widget<u8, 3, "{}%", "{}%">>(T_SFX_VOLUME_C, {
		.animation = {SFX_VOLUME_START_POS, {875, SFX_VOLUME_START_POS.y}, 0.5_s},
		.ui = m_ui,
		.variable = m_pending.sfx_volume,
		.status = [this] { return m_substate != substate::EXITING; },
		.validation = [](int v) { return u8(std::min(v, 100)); }
	});
	m_ui.emplace<arrow_widget>(T_SFX_VOLUME_I, {
		.animation = {SFX_VOLUME_START_POS, {985, SFX_VOLUME_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate != substate::EXITING && m_pending.sfx_volume < 100; },
		.action = [&sv = m_pending.sfx_volume] { sv = u8(std::min(sv + input::instance().choose(1, 10, 25), 100)); }
	});
	m_ui.emplace<arrow_widget>(T_MUSIC_VOLUME_D, {
		.animation = {MUSIC_VOLUME_START_POS, {765, MUSIC_VOLUME_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate != substate::EXITING && m_pending.music_volume > 0; },
		.action = [&mv = m_pending.music_volume] { mv = u8(std::max(mv - input::instance().choose(1, 10, 25), 0)); }
	});
	m_ui.emplace<numeric_input_widget<u8, 3, "{}%", "{}%">>(T_MUSIC_VOLUME_C, {
		.animation = {MUSIC_VOLUME_START_POS, {875, MUSIC_VOLUME_START_POS.y}, 0.5_s},
		.ui = m_ui,
		.variable = m_pending.music_volume,
		.status = [this] { return m_substate != substate::EXITING; },
		.validation = [](int v) { return u8(std::min(v, 100)); }
	});
	m_ui.emplace<arrow_widget>(T_MUSIC_VOLUME_I, {
		.animation = {MUSIC_VOLUME_START_POS, {985, MUSIC_VOLUME_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate != substate::EXITING && m_pending.music_volume < 100; },
		.action = [&mv = m_pending.music_volume] { mv = u8(std::min(mv + input::instance().choose(1, 10, 25), 100)); }
	});
	m_ui.emplace<text_button_widget>(T_LANGUAGE_C, {
		.animation = {LANGUAGE_START_POS, {985, LANGUAGE_START_POS.y}, 0.5_s},
		.alignment = tr::align::CENTER_RIGHT,
		.text = [this] {
			return localization::instance().available_languages.contains(m_pending.language)
			       ? localization::instance().available_languages.at(m_pending.language).name
				   : "???";
		},
		.font = font::LANGUAGE_PREVIEW,
		.status = [this] {
			return m_substate != substate::EXITING &&
			       (localization::instance().available_languages.size() >= 2 - (!localization::instance().available_languages.contains(m_pending.language)));
		},
		.action = [this] { on_change_language(); }
	});

	struct bottom_button_parameters {
		status_command status;
		action_command action;
		sound sound;
	};
	const std::array<bottom_button_parameters, BOTTOM_BUTTONS.size()> bottom_button_parameters{{
		{[this] { return m_substate != substate::EXITING && m_pending != active_settings::instance(); },
		 [this] { on_revert(); },
		 sound::CONFIRM},
		{[this] { return m_substate != substate::EXITING && m_pending != active_settings::instance(); },
		 [this] { on_apply(); },
		 sound::CONFIRM},
		{[this] { return m_substate != substate::EXITING && m_pending == active_settings::instance(); },
		 [this] { on_exit(); },
		 sound::CANCEL},
	}};
	for (usize i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		m_ui.emplace<text_button_widget>(BOTTOM_BUTTONS[i], {
			.animation = {BOTTOM_START_POS, {500, 1000 - 50 * BOTTOM_BUTTONS.size() + (i + 1) * 50}, 0.5_s},
			.alignment = tr::align::BOTTOM_CENTER,
			.text = localized_text{BOTTOM_BUTTONS[i]},
			.status = bottom_button_parameters[i].status,
			.action = bottom_button_parameters[i].action,
			.action_sound = bottom_button_parameters[i].sound
		});
	}
	// clang-format on
}

//

tr::next_state settings_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_SETTINGS:
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

//

void settings_state::set_up_exit_animation()
{
	m_ui[T_TITLE].move_and_hide(TOP_START_POS, 0.5_s);
	for (const label_info& label : LABELS) {
		m_ui[label.tag].move_x_and_hide(-50, 0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		m_ui[tag].move_x_and_hide(1050, 0.5_s);
	}
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].move_and_hide(BOTTOM_START_POS, 0.5_s);
	}
}

//

void settings_state::on_change_display_mode()
{
	switch (m_pending.display_mode) {
	case display_mode::WINDOWED:
		m_pending.display_mode = display_mode::FULLSCREEN;
		m_ui.as<label_widget>(T_WINDOW_SIZE).tint.change(DISABLED_GRAY, 0.1_s);
		break;
	case display_mode::FULLSCREEN:
		m_pending.display_mode = display_mode::WINDOWED;
		m_ui.as<label_widget>(T_WINDOW_SIZE).tint.change(GRAY, 0.1_s);
		break;
	}
}

void settings_state::on_change_player_skin()
{
	std::vector<std::string>::iterator player_skin_it{std::ranges::find(m_player_skins, m_pending.player_skin)};
	if (player_skin_it == m_player_skins.end() && !m_player_skins.empty()) {
		player_skin_it = m_player_skins.begin();
		m_pending.player_skin = *player_skin_it;
	}
	else if (m_player_skins.empty() || ++player_skin_it == m_player_skins.end()) {
		m_pending.player_skin = {};
	}
	else {
		m_pending.player_skin = *player_skin_it;
	}
	m_ui.as<player_skin_preview_widget>(T_PLAYER_SKIN_PREVIEW).update_skin();
}

void settings_state::on_change_language()
{
	const std::map<language_code, language_info>& available_languages{localization::instance().available_languages};

	std::map<language_code, language_info>::const_iterator language_it{available_languages.find(m_pending.language)};
	if (language_it == available_languages.end() || ++language_it == available_languages.end()) {
		language_it = available_languages.begin();
	}
	m_pending.language = language_it->first;
	g_text_engine.reload_language_preview_font(m_pending);
}

void settings_state::on_revert()
{
	m_pending = active_settings::instance();
	g_text_engine.reload_language_preview_font(m_pending);
	const tr::rgba8 window_size_color{m_pending.display_mode == display_mode::WINDOWED ? GRAY : DISABLED_GRAY};
	m_ui.as<label_widget>(T_WINDOW_SIZE).tint.change(window_size_color, 0.1_s);
	m_ui.as<player_skin_preview_widget>(T_PLAYER_SKIN_PREVIEW).update_skin();
}

void settings_state::on_apply()
{
	if (active_settings::instance().releasing_graphical_resources_required_to_apply(m_pending)) {
		m_ui.release_graphical_resources();
	}
	active_settings::instance().apply(m_pending);
}

void settings_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<title_state>(m_game);
}