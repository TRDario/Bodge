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
constexpr tag T_VSYNC{"vsync"};
constexpr tag T_VSYNC_C{"vsync_c"};
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

constexpr std::array<tag, 23> RIGHT_WIDGETS{
	T_DISPLAY_MODE_C,
	T_WINDOW_SIZE_D, T_WINDOW_SIZE_C, T_WINDOW_SIZE_I,
	T_VSYNC_C,
	T_MSAA_D, T_MSAA_C, T_MSAA_I,
	T_PRIMARY_HUE_D, T_PRIMARY_HUE_C, T_PRIMARY_HUE_I, T_PRIMARY_HUE_PREVIEW,
	T_SECONDARY_HUE_D, T_SECONDARY_HUE_C, T_SECONDARY_HUE_I, T_SECONDARY_HUE_PREVIEW,
	T_SFX_VOLUME_D, T_SFX_VOLUME_C, T_SFX_VOLUME_I,
	T_MUSIC_VOLUME_D, T_MUSIC_VOLUME_C, T_MUSIC_VOLUME_I,
	T_LANGUAGE_C,
};

constexpr std::array<tag, 3> BOTTOM_BUTTONS{T_REVERT, T_APPLY, T_EXIT};

constexpr const char* NO_TOOLTIP_STR{nullptr};
constexpr std::array<label_info, 9> LABELS{{
	{T_DISPLAY_MODE, "display_mode_tt"},
	{T_WINDOW_SIZE, "window_size_tt"},
	{T_VSYNC, "vsync_tt"},
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
	selection_tree_row{T_VSYNC_C},
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
	{"Ctrl+Z"_kc, T_REVERT},
	{"Ctrl+S"_kc, T_APPLY},
	{"Enter"_kc, T_APPLY},
	{"Enter"_kc, T_EXIT},
};

constexpr glm::vec2 DISPLAY_MODE_START_POS{1050, 158.5f};
constexpr glm::vec2 WINDOW_SIZE_START_POS{1050, 233.5f};
constexpr glm::vec2 VSYNC_START_POS{1050, 308.5f};
constexpr glm::vec2 MSAA_START_POS{1050, 383.5f};
constexpr glm::vec2 PRIMARY_HUE_START_POS{1050, 458.5f};
constexpr glm::vec2 SECONDARY_HUE_START_POS{1050, 533.5f};
constexpr glm::vec2 SFX_VOLUME_START_POS{1050, 608.5f};
constexpr glm::vec2 MUSIC_VOLUME_START_POS{1050, 683.5f};
constexpr glm::vec2 LANGUAGE_START_POS{1050, 758.5f};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> DISPLAY_MODE_C_MOVE_IN{tween::CUBIC, DISPLAY_MODE_START_POS, {985, DISPLAY_MODE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> WINDOW_SIZE_D_MOVE_IN{tween::CUBIC, WINDOW_SIZE_START_POS, {765, WINDOW_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> WINDOW_SIZE_C_MOVE_IN{tween::CUBIC, WINDOW_SIZE_START_POS, {875, WINDOW_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> WINDOW_SIZE_I_MOVE_IN{tween::CUBIC, WINDOW_SIZE_START_POS, {985, WINDOW_SIZE_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> VSYNC_C_MOVE_IN{tween::CUBIC, VSYNC_START_POS, {985, VSYNC_START_POS.y}, 0.5_s};
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

settings_state::settings_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_SETTINGS}, m_pending{g_settings}
{
}

//

void settings_state::set_up_ui()
{
	using enum tr::align;

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	for (usize i = 0; i < LABELS.size(); ++i) {
		const label_info& label{LABELS[i]};
		const tweener<glm::vec2> move_in{tween::CUBIC, {-50, 158.5f + i * 75}, {15, 158.5f + i * 75}, 0.5_s};
		const tr::rgba8 color{label.tag == T_WINDOW_SIZE && m_pending.display_mode == display_mode::FULLSCREEN ? "505050A0"_rgba8
																											   : "A0A0A0A0"_rgba8};
		m_ui.emplace<label_widget>(label.tag, move_in, CENTER_LEFT, 0.5_s, tag_tooltip_loc{LABELS[i].tooltip}, tag_loc{label.tag},
								   text_style::NORMAL, 48, color);
	}

	m_ui.emplace<text_button_widget>(T_DISPLAY_MODE_C, DISPLAY_MODE_C_MOVE_IN, CENTER_RIGHT, 0.5_s, NO_TOOLTIP, display_mode_c_text,
									 font::LANGUAGE, 48, interactible, on_display_mode_c, sound::CONFIRM);
	m_ui.emplace<numeric_arrow_widget<u16, dec, MIN_WINDOW_SIZE, 1, 10, 100>>(T_WINDOW_SIZE_D, WINDOW_SIZE_D_MOVE_IN, CENTER_LEFT, 0.5_s,
																			  window_size_dc_interactible, m_pending.window_size);
	m_ui.emplace<numeric_input_widget<u16, 4, "{}", "{}">>(T_WINDOW_SIZE_C, WINDOW_SIZE_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
														   m_pending.window_size, window_size_dc_interactible,
														   [](u16 v) { return std::clamp(v, MIN_WINDOW_SIZE, max_window_size()); });
	m_ui.emplace<arrow_widget>(T_WINDOW_SIZE_I, WINDOW_SIZE_I_MOVE_IN, CENTER_RIGHT, 0.5_s, true, window_size_i_interactible,
							   on_window_size_i);
	m_ui.emplace<text_button_widget>(T_VSYNC_C, VSYNC_C_MOVE_IN, CENTER_RIGHT, 0.5_s, NO_TOOLTIP, vsync_c_text, font::LANGUAGE_PREVIEW, 48,
									 interactible, on_vsync_c, sound::CONFIRM);
	m_ui.emplace<arrow_widget>(T_MSAA_D, MSAA_D_MOVE_IN, CENTER_LEFT, 0.5_s, false, msaa_d_interactible, on_msaa_d);
	m_ui.emplace<label_widget>(T_MSAA_C, MSAA_C_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, msaa_c_text, text_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_MSAA_I, MSAA_I_MOVE_IN, CENTER_RIGHT, 0.5_s, true, msaa_i_interactible, on_msaa_i);
	m_ui.emplace<arrow_widget>(T_PRIMARY_HUE_D, PRIMARY_HUE_D_MOVE_IN, CENTER_LEFT, 0.5_s, false, interactible, on_primary_hue_d);
	m_ui.emplace<numeric_input_widget<u16, 3, "{}", "{}">>(T_PRIMARY_HUE_C, PRIMARY_HUE_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
														   m_pending.primary_hue, interactible, [](int v) { return u16(v % 360); });
	m_ui.emplace<arrow_widget>(T_PRIMARY_HUE_I, PRIMARY_HUE_I_MOVE_IN, CENTER_RIGHT, 0.5_s, true, interactible, on_primary_hue_i);
	m_ui.emplace<color_preview_widget>(T_PRIMARY_HUE_PREVIEW, PRIMARY_HUE_PREVIEW_MOVE_IN, CENTER_RIGHT, 0.5_s, m_pending.primary_hue);
	m_ui.emplace<arrow_widget>(T_SECONDARY_HUE_D, SECONDARY_HUE_D_MOVE_IN, CENTER_LEFT, 0.5_s, false, interactible, on_secondary_hue_d);
	m_ui.emplace<numeric_input_widget<u16, 3, "{}", "{}">>(T_SECONDARY_HUE_C, SECONDARY_HUE_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
														   m_pending.secondary_hue, interactible, [](int v) { return u16(v % 360); });
	m_ui.emplace<arrow_widget>(T_SECONDARY_HUE_I, SECONDARY_HUE_I_MOVE_IN, CENTER_RIGHT, 0.5_s, true, interactible, on_secondary_hue_i);
	m_ui.emplace<color_preview_widget>(T_SECONDARY_HUE_PREVIEW, SECONDARY_HUE_PREVIEW_MOVE_IN, CENTER_RIGHT, 0.5_s,
									   m_pending.secondary_hue);
	m_ui.emplace<numeric_arrow_widget<u8, dec, 0, 1, 10, 25>>(T_SFX_VOLUME_D, SFX_VOLUME_D_MOVE_IN, CENTER_LEFT, 0.5_s, interactible,
															  m_pending.sfx_volume);
	m_ui.emplace<numeric_input_widget<u8, 3, "{}%", "{}%">>(T_SFX_VOLUME_C, SFX_VOLUME_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
															m_pending.sfx_volume, interactible, clamp_validation<u8, 0, 100>{});
	m_ui.emplace<numeric_arrow_widget<u8, inc, 100, 1, 10, 25>>(T_SFX_VOLUME_I, SFX_VOLUME_I_MOVE_IN, CENTER_RIGHT, 0.5_s, interactible,
																m_pending.sfx_volume);
	m_ui.emplace<numeric_arrow_widget<u8, dec, 0, 1, 10, 25>>(T_MUSIC_VOLUME_D, MUSIC_VOLUME_D_MOVE_IN, CENTER_LEFT, 0.5_s, interactible,
															  m_pending.music_volume);
	m_ui.emplace<numeric_input_widget<u8, 3, "{}%", "{}%">>(T_MUSIC_VOLUME_C, MUSIC_VOLUME_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
															m_pending.music_volume, interactible, clamp_validation<u8, 0, 100>{});
	m_ui.emplace<numeric_arrow_widget<u8, inc, 100, 1, 10, 25>>(T_MUSIC_VOLUME_I, MUSIC_VOLUME_I_MOVE_IN, CENTER_RIGHT, 0.5_s, interactible,
																m_pending.music_volume);
	m_ui.emplace<text_button_widget>(T_LANGUAGE_C, LANGUAGE_C_MOVE_IN, CENTER_RIGHT, 0.5_s, NO_TOOLTIP, language_c_text,
									 font::LANGUAGE_PREVIEW, 48, language_c_interactible, on_language_c, sound::CONFIRM);
	for (usize i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const tweener<glm::vec2> move_in{tween::CUBIC, BOTTOM_START_POS, {500, 1000 - 50 * BOTTOM_BUTTONS.size() + (i + 1) * 50}, 0.5_s};
		const sound sound{i == 1 ? sound::CONFIRM : sound::CANCEL};
		m_ui.emplace<text_button_widget>(BOTTOM_BUTTONS[i], move_in, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{BOTTOM_BUTTONS[i]},
										 font::LANGUAGE, 48, BOTTOM_STATUS_CALLBACKS[i], BOTTOM_ACTION_CALLBACKS[i], sound);
	}
}

next_state settings_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_SETTINGS:
		return tr::KEEP_STATE;
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
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

//

std::string settings_state::display_mode_c_text()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return std::string{g_loc[self.m_pending.display_mode == display_mode::FULLSCREEN ? "fullscreen" : "windowed"]};
}

std::string settings_state::vsync_c_text()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return std::string{g_loc[self.m_pending.vsync ? "on" : "off"]};
}

std::string settings_state::msaa_c_text()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_pending.msaa == NO_MSAA ? "--" : TR_FMT::format("x{}", self.m_pending.msaa);
}

std::string settings_state::language_c_text()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return g_languages.contains(self.m_pending.language) ? g_languages[self.m_pending.language].name : "???";
}

bool settings_state::interactible()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_substate == substate::IN_SETTINGS;
}

bool settings_state::window_size_dc_interactible()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_substate != substate::ENTERING_TITLE && self.m_pending.display_mode != display_mode::FULLSCREEN;
}

bool settings_state::window_size_i_interactible()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_substate != substate::ENTERING_TITLE && self.m_pending.display_mode != display_mode::FULLSCREEN &&
		   self.m_pending.window_size < max_window_size();
}

bool settings_state::msaa_d_interactible()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_substate != substate::ENTERING_TITLE && self.m_pending.msaa != NO_MSAA;
}

