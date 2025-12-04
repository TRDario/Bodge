#include "../../include/state/state.hpp"
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
constexpr tag T_LIFE_FRAGMENT_SPAWN_INTERVAL{"life_fragment_spawn_interval"};
constexpr tag T_LIFE_FRAGMENT_SPAWN_INTERVAL_D{"life_fragment_spawn_interval_d"};
constexpr tag T_LIFE_FRAGMENT_SPAWN_INTERVAL_C{"life_fragment_spawn_interval_c"};
constexpr tag T_LIFE_FRAGMENT_SPAWN_INTERVAL_I{"life_fragment_spawn_interval_i"};
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
	{T_LIFE_FRAGMENT_SPAWN_INTERVAL, "life_fragment_spawn_interval_tt"},
	{T_HITBOX_RADIUS, "hitbox_size_tt"},
	{T_INERTIA_FACTOR, "inertia_factor_tt"},
}};

constexpr std::array<tag, 13> RIGHT_WIDGETS{
	T_STARTING_LIVES_D, T_STARTING_LIVES_C, T_STARTING_LIVES_I,
	T_SPAWN_LIFE_FRAGMENTS_C,
	T_LIFE_FRAGMENT_SPAWN_INTERVAL_D, T_LIFE_FRAGMENT_SPAWN_INTERVAL_C, T_LIFE_FRAGMENT_SPAWN_INTERVAL_I,
	T_HITBOX_RADIUS_D,  T_HITBOX_RADIUS_C, T_HITBOX_RADIUS_I,
	T_INERTIA_FACTOR_D, T_INERTIA_FACTOR_C, T_INERTIA_FACTOR_I,
};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_STARTING_LIVES_D, T_STARTING_LIVES_C, T_STARTING_LIVES_I},
	selection_tree_row{T_SPAWN_LIFE_FRAGMENTS_C},
	selection_tree_row{T_LIFE_FRAGMENT_SPAWN_INTERVAL_D, T_LIFE_FRAGMENT_SPAWN_INTERVAL_C, T_LIFE_FRAGMENT_SPAWN_INTERVAL_I},
	selection_tree_row{T_HITBOX_RADIUS_D, T_HITBOX_RADIUS_C, T_HITBOX_RADIUS_I},
	selection_tree_row{T_INERTIA_FACTOR_D, T_INERTIA_FACTOR_C, T_INERTIA_FACTOR_I},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{"Escape"_kc, T_EXIT},
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
constexpr tweener<glm::vec2> LIFE_FRAGMENT_SPAWN_INTERVAL_D_MOVE_IN{tween::CUBIC, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {765, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> LIFE_FRAGMENT_SPAWN_INTERVAL_C_MOVE_IN{tween::CUBIC, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {875.5f, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s};
constexpr tweener<glm::vec2> LIFE_FRAGMENT_SPAWN_INTERVAL_I_MOVE_IN{tween::CUBIC, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {985, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s};
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
	// TEXT CALLBACKS

	const text_callback spawn_life_fragments_c_tcb{
		[&slf = m_pending.player.spawn_life_fragments] { return std::string{g_loc[slf ? "on" : "off"]}; },
	};

	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_EDITOR; },
	};
	const status_callback starting_lives_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.starting_lives > 0; },
	};
	const status_callback starting_lives_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.starting_lives < 255; },
	};
	const status_callback life_fragment_spawn_interval_d_scb{[this] {
		return m_substate == substate::IN_EDITOR && m_pending.player.spawn_life_fragments &&
			   m_pending.player.life_fragment_spawn_interval > 15_s;
	}};
	const status_callback life_fragment_spawn_interval_c_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.spawn_life_fragments; },
	};
	const status_callback life_fragment_spawn_interval_i_scb{[this] {
		return m_substate == substate::IN_EDITOR && m_pending.player.spawn_life_fragments &&
			   m_pending.player.life_fragment_spawn_interval < 90_s;
	}};
	const status_callback hitbox_radius_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.hitbox_radius > 1.0f; },
	};
	const status_callback hitbox_radius_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.hitbox_radius < 100.0f; },
	};
	const status_callback inertia_factor_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.inertia_factor > 0.0f; },
	};
	const status_callback inertia_factor_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.inertia_factor < 0.99f; },
	};

	// ACTION CALLBACKS

	const action_callback starting_lives_d_acb{
		[&sl = m_pending.player.starting_lives] { sl = std::max(int(sl - keymods_choose(1, 5, 10)), 0); },
	};
	const action_callback starting_lives_i_acb{
		[&sl = m_pending.player.starting_lives] { sl = std::min(sl + keymods_choose(1, 5, 10), 255); },
	};
	const action_callback spawn_life_fragments_c_acb{
		[&slf = m_pending.player.spawn_life_fragments, this] {
			slf = !slf;
			if (slf) {
				m_ui.as<label_widget>(T_LIFE_FRAGMENT_SPAWN_INTERVAL).color.change(tween::LERP, GRAY, 0.1_s);
			}
			else {
				m_ui.as<label_widget>(T_LIFE_FRAGMENT_SPAWN_INTERVAL).color.change(tween::LERP, DISABLED_GRAY, 0.1_s);
			}
		},
	};
	const action_callback life_fragment_spawn_interval_d_acb{
		[&lfsi = m_pending.player.life_fragment_spawn_interval] { lfsi = std::max(lfsi - keymods_choose(0.1_s, 1_s, 10_s), 15_s); },
	};
	const action_callback life_fragment_spawn_interval_i_acb{
		[&lfsi = m_pending.player.life_fragment_spawn_interval] { lfsi = std::min(lfsi + keymods_choose(0.1_s, 1_s, 10_s), 90_s); },
	};
	const action_callback hitbox_radius_d_acb{
		[&hr = m_pending.player.hitbox_radius] { hr = std::max(hr - keymods_choose(1, 5, 10), 1.0f); },
	};
	const action_callback hitbox_radius_i_acb{
		[&hr = m_pending.player.hitbox_radius] { hr = std::min(hr + keymods_choose(1, 5, 10), 100.0f); },
	};
	const action_callback inertia_factor_d_acb{
		[&in = m_pending.player.inertia_factor] { in = std::max(in - keymods_choose(0.01f, 0.05f, 0.1f), 0.0f); },
	};
	const action_callback inertia_factor_i_acb{
		[&in = m_pending.player.inertia_factor] { in = std::min(in + keymods_choose(0.01f, 0.05f, 0.1f), 0.99f); },
	};
	const action_callback exit_acb{
		[this] {
			m_substate = substate::EXITING;
			m_timer = 0;
			set_up_exit_animation();
			m_next_state = make_async<gamemode_designer_state>(m_game, m_pending, true);
		},
	};

	// VALIDATION CALLBACKS

	const validation_callback<u32> starting_lives_c_vcb{
		[](u32 v) { return std::clamp(v, 0_u32, 255_u32); },
	};
	const validation_callback<ticks> life_fragment_spawn_interval_c_vcb{
		[](ticks v) { return std::clamp(v, 15_s, 100_s); },
	};
	const validation_callback<float> hitbox_radius_c_vcb{
		[](float v) { return std::clamp(v, 1.0f, 100.0f); },
	};
	const validation_callback<float> inertia_factor_c_vcb{
		[](float v) { return std::clamp(v, 0.0f, 0.99f); },
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE}, text_style::NORMAL,
							   64);
	m_ui.emplace<label_widget>(T_SUBTITLE, SUBTITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SUBTITLE},
							   text_style::NORMAL, 32);
	m_ui.emplace<arrow_widget>(T_STARTING_LIVES_D, STARTING_LIVES_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, starting_lives_d_scb,
							   starting_lives_d_acb);
	m_ui.emplace<numeric_input_widget<u8, 3, "{}", "{}">>(T_STARTING_LIVES_C, STARTING_LIVES_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48, m_ui,
														  m_pending.player.starting_lives, scb, starting_lives_c_vcb);
	m_ui.emplace<arrow_widget>(T_STARTING_LIVES_I, STARTING_LIVES_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, starting_lives_i_scb,
							   starting_lives_i_acb);
	m_ui.emplace<text_button_widget>(T_SPAWN_LIFE_FRAGMENTS_C, SPAWN_LIFE_FRAGMENTS_C_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 spawn_life_fragments_c_tcb, font::LANGUAGE, 48, scb, spawn_life_fragments_c_acb, sound::CONFIRM);
	m_ui.emplace<arrow_widget>(T_LIFE_FRAGMENT_SPAWN_INTERVAL_D, LIFE_FRAGMENT_SPAWN_INTERVAL_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s,
							   false, life_fragment_spawn_interval_d_scb, life_fragment_spawn_interval_d_acb);
	m_ui.emplace<interval_input_widget<4>>(T_LIFE_FRAGMENT_SPAWN_INTERVAL_C, LIFE_FRAGMENT_SPAWN_INTERVAL_C_MOVE_IN, tr::align::CENTER,
										   0.5_s, 48, m_ui, m_pending.player.life_fragment_spawn_interval,
										   life_fragment_spawn_interval_c_scb, life_fragment_spawn_interval_c_vcb);
	m_ui.emplace<arrow_widget>(T_LIFE_FRAGMENT_SPAWN_INTERVAL_I, LIFE_FRAGMENT_SPAWN_INTERVAL_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s,
							   true, life_fragment_spawn_interval_i_scb, life_fragment_spawn_interval_i_acb);
	m_ui.emplace<arrow_widget>(T_HITBOX_RADIUS_D, HITBOX_RADIUS_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, hitbox_radius_d_scb,
							   hitbox_radius_d_acb);
	m_ui.emplace<numeric_input_widget<float, 3, "{:.0f}", "{}">>(T_HITBOX_RADIUS_C, HITBOX_RADIUS_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48,
																 m_ui, m_pending.player.hitbox_radius, scb, hitbox_radius_c_vcb);
	m_ui.emplace<arrow_widget>(T_HITBOX_RADIUS_I, HITBOX_RADIUS_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, hitbox_radius_i_scb,
							   hitbox_radius_i_acb);
	m_ui.emplace<arrow_widget>(T_INERTIA_FACTOR_D, INERTIA_FACTOR_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, inertia_factor_d_scb,
							   inertia_factor_d_acb);
	m_ui.emplace<numeric_input_widget<float, 4, "{:.2f}", "{}">>(T_INERTIA_FACTOR_C, INERTIA_FACTOR_C_MOVE_IN, tr::align::CENTER, 0.5_s, 48,
																 m_ui, m_pending.player.inertia_factor, scb, inertia_factor_c_vcb);
	m_ui.emplace<arrow_widget>(T_INERTIA_FACTOR_I, INERTIA_FACTOR_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, inertia_factor_i_scb,
							   inertia_factor_i_acb);
	for (usize i = 0; i < LABELS.size(); ++i) {
		const label_info& label{LABELS[i]};
		const tweener<glm::vec2> move_in{tween::CUBIC, {-50, 375 + i * 75}, {15, 375 + i * 75}, 0.5_s};
		m_ui.emplace<label_widget>(label.tag, move_in, tr::align::CENTER_LEFT, 0.5_s, tooltip_loc_text_callback{LABELS[i].tooltip},
								   loc_text_callback{label.tag}, text_style::NORMAL, 48);
	}
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

tr::next_state player_settings_editor_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_EDITOR:
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
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