#include "../../include/state/state.hpp"
#include "../../include/system.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr tag T_TITLE{"gamemode_designer"};
constexpr tag T_SUBTITLE{"player_settings"};
constexpr tag T_STARTING_LIVES{"starting_lives"};
constexpr tag T_STARTING_LIVES_D{"starting_lives_d"};
constexpr tag T_STARTING_LIVES_C{"starting_lives_c"};
constexpr tag T_STARTING_LIVES_I{"starting_lives_i"};
constexpr tag T_SPAWN_LIFE_FRAGMENTS{"spawn_life_fragments"};
constexpr tag T_SPAWN_LIFE_FRAGMENTS_C{"spawn_life_fragments_c"};
constexpr tag T_FRAGMENT_SPAWN_INTERVAL{"life_fragment_spawn_interval"};
constexpr tag T_FRAGMENT_SPAWN_INTERVAL_D{"life_fragment_spawn_interval_d"};
constexpr tag T_FRAGMENT_SPAWN_INTERVAL_C{"life_fragment_spawn_interval_c"};
constexpr tag T_FRAGMENT_SPAWN_INTERVAL_I{"life_fragment_spawn_interval_i"};
constexpr tag T_HITBOX_RADIUS{"hitbox_radius"};
constexpr tag T_HITBOX_RADIUS_D{"hitbox_radius_d"};
constexpr tag T_HITBOX_RADIUS_C{"hitbox_radius_c"};
constexpr tag T_HITBOX_RADIUS_I{"hitbox_radius_i"};
constexpr tag T_INERTIA_FACTOR{"inertia_factor"};
constexpr tag T_INERTIA_FACTOR_D{"inertia_factor_d"};
constexpr tag T_INERTIA_FACTOR_C{"inertia_factor_c"};
constexpr tag T_INERTIA_FACTOR_I{"inertia_factor_i"};
constexpr tag T_EXIT{"exit"};

constexpr std::array<label_info, 5> LABELS{{
	{T_STARTING_LIVES, "starting_lives_tt"},
	{T_SPAWN_LIFE_FRAGMENTS, "spawn_life_fragments_tt"},
	{T_FRAGMENT_SPAWN_INTERVAL, "life_fragment_spawn_interval_tt"},
	{T_HITBOX_RADIUS, "hitbox_size_tt"},
	{T_INERTIA_FACTOR, "inertia_factor_tt"},
}};

constexpr std::array<tag, 13> RIGHT_WIDGETS{
	T_STARTING_LIVES_D, T_STARTING_LIVES_C, T_STARTING_LIVES_I,
	T_SPAWN_LIFE_FRAGMENTS_C,
	T_FRAGMENT_SPAWN_INTERVAL_D, T_FRAGMENT_SPAWN_INTERVAL_C, T_FRAGMENT_SPAWN_INTERVAL_I,
	T_HITBOX_RADIUS_D,  T_HITBOX_RADIUS_C, T_HITBOX_RADIUS_I,
	T_INERTIA_FACTOR_D, T_INERTIA_FACTOR_C, T_INERTIA_FACTOR_I,
};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_STARTING_LIVES_D, T_STARTING_LIVES_C, T_STARTING_LIVES_I},
	selection_tree_row{T_SPAWN_LIFE_FRAGMENTS_C},
	selection_tree_row{T_FRAGMENT_SPAWN_INTERVAL_D, T_FRAGMENT_SPAWN_INTERVAL_C, T_FRAGMENT_SPAWN_INTERVAL_I},
	selection_tree_row{T_HITBOX_RADIUS_D, T_HITBOX_RADIUS_C, T_HITBOX_RADIUS_I},
	selection_tree_row{T_INERTIA_FACTOR_D, T_INERTIA_FACTOR_C, T_INERTIA_FACTOR_I},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{"Enter"_kc, T_EXIT},
	{"1"_kc, T_EXIT},
};