bool settings_state::msaa_i_interactible()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_substate != substate::ENTERING_TITLE && self.m_pending.msaa != tr::sys::max_msaa();
}

bool settings_state::revert_apply_interactible()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_substate != substate::ENTERING_TITLE && self.m_pending != g_settings;
}

bool settings_state::exit_interactible()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_substate != substate::ENTERING_TITLE && self.m_pending == g_settings;
}

bool settings_state::language_c_interactible()
{
	const settings_state& self{g_state_machine.get<settings_state>()};

	return self.m_substate != substate::ENTERING_TITLE && (g_languages.size() >= 2 - (!g_languages.contains(self.m_pending.language)));
}

void settings_state::on_display_mode_c()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	switch (self.m_pending.display_mode) {
	case display_mode::WINDOWED:
		self.m_pending.display_mode = display_mode::FULLSCREEN;
		self.m_ui.as<label_widget>(T_WINDOW_SIZE).color.change(tween::LERP, "505050A0"_rgba8, 0.1_s);
		break;
	case display_mode::FULLSCREEN:
		self.m_pending.display_mode = display_mode::WINDOWED;
		self.m_ui.as<label_widget>(T_WINDOW_SIZE).color.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
		break;
	}
}

void settings_state::on_window_size_i()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending.window_size = std::min(max_window_size(), u16(self.m_pending.window_size + engine::keymods_choose(1, 10, 100)));
}

