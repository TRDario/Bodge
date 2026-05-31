///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements player_settings_editor_state from state.hpp.                                                                               //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"gamemode_manager"};
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

// Left-hand side labels.
constexpr std::array<label_info, 5> LABELS{{
	{T_STARTING_LIVES, "starting_lives_tt"},
	{T_SPAWN_LIFE_FRAGMENTS, "spawn_life_fragments_tt"},
	{T_LIFE_FRAGMENT_SPAWN_INTERVAL, "life_fragment_spawn_interval_tt"},
	{T_HITBOX_RADIUS, "hitbox_size_tt"},
	{T_INERTIA_FACTOR, "inertia_factor_tt"},
}};

// Right-hand side interactible widgets.
constexpr std::array<tag, 13> RIGHT_WIDGETS{
	T_STARTING_LIVES_D, T_STARTING_LIVES_C, T_STARTING_LIVES_I,
	T_SPAWN_LIFE_FRAGMENTS_C,
	T_LIFE_FRAGMENT_SPAWN_INTERVAL_D, T_LIFE_FRAGMENT_SPAWN_INTERVAL_C, T_LIFE_FRAGMENT_SPAWN_INTERVAL_I,
	T_HITBOX_RADIUS_D,  T_HITBOX_RADIUS_C, T_HITBOX_RADIUS_I,
	T_INERTIA_FACTOR_D, T_INERTIA_FACTOR_C, T_INERTIA_FACTOR_I,
};

// Selection tree for the player settings editor menu.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_STARTING_LIVES_D, T_STARTING_LIVES_C, T_STARTING_LIVES_I},
	selection_tree_row{T_SPAWN_LIFE_FRAGMENTS_C},
	selection_tree_row{T_LIFE_FRAGMENT_SPAWN_INTERVAL_D, T_LIFE_FRAGMENT_SPAWN_INTERVAL_C, T_LIFE_FRAGMENT_SPAWN_INTERVAL_I},
	selection_tree_row{T_HITBOX_RADIUS_D, T_HITBOX_RADIUS_C, T_HITBOX_RADIUS_I},
	selection_tree_row{T_INERTIA_FACTOR_D, T_INERTIA_FACTOR_C, T_INERTIA_FACTOR_I},
	selection_tree_row{T_EXIT},
};

// Shortcut table for the player settings editor menu.
constexpr shortcut_table SHORTCUTS{
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT}, {"1"_kc, T_EXIT},
};

// Starting position for the starting lives right widgets.
constexpr glm::vec2 STARTING_LIVES_START_POS{1050, 375};
// Starting position for the spawn life fragments right widgets.
constexpr glm::vec2 SPAWN_LIFE_FRAGMENTS_START_POS{1050, STARTING_LIVES_START_POS.y + 75};
// Starting position for the life fragment spawn interval right widgets.
constexpr glm::vec2 LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS{1050, SPAWN_LIFE_FRAGMENTS_START_POS.y + 75};
// Starting position for the hitbox radius right widgets.
constexpr glm::vec2 HITBOX_RADIUS_START_POS{1050, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y + 75};
// Starting position for the inertia factor right widgets.
constexpr glm::vec2 INERTIA_FACTOR_START_POS{1050, HITBOX_RADIUS_START_POS.y + 75};

// clang-format on
/////////////////////////////////////////////////////// PLAYER SETTINGS EDITOR STATE //////////////////////////////////////////////////////