constexpr glm::vec2 STARTING_LIVES_START_POS{1050, 375};
constexpr glm::vec2 SPAWN_LIFE_FRAGMENTS_START_POS{1050, 450};
constexpr glm::vec2 LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS{1050, 525};
constexpr glm::vec2 HITBOX_RADIUS_START_POS{1050, 600};
constexpr glm::vec2 INERTIA_FACTOR_START_POS{1050, 675};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{TITLE_POS};
constexpr tweener<glm::vec2> SUBTITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, {500, TITLE_POS.y + 64}, 0.5_s};
constexpr tweener<glm::vec2> STARTING_LIVES_D_MOVE_IN{tween::CUBIC, STARTING_LIVES_START_POS, {765, STARTING_LIVES_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> STARTING_LIVES_C_MOVE_IN{tween::CUBIC, STARTING_LIVES_START_POS, {875.5f, STARTING_LIVES_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> STARTING_LIVES_I_MOVE_IN{tween::CUBIC, STARTING_LIVES_START_POS, {985, STARTING_LIVES_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> SPAWN_LIFE_FRAGMENTS_C_MOVE_IN{tween::CUBIC, SPAWN_LIFE_FRAGMENTS_START_POS, {875.5f, SPAWN_LIFE_FRAGMENTS_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> FRAGMENT_SPAWN_INTERVAL_D_MOVE_IN{tween::CUBIC, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {765, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> FRAGMENT_SPAWN_INTERVAL_C_MOVE_IN{tween::CUBIC, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {875.5f, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> FRAGMENT_SPAWN_INTERVAL_I_MOVE_IN{tween::CUBIC, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {985, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> HITBOX_RADIUS_D_MOVE_IN{tween::CUBIC, HITBOX_RADIUS_START_POS, {765, HITBOX_RADIUS_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> HITBOX_RADIUS_C_MOVE_IN{tween::CUBIC, HITBOX_RADIUS_START_POS, {875.5f, HITBOX_RADIUS_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> HITBOX_RADIUS_I_MOVE_IN{tween::CUBIC, HITBOX_RADIUS_START_POS, {985, HITBOX_RADIUS_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INERTIA_FACTOR_D_MOVE_IN{tween::CUBIC, INERTIA_FACTOR_START_POS, {765, INERTIA_FACTOR_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INERTIA_FACTOR_C_MOVE_IN{tween::CUBIC, INERTIA_FACTOR_START_POS, {875.5f, INERTIA_FACTOR_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> INERTIA_FACTOR_I_MOVE_IN{tween::CUBIC, INERTIA_FACTOR_START_POS, {985, INERTIA_FACTOR_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

player_settings_editor_state::player_settings_editor_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_EDITOR}, m_pending{gamemode}
{
}

//

void player_settings_editor_state::set_up_ui()
{
	using enum tr::align;

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 0, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_SUBTITLE, SUBTITLE_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_SUBTITLE}, text_style::NORMAL, 32);
	m_ui.emplace<numeric_arrow_widget<u8, dec, 0, 1, 5, 10>>(T_STARTING_LIVES_D, STARTING_LIVES_D_MOVE_IN, CENTER_LEFT, 0.5_s, interactible,
															 m_pending.player.starting_lives);
	m_ui.emplace<numeric_input_widget<u8, 3, "{}", "{}">>(T_STARTING_LIVES_C, STARTING_LIVES_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
														  m_pending.player.starting_lives, interactible, clamp_validation<u32, 0, 255>{});
	m_ui.emplace<numeric_arrow_widget<u8, inc, 255, 1, 5, 10>>(T_STARTING_LIVES_I, STARTING_LIVES_I_MOVE_IN, CENTER_RIGHT, 0.5_s,
															   interactible, m_pending.player.starting_lives);
	m_ui.emplace<text_button_widget>(T_SPAWN_LIFE_FRAGMENTS_C, SPAWN_LIFE_FRAGMENTS_C_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP,
									 spawn_life_fragments_c_text, font::LANGUAGE, 48, interactible, on_spawn_life_fragments_c,
									 sound::CONFIRM);
	m_ui.emplace<numeric_arrow_widget<ticks, dec, 15_s, 0.1_s, 1_s, 10_s>>(T_FRAGMENT_SPAWN_INTERVAL_D, FRAGMENT_SPAWN_INTERVAL_D_MOVE_IN,
																		   CENTER_LEFT, 0.5_s, fragment_spawn_interval_interactible,
																		   m_pending.player.life_fragment_spawn_interval);
	m_ui.emplace<interval_input_widget<4>>(T_FRAGMENT_SPAWN_INTERVAL_C, FRAGMENT_SPAWN_INTERVAL_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
										   m_pending.player.life_fragment_spawn_interval, fragment_spawn_interval_interactible,
										   clamp_validation<ticks, 15_s, 100_s>{});
	m_ui.emplace<numeric_arrow_widget<ticks, inc, 90_s, 0.1_s, 1_s, 10_s>>(T_FRAGMENT_SPAWN_INTERVAL_I, FRAGMENT_SPAWN_INTERVAL_I_MOVE_IN,
																		   CENTER_RIGHT, 0.5_s, fragment_spawn_interval_interactible,
																		   m_pending.player.life_fragment_spawn_interval);
	m_ui.emplace<numeric_arrow_widget<float, dec, 1.0f, 1.0f, 5.0f, 10.0f>>(T_HITBOX_RADIUS_D, HITBOX_RADIUS_D_MOVE_IN, CENTER_LEFT, 0.5_s,
																			interactible, m_pending.player.hitbox_radius);
	m_ui.emplace<numeric_input_widget<float, 3, "{:.0f}", "{}">>(T_HITBOX_RADIUS_C, HITBOX_RADIUS_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
																 m_pending.player.hitbox_radius, interactible,
																 clamp_validation<float, 1.0f, 100.0f>{});
	m_ui.emplace<numeric_arrow_widget<float, inc, 100.0f, 1.0f, 5.0f, 10.0f>>(T_HITBOX_RADIUS_I, HITBOX_RADIUS_I_MOVE_IN, CENTER_RIGHT,
																			  0.5_s, interactible, m_pending.player.hitbox_radius);
	m_ui.emplace<numeric_arrow_widget<float, dec, 0.0f, 0.01f, 0.05f, 0.1f>>(T_INERTIA_FACTOR_D, INERTIA_FACTOR_D_MOVE_IN, CENTER_LEFT,
																			 0.5_s, interactible, m_pending.player.inertia_factor);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.2f}", "{}">>(T_INERTIA_FACTOR_C, INERTIA_FACTOR_C_MOVE_IN, CENTER, 0.5_s, 48, m_ui,
																 m_pending.player.inertia_factor, interactible,
																 clamp_validation<float, 0.0f, 0.99f>{});
	m_ui.emplace<numeric_arrow_widget<float, inc, 0.99f, 0.01f, 0.05f, 0.1f>>(T_INERTIA_FACTOR_I, INERTIA_FACTOR_I_MOVE_IN, CENTER_RIGHT,
																			  0.5_s, interactible, m_pending.player.inertia_factor);
	for (usize i = 0; i < LABELS.size(); ++i) {
		const label_info& label{LABELS[i]};
		const tweener<glm::vec2> move_in{tween::CUBIC, {-50, 375 + i * 75}, {15, 375 + i * 75}, 0.5_s};
		m_ui.emplace<label_widget>(label.tag, move_in, CENTER_LEFT, 0.5_s, tag_tooltip_loc{LABELS[i].tooltip}, tag_loc{label.tag},
								   text_style::NORMAL, 48);
	}
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_EXIT}, font::LANGUAGE, 48,
									 interactible, on_exit, sound::CANCEL);
}

next_state player_settings_editor_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_EDITOR:
		return tr::KEEP_STATE;
	case substate::EXITING:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
	}
}

//

void player_settings_editor_state::set_up_exit_animation()
{
	widget& subtitle{m_ui[T_SUBTITLE]};
	widget& exit{m_ui[T_EXIT]};
	subtitle.pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	subtitle.hide(0.5_s);
	for (tag tag : tr::project(LABELS, &label_info::tag)) {
		widget& widget{m_ui[tag]};
		widget.pos.change(tween::CUBIC, {-50, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		widget& widget{m_ui[tag]};
		widget.pos.change(tween::CUBIC, {1050, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	exit.pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	exit.hide(0.5_s);
}

//

std::string player_settings_editor_state::spawn_life_fragments_c_text()
{
	const player_settings_editor_state& self{g_state_machine.get<player_settings_editor_state>()};

	return std::string{g_loc[self.m_pending.player.spawn_life_fragments ? "on" : "off"]};
}

bool player_settings_editor_state::interactible()
{
	const player_settings_editor_state& self{g_state_machine.get<player_settings_editor_state>()};

	return self.m_substate == substate::IN_EDITOR;
}

bool player_settings_editor_state::fragment_spawn_interval_interactible()
{
	const player_settings_editor_state& self{g_state_machine.get<player_settings_editor_state>()};

	return self.m_substate == substate::IN_EDITOR && self.m_pending.player.spawn_life_fragments;
}

void player_settings_editor_state::on_spawn_life_fragments_c()
{
	player_settings_editor_state& self{g_state_machine.get<player_settings_editor_state>()};

	self.m_pending.player.spawn_life_fragments = !self.m_pending.player.spawn_life_fragments;
	if (self.m_pending.player.spawn_life_fragments) {
		self.m_ui.as<label_widget>(T_FRAGMENT_SPAWN_INTERVAL).color.change(tween::LERP, "A0A0A0A0"_rgba8, 0.1_s);
	}
	else {
		self.m_ui.as<label_widget>(T_FRAGMENT_SPAWN_INTERVAL).color.change(tween::LERP, "505050A0"_rgba8, 0.1_s);
	}
}

void player_settings_editor_state::on_exit()
{
	player_settings_editor_state& self{g_state_machine.get<player_settings_editor_state>()};

	self.m_substate = substate::EXITING;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<gamemode_designer_state>(std::shared_ptr<playerless_game>{self.m_game}, self.m_pending, true);
}