void settings_state::on_vsync_c()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending.vsync = !self.m_pending.vsync;
}

void settings_state::on_msaa_d()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending.msaa = self.m_pending.msaa == 2 ? NO_MSAA : u8(self.m_pending.msaa / 2);
}

void settings_state::on_msaa_i()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending.msaa = self.m_pending.msaa == NO_MSAA ? 2 : u8(self.m_pending.msaa * 2);
}

void settings_state::on_primary_hue_d()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending.primary_hue = u16((self.m_pending.primary_hue - engine::keymods_choose(1, 10, 100) + 360) % 360);
}

void settings_state::on_primary_hue_i()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending.primary_hue = u16((self.m_pending.primary_hue + engine::keymods_choose(1, 10, 100)) % 360);
}

void settings_state::on_secondary_hue_d()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending.secondary_hue = u16((self.m_pending.secondary_hue - engine::keymods_choose(1, 10, 100) + 360) % 360);
}

void settings_state::on_secondary_hue_i()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending.secondary_hue = u16((self.m_pending.secondary_hue + engine::keymods_choose(1, 10, 100)) % 360);
}

void settings_state::on_language_c()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	std::map<language_code, language_info>::iterator it{g_languages.find(self.m_pending.language)};
	if (it == g_languages.end() || ++it == g_languages.end()) {
		it = g_languages.begin();
	}
	self.m_pending.language = it->first;
	g_text_engine.reload_language_preview_font(self.m_pending);
}

void settings_state::on_revert()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_pending = g_settings;
	g_text_engine.reload_language_preview_font(self.m_pending);
	const tr::rgba8 window_size_color{self.m_pending.display_mode == display_mode::WINDOWED ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8};
	self.m_ui.as<label_widget>(T_WINDOW_SIZE).color.change(tween::LERP, window_size_color, 0.1_s);
}

void settings_state::on_apply()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	const settings old{g_settings};
	g_settings = self.m_pending;
	if (engine::restart_required(old)) {
		self.m_ui.release_graphical_resources();
	}
	else if (self.m_pending.vsync != old.vsync) {
		tr::sys::set_window_vsync(self.m_pending.vsync ? tr::sys::vsync::ADAPTIVE : tr::sys::vsync::DISABLED);
	}

	if (old.language != g_settings.language) {
		load_localization();
	}
	if (!g_languages.contains(old.language) || g_languages[old.language].font != g_languages[g_settings.language].font) {
		self.m_ui.release_graphical_resources();
		g_text_engine.set_language_font();
	}
	engine::apply_settings(old);
}

void settings_state::on_exit()
{
	settings_state& self{g_state_machine.get<settings_state>()};

	self.m_substate = substate::ENTERING_TITLE;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<title_state>(self.m_game);
}