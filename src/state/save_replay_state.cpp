#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

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
	{"Enter"_kc, T_DISCARD},
	{"C"_kc, T_DISCARD},
	{"2"_kc, T_DISCARD},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> NAME_MOVE_IN{tween::CUBIC, {400, 200}, {500, 200}, 0.5_s};
constexpr tweener<glm::vec2> NAME_INPUT_MOVE_IN{tween::CUBIC, {400, 235}, {500, 235}, 0.5_s};
constexpr tweener<glm::vec2> DESCRIPTION_MOVE_IN{tween::CUBIC, {600, 440}, {500, 440}, 0.5_s};
constexpr tweener<glm::vec2> DESCRIPTION_INPUT_MOVE_IN{tween::CUBIC, {600, 475}, {500, 475}, 0.5_s};
constexpr tweener<glm::vec2> SAVE_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweener<glm::vec2> DISCARD_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

save_replay_state::save_replay_state(std::shared_ptr<game> game, save_screen_flags flags)
	: game_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game), bool(flags & save_screen_flags::GAME_OVER)}
	, m_substate{substate_base::SAVING_REPLAY | flags}
	, m_replay{((active_game&)*m_game).replay.header()}
{
	using enum tr::align;

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_NAME, NAME_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_NAME}, text_style::NORMAL, 48);
	m_ui.emplace<line_input_widget<20>>(T_NAME_INPUT, NAME_INPUT_MOVE_IN, TOP_CENTER, 0.5_s, text_style::NORMAL, 64, interactible, on_name,
										std::string_view{});
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_DESCRIPTION}, text_style::NORMAL,
							   48);
	m_ui.emplace<multiline_input_widget<255>>(T_DESCRIPTION_INPUT, DESCRIPTION_INPUT_MOVE_IN, TOP_CENTER, 0.5_s, 800, 10, 24, interactible);
	m_ui.emplace<text_button_widget>(T_SAVE, SAVE_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_SAVE}, font::LANGUAGE, 48,
									 save_interactible, on_save, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_DISCARD, DISCARD_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_DISCARD}, font::LANGUAGE, 48,
									 interactible, on_discard, sound::CONFIRM);
}

//

next_state save_replay_state::tick()
{
	game_menu_state::tick();
	if (m_timer >= 0.5_s && to_base(m_substate) == substate_base::EXITING) {
		g_graphics->basic_renderer.set_default_transform(TRANSFORM);
		return g_next_state.get();
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
		return m_timer / 0.5_sf;
	}
	else {
		return 0;
	}
}

void save_replay_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_NAME].pos.change(tween::CUBIC, {600, 200}, 0.5_s);
	m_ui[T_NAME_INPUT].pos.change(tween::CUBIC, {600, 235}, 0.5_s);
	m_ui[T_DESCRIPTION].pos.change(tween::CUBIC, {400, 440}, 0.5_s);
	m_ui[T_DESCRIPTION_INPUT].pos.change(tween::CUBIC, {400, 475}, 0.5_s);
	m_ui[T_SAVE].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui[T_DISCARD].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}

//

bool save_replay_state::interactible()
{
	const save_replay_state& self{g_state_machine.get<save_replay_state>()};

	return to_base(self.m_substate) == substate_base::SAVING_REPLAY;
}

bool save_replay_state::save_interactible()
{
	const save_replay_state& self{g_state_machine.get<save_replay_state>()};

	return to_base(self.m_substate) == substate_base::SAVING_REPLAY && !self.m_ui.as<line_input_widget<20>>(T_NAME_INPUT).buffer.empty();
}

void save_replay_state::on_name()
{
	save_replay_state& self{g_state_machine.get<save_replay_state>()};

	self.m_ui.select_next_widget();
}

void save_replay_state::on_save()
{
	save_replay_state& self{g_state_machine.get<save_replay_state>()};

	const score_flags flags{!self.m_game->game_over(), g_cli_settings.game_speed != 1.0f};
	const auto& description{self.m_ui.as<multiline_input_widget<255>>(T_DESCRIPTION_INPUT).buffer};
	const auto& name{self.m_ui.as<line_input_widget<20>>("name_input").buffer};
	active_game& game{(active_game&)*self.m_game};

	self.m_substate = substate_base::EXITING | to_flags(self.m_substate);
	self.m_timer = 0;
	self.set_up_exit_animation();
	game.replay.set_header(score_entry{description, current_timestamp(), game.final_score(), game.final_time(), flags}, name);
	game.replay.save_to_file();
	if (!(to_flags(self.m_substate) & save_screen_flags::RESTARTING)) {
		prepare_next_state<title_state>();
	}
	else {
		prepare_next_game_state<active_game>(game_type::REGULAR, true, self.m_game->gamemode());
	}
}

void save_replay_state::on_discard()
{
	save_replay_state& self{g_state_machine.get<save_replay_state>()};

	self.m_substate = substate_base::EXITING | to_flags(self.m_substate);
	self.m_timer = 0;
	self.set_up_exit_animation();
	if (!(to_flags(self.m_substate) & save_screen_flags::RESTARTING)) {
		prepare_next_state<title_state>();
	}
	else {
		prepare_next_game_state<active_game>(game_type::REGULAR, true, self.m_game->gamemode());
	}
}