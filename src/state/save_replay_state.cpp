#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"save_replay"};
constexpr tag T_NAME{"name"};
constexpr tag T_NAME_INPUT{"name_input"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_DESCRIPTION_INPUT{"description_input"};
constexpr tag T_SAVE{"save"};
constexpr tag T_DISCARD{"discard"};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_NAME_INPUT},
	selection_tree_row{T_DESCRIPTION_INPUT},
	selection_tree_row{T_SAVE},
	selection_tree_row{T_DISCARD},
};

constexpr shortcut_table SHORTCUTS{
	{"Enter"_kc, T_SAVE},
	{"S"_kc, T_SAVE},
	{"1"_kc, T_SAVE},
	{"Escape"_kc, T_DISCARD},
	{"C"_kc, T_DISCARD},
	{"2"_kc, T_DISCARD},
};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweened_position NAME_MOVE_IN{{400, 200}, {500, 200}, 0.5_s};
constexpr tweened_position NAME_INPUT_MOVE_IN{{400, 235}, {500, 235}, 0.5_s};
constexpr tweened_position DESCRIPTION_MOVE_IN{{600, 440}, {500, 440}, 0.5_s};
constexpr tweened_position DESCRIPTION_INPUT_MOVE_IN{{600, 475}, {500, 475}, 0.5_s};
constexpr tweened_position SAVE_MOVE_IN{BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweened_position DISCARD_MOVE_IN{BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on
//////////////////////////////////////////////////////////// SAVE REPLAY STATE ////////////////////////////////////////////////////////////

save_replay_state::save_replay_state(std::shared_ptr<game> game, save_screen_flags flags)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), update_game(bool(flags & save_screen_flags::GAME_OVER))}
	, m_substate{substate_base::SAVING_REPLAY | flags}
	, m_replay{((active_game&)*m_game).replay.header()}
{
	// STATUS CALLBACKS

	const status_callback scb{[this] { return to_base(m_substate) == substate_base::SAVING_REPLAY; }};
	const status_callback save_scb{[this] {
		return to_base(m_substate) == substate_base::SAVING_REPLAY && !m_ui.as<line_input_widget<20>>(T_NAME_INPUT).buffer.empty();
	}};

	// ACTION CALLBACKS

	const action_callback name_acb{[this] { m_ui.select_next_widget(); }};
	const action_callback save_acb{[this] {
		const score_flags flags{!m_game->game_over(), g_cli_settings.game_speed != 1.0f};
		const auto& description{m_ui.as<multiline_input_widget<255>>(T_DESCRIPTION_INPUT).buffer};
		const auto& name{m_ui.as<line_input_widget<20>>("name_input").buffer};
		active_game& game{(active_game&)*m_game};

		m_substate = substate_base::EXITING | to_flags(m_substate);
		m_elapsed = 0;
		set_up_exit_animation();
		game.replay.set_header(score_entry{description, current_timestamp(), game.final_score(), game.final_time(), flags}, name);
		game.replay.save_to_file();
		if (!(to_flags(m_substate) & save_screen_flags::RESTARTING)) {
			m_next_state = make_async<title_state>();
		}
		else {
			m_next_state = make_game_state_async<active_game>(game_type::REGULAR, fade_in::YES, m_game->gamemode());
		}
	}};
	const action_callback discard_acb{[this] {
		m_substate = substate_base::EXITING | to_flags(m_substate);
		m_elapsed = 0;
		set_up_exit_animation();
		if (!(to_flags(m_substate) & save_screen_flags::RESTARTING)) {
			m_next_state = make_async<title_state>();
		}
		else {
			m_next_state = make_game_state_async<active_game>(game_type::REGULAR, fade_in::YES, m_game->gamemode());
		}
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_NAME, NAME_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_NAME},
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<line_input_widget<20>>(T_NAME_INPUT, NAME_INPUT_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, tr::sys::ttf_style::NORMAL, 64, scb,
										name_acb);
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_DESCRIPTION},
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<multiline_input_widget<255>>(T_DESCRIPTION_INPUT, DESCRIPTION_INPUT_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, 800, 10, 24,
											  scb);
	m_ui.emplace<text_button_widget>(T_SAVE, SAVE_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SAVE},
									 font::LANGUAGE, 48, save_scb, save_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_DISCARD, DISCARD_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_DISCARD},
									 font::LANGUAGE, 48, scb, discard_acb, sound::CONFIRM);
}

//

tr::next_state save_replay_state::tick()
{
	game_menu_state::tick();
	if (m_elapsed >= 0.5_s && to_base(m_substate) == substate_base::EXITING) {
		g_renderer->set_default_transform(TRANSFORM);
		return m_next_state.get();
	}
	else {
		return tr::KEEP_STATE;
	}
}

//

save_replay_state::substate operator|(const save_replay_state::substate_base& l, const save_screen_flags& r)
{
	return save_replay_state::substate(int(l) | int(r));
}

save_replay_state::substate_base to_base(save_replay_state::substate state)
{
	return save_replay_state::substate_base(int(state) & 0x1);
}

save_screen_flags to_flags(save_replay_state::substate state)
{
	return save_screen_flags(int(state) & int(save_screen_flags::MASK));
}

float save_replay_state::fade_overlay_opacity()
{
	if (to_base(m_substate) == substate_base::EXITING) {
		return m_elapsed / 0.5_sf;
	}
	else {
		return 0;
	}
}

void save_replay_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.move(TOP_START_POS, 0.5_s);
	m_ui[T_NAME].pos.move_x(600, 0.5_s);
	m_ui[T_NAME_INPUT].pos.move_x(600, 0.5_s);
	m_ui[T_DESCRIPTION].pos.move_x(400, 0.5_s);
	m_ui[T_DESCRIPTION_INPUT].pos.move_x(400, 0.5_s);
	m_ui[T_SAVE].pos.move(BOTTOM_START_POS, 0.5_s);
	m_ui[T_DISCARD].pos.move(BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}