player_settings_editor_state::player_settings_editor_state(std::shared_ptr<playerless_game> game, gamemode_editor data, gamemode gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_EDITOR}
	, m_data{std::move(data)}
	, m_pending{std::move(gamemode)}
{
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = TITLE_POS,
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = 0_s,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_SUBTITLE, {
		.animation = {TOP_START_POS, {500, TITLE_POS.y + 64}, 0.5_s},
		.alignment = tr::align::TOP_CENTER,
		.text = localized_text{T_SUBTITLE},
		.font_size = 32
	});
	m_ui.emplace<arrow_widget>(T_STARTING_LIVES_D, {
		.animation = {STARTING_LIVES_START_POS, {765, STARTING_LIVES_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.player.starting_lives > 0; },
		.action = [&sl = m_pending.player.starting_lives] { sl = u8(std::max(int(sl - keymods_choose(1, 5, 10)), 0)); }
	});
	m_ui.emplace<numeric_input_widget<u8, 3>>(T_STARTING_LIVES_C,
		tweened_position{STARTING_LIVES_START_POS, {875.5f, STARTING_LIVES_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.player.starting_lives,
		[this] { return m_substate == substate::IN_EDITOR; },
		[](u32 v) { return std::clamp(v, 0_u32, 255_u32); }
	);
	m_ui.emplace<arrow_widget>(T_STARTING_LIVES_I, {
		.animation = {STARTING_LIVES_START_POS, {985, STARTING_LIVES_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.player.starting_lives < 255; },
		.action = [&sl = m_pending.player.starting_lives] { sl = u8(std::min(sl + keymods_choose(1, 5, 10), 255)); }
	});
	m_ui.emplace<text_button_widget>(T_SPAWN_LIFE_FRAGMENTS_C,
		tweened_position{SPAWN_LIFE_FRAGMENTS_START_POS, {875.5f, SPAWN_LIFE_FRAGMENTS_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		NO_TOOLTIP,
		[&slf = m_pending.player.spawn_life_fragments] { return std::string{g_loc[slf ? "on" : "off"]}; },
		font::LANGUAGE,
		48,
		[this] { return m_substate == substate::IN_EDITOR; },
		[this] { on_toggle_life_fragments(); },
		sound::CONFIRM
	);
	m_ui.emplace<arrow_widget>(T_LIFE_FRAGMENT_SPAWN_INTERVAL_D, {
		.animation = {LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {765, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] {
			return m_substate == substate::IN_EDITOR && m_pending.player.spawn_life_fragments &&
				   m_pending.player.life_fragment_spawn_interval > 15_s;
		},
		.action = [&lfsi = m_pending.player.life_fragment_spawn_interval] { lfsi = std::max(lfsi - keymods_choose(0.1_s, 1_s, 10_s), 15_s); }
	});
	m_ui.emplace<interval_input_widget<4>>(T_LIFE_FRAGMENT_SPAWN_INTERVAL_C,
		tweened_position{LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {875.5f, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.player.life_fragment_spawn_interval,
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.spawn_life_fragments; },
		[](ticks v) { return std::clamp(v, 15_s, 100_s); }
	);
	m_ui.emplace<arrow_widget>(T_LIFE_FRAGMENT_SPAWN_INTERVAL_I, {
		.animation = {LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS, {985, LIFE_FRAGMENT_SPAWN_INTERVAL_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] {
			return m_substate == substate::IN_EDITOR && m_pending.player.spawn_life_fragments &&
				m_pending.player.life_fragment_spawn_interval < 90_s;
		},
		.action = [&lfsi = m_pending.player.life_fragment_spawn_interval] { lfsi = std::min(lfsi + keymods_choose(0.1_s, 1_s, 10_s), 90_s); }
	});
	m_ui.emplace<arrow_widget>(T_HITBOX_RADIUS_D, {
		.animation = {HITBOX_RADIUS_START_POS, {765, HITBOX_RADIUS_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.player.hitbox_radius > 1.0f; },
		.action = [&hr = m_pending.player.hitbox_radius] { hr = std::max(hr - keymods_choose(1, 5, 10), 1.0f); }
	});
	m_ui.emplace<numeric_input_widget<float, 3, "{:.0f}">>(T_HITBOX_RADIUS_C,
		tweened_position{HITBOX_RADIUS_START_POS, {875.5f, HITBOX_RADIUS_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.player.hitbox_radius,
		[this] { return m_substate == substate::IN_EDITOR; },
		[](float v) { return std::clamp(v, 1.0f, 100.0f); }
	);
	m_ui.emplace<arrow_widget>(T_HITBOX_RADIUS_I, {
		.animation = {HITBOX_RADIUS_START_POS, {985, HITBOX_RADIUS_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.player.hitbox_radius < 100.0f; },
		.action = [&hr = m_pending.player.hitbox_radius] { hr = std::min(hr + keymods_choose(1, 5, 10), 100.0f); }
	});
	m_ui.emplace<arrow_widget>(T_INERTIA_FACTOR_D, {
		.animation = {INERTIA_FACTOR_START_POS, {765, INERTIA_FACTOR_START_POS.y}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.player.inertia_factor > 0.0f; },
		.action = [&in = m_pending.player.inertia_factor] { in = std::max(in - keymods_choose(0.01f, 0.05f, 0.1f), 0.0f); }
	});
	m_ui.emplace<numeric_input_widget<float, 4, "{:.2f}">>(T_INERTIA_FACTOR_C,
		tweened_position{INERTIA_FACTOR_START_POS, {875.5f, INERTIA_FACTOR_START_POS.y}, 0.5_s},
		tr::align::CENTER,
		0.5_s,
		48,
		m_ui,
		m_pending.player.inertia_factor,
		[this] { return m_substate == substate::IN_EDITOR; },
		[](float v) { return std::clamp(v, 0.0f, 0.99f); }
	);
	m_ui.emplace<arrow_widget>(T_INERTIA_FACTOR_I, {
		.animation = {INERTIA_FACTOR_START_POS, {985, INERTIA_FACTOR_START_POS.y}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_EDITOR && m_pending.player.inertia_factor < 0.99f; },
		.action = [&in = m_pending.player.inertia_factor] { in = std::min(in + keymods_choose(0.01f, 0.05f, 0.1f), 0.99f); }
	});

	for (usize i = 0; i < LABELS.size(); ++i) {
		m_ui.emplace<label_widget>(LABELS[i].tag, {
			.animation = {{-50, 375 + i * 75}, {15, 375 + i * 75}, 0.5_s},
			.alignment = tr::align::CENTER_LEFT,
			.tooltip_text = localized_text{LABELS[i].tooltip},
			.text = localized_text{LABELS[i].tag}
		});
	}

	m_ui.emplace<text_button_widget>(T_EXIT,
		tweened_position{BOTTOM_START_POS, {500, 1000}, 0.5_s},
		tr::align::BOTTOM_CENTER,
		0.5_s,
		NO_TOOLTIP, localized_text{T_EXIT},
		font::LANGUAGE,
		48,
		[this] { return m_substate == substate::IN_EDITOR; },
		[this] { on_exit(); },
		sound::CANCEL
	);
	// clang-format on
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
	m_ui[T_SUBTITLE].move_and_hide(TOP_START_POS, 0.5_s);
	for (const label_info& label : LABELS) {
		m_ui[label.tag].move_x_and_hide(-50, 0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		m_ui[tag].move_x_and_hide(1050, 0.5_s);
	}
	m_ui[T_EXIT].move_and_hide(BOTTOM_START_POS, 0.5_s);
}

//

void player_settings_editor_state::on_toggle_life_fragments()
{
	m_pending.player.spawn_life_fragments = !m_pending.player.spawn_life_fragments;
	if (m_pending.player.spawn_life_fragments) {
		m_ui.as<label_widget>(T_LIFE_FRAGMENT_SPAWN_INTERVAL).tint.change(GRAY, 0.1_s);
	}
	else {
		m_ui.as<label_widget>(T_LIFE_FRAGMENT_SPAWN_INTERVAL).tint.change(DISABLED_GRAY, 0.1_s);
	}
}

void player_settings_editor_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<gamemode_editor_state>(m_game, m_data, m_pending, animate_subtitle::YES);
}