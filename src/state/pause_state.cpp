#include "../../include/state/state.hpp"
#include "../../include/system.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr tag T_PAUSED{"paused"};
constexpr tag T_TEST_PAUSED{"test_paused"};
constexpr tag T_REPLAY_PAUSED{"replay_paused"};
constexpr tag T_UNPAUSE{"unpause"};
constexpr tag T_SAVE_AND_RESTART{"save_and_restart"};
constexpr tag T_RESTART{"restart"};
constexpr tag T_SAVE_AND_QUIT{"save_and_quit"};
constexpr tag T_QUIT{"quit"};

constexpr std::array<tag, 5> BUTTONS_REGULAR{T_UNPAUSE, T_SAVE_AND_RESTART, T_RESTART, T_SAVE_AND_QUIT, T_QUIT};
constexpr std::array<tag, 3> BUTTONS_SPECIAL{T_UNPAUSE, T_RESTART, T_QUIT};

constexpr selection_tree SELECTION_TREE_REGULAR{
	selection_tree_row{T_UNPAUSE},
	selection_tree_row{T_SAVE_AND_RESTART},
	selection_tree_row{T_RESTART},
	selection_tree_row{T_SAVE_AND_QUIT},
	selection_tree_row{T_QUIT},
};
constexpr selection_tree SELECTION_TREE_SPECIAL{
	selection_tree_row{T_UNPAUSE},
	selection_tree_row{T_RESTART},
	selection_tree_row{T_QUIT},
};

constexpr shortcut_table SHORTCUTS_REGULAR{
	{"Enter"_kc, T_UNPAUSE},			{"1"_kc, T_UNPAUSE},
	{"Shift+R"_kc, T_SAVE_AND_RESTART}, {"2"_kc, T_SAVE_AND_RESTART},
	{"R"_kc, T_RESTART},				{"3"_kc, T_RESTART},
	{"Shift+Q"_kc, T_SAVE_AND_QUIT},	{"4"_kc, T_SAVE_AND_QUIT},
	{"Q"_kc, T_QUIT},					{"5"_kc, T_QUIT},
};
constexpr shortcut_table SHORTCUTS_SPECIAL{
	{"Enter"_kc, T_UNPAUSE}, {"1"_kc, T_UNPAUSE},
	{"R"_kc, T_RESTART},	 {"2"_kc, T_RESTART},
	{"Q"_kc, T_QUIT},		 {"3"_kc, T_QUIT},
};

// clang-format on

pause_state::pause_state(std::shared_ptr<game> game, game_type type, glm::vec2 mouse_pos, bool blur_in)
	: game_menu_state{type == game_type::REGULAR ? SELECTION_TREE_REGULAR : SELECTION_TREE_SPECIAL,
					  type == game_type::REGULAR ? SHORTCUTS_REGULAR : SHORTCUTS_SPECIAL, std::move(game), false}
	, m_substate{(blur_in ? substate_base::PAUSING : substate_base::PAUSED) | type}
	, m_start_mouse_pos{mouse_pos}
{
	if (blur_in) {
		m_game->add_to_renderer();
		g_graphics->basic_renderer.draw(g_graphics->blur_renderer.input());
	}
}

//

void pause_state::set_up_ui()
{
	if (to_type(m_substate) == game_type::REGULAR) {
		set_up_full_ui();
	}
	else {
		set_up_limited_ui();
	}
}

next_state pause_state::tick()
{
	game_menu_state::tick();
	switch (to_base(m_substate)) {
	case substate_base::PAUSING:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate_base::PAUSED | to_type(m_substate);
		}
		return tr::KEEP_STATE;
	case substate_base::PAUSED:
		return tr::KEEP_STATE;
	case substate_base::UNPAUSING:
		if (to_type(m_substate) != game_type::REPLAY) {
			float ratio{m_timer / 0.5_sf};
			ratio = ratio < 0.5 ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
			g_mouse_pos = m_end_mouse_pos + (m_start_mouse_pos - m_end_mouse_pos) * ratio;
		}

		if (m_timer >= 0.5_s) {
			g_audio.unpause_song();
			return g_next_state.get();
		}
		else {
			return tr::KEEP_STATE;
		}
	case substate_base::RESTARTING:
		if (m_timer < 0.5_s) {
			return tr::KEEP_STATE;
		}
		g_graphics->basic_renderer.set_default_transform(TRANSFORM);
		return g_next_state.get();
	case substate_base::SAVING:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
	case substate_base::QUITTING:
		if (m_timer < 0.5_s) {
			return tr::KEEP_STATE;
		}
		g_graphics->basic_renderer.set_default_transform(TRANSFORM);
		switch (to_type(m_substate)) {
		case game_type::REGULAR:
			g_audio.play_song("menu", 1.0s);
			break;
		case game_type::GAMEMODE_DESIGNER_TEST:
		case game_type::REPLAY:
			g_audio.play_song("menu", SKIP_MENU_SONG_INTRO_TIMESTAMP, 0.5s);
			break;
		}
		return g_next_state.get();
	}
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

pause_state::substate operator|(const pause_state::substate_base& l, const game_type& r)
{
	return pause_state::substate(int(l) | int(r));
}

pause_state::substate_base to_base(pause_state::substate state)
{
	return pause_state::substate_base(int(state) & 0x7);
}

game_type to_type(pause_state::substate state)
{
	return game_type(int(state) & 0x18);
}

float pause_state::fade_overlay_opacity()
{
	return to_base(m_substate) == substate_base::RESTARTING || to_base(m_substate) == substate_base::QUITTING ? m_timer / 0.5_sf : 0;
}

float pause_state::saturation_factor()
{
	switch (to_base(m_substate)) {
	case substate_base::PAUSED:
	case substate_base::SAVING:
	case substate_base::RESTARTING:
	case substate_base::QUITTING:
		return 0.35f;
	case substate_base::PAUSING:
		return 1 - m_timer / 0.5_sf * 0.65f;
	case substate_base::UNPAUSING:
		return 0.35f + m_timer / 0.5_sf * 0.65f;
	}
}

float pause_state::blur_strength()
{
	switch (to_base(m_substate)) {
	case substate_base::PAUSED:
	case substate_base::SAVING:
	case substate_base::RESTARTING:
	case substate_base::QUITTING:
		return 10;
	case substate_base::PAUSING:
		return m_timer / 0.5_sf * 10;
	case substate_base::UNPAUSING:
		return (1 - m_timer / 0.5_sf) * 10;
	}
}

void pause_state::set_up_full_ui()
{
	using enum tr::align;

	constexpr float TITLE_Y{500.0f - (BUTTONS_REGULAR.size() + 1) * 30};
	constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, {500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};

	m_ui.emplace<label_widget>(T_PAUSED, TITLE_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_PAUSED}, text_style::NORMAL, 64);
	for (usize i = 0; i < BUTTONS_REGULAR.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS_REGULAR.size() + 1) * 30 + (i + 2) * 60};
		const tweener<glm::vec2> move_in{tween::CUBIC, {500 + offset, y}, {500, y}, 0.5_s};

		m_ui.emplace<text_button_widget>(BUTTONS_REGULAR[i], move_in, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{BUTTONS_REGULAR[i]},
										 font::LANGUAGE, 48, i == 0 ? unpause_interactible : interactible, FULL_ACTION_CALLBACKS[i],
										 sound::CONFIRM);
	}
}

void pause_state::set_up_limited_ui()
{
	using enum tr::align;

	constexpr float TITLE_Y{500.0f - (BUTTONS_SPECIAL.size() + 1) * 30};
	constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, {500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};
	const tag title_tag{to_type(m_substate) == game_type::REPLAY ? T_REPLAY_PAUSED : T_TEST_PAUSED};

	m_ui.emplace<label_widget>(title_tag, TITLE_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{title_tag}, text_style::NORMAL, 64);
	for (usize i = 0; i < BUTTONS_SPECIAL.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS_SPECIAL.size() + 1) * 30 + (i + 2) * 60};
		const tweener<glm::vec2> move_in{tween::CUBIC, {500 + offset, y}, {500, y}, 0.5_s};

		m_ui.emplace<text_button_widget>(BUTTONS_SPECIAL[i], move_in, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{BUTTONS_SPECIAL[i]},
										 font::LANGUAGE, 48, i == 0 ? unpause_interactible : interactible, LIMITED_ACTION_CALLBACKS[i],
										 sound::CONFIRM);
	}
}

void pause_state::set_up_exit_animation()
{
	if (to_type(m_substate) == game_type::REGULAR) {
		m_ui[T_PAUSED].pos.change(tween::CUBIC, {500, 400 - (BUTTONS_REGULAR.size() + 1) * 30}, 0.5_s);
		for (usize i = 0; i < BUTTONS_REGULAR.size(); ++i) {
			const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
			widget& widget{m_ui[BUTTONS_REGULAR[i]]};
			widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
		}
	}
	else {
		widget& title{m_ui[to_type(m_substate) == game_type::GAMEMODE_DESIGNER_TEST ? T_TEST_PAUSED : T_REPLAY_PAUSED]};
		title.pos.change(tween::CUBIC, {500, 400 - (BUTTONS_SPECIAL.size() + 1) * 30}, 0.5_s);
		for (usize i = 0; i < BUTTONS_SPECIAL.size(); ++i) {
			const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * g_rng.generate(50.0f, 150.0f)};
			widget& widget{m_ui[BUTTONS_SPECIAL[i]]};
			widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
		}
	}

	m_ui.hide_all_widgets(0.5_s);
}

//

bool pause_state::interactible()
{
	const pause_state& self{g_state_machine.get<pause_state>()};

	return to_base(self.m_substate) == substate_base::PAUSED || to_base(self.m_substate) == substate_base::PAUSING;
}

bool pause_state::unpause_interactible()
{
	const pause_state& self{g_state_machine.get<pause_state>()};

	return to_base(self.m_substate) == substate_base::PAUSED;
}

void pause_state::on_unpause()
{
	pause_state& self{g_state_machine.get<pause_state>()};

	self.m_timer = 0;
	self.m_substate = substate_base::UNPAUSING | to_type(self.m_substate);
	self.m_end_mouse_pos = g_mouse_pos;
	self.set_up_exit_animation();
	g_audio.play_sound(sound::UNPAUSE, 0.8f, 0.0f);
	prepare_next_state<game_state>(self.m_game, game_type::REGULAR, false);
}

void pause_state::on_save_and_restart()
{
	pause_state& self{g_state_machine.get<pause_state>()};

	self.m_timer = 0;
	self.m_substate = substate_base::SAVING | game_type::REGULAR;
	self.set_up_exit_animation();
	prepare_next_state<save_score_state>(self.m_game, self.m_start_mouse_pos, save_screen_flags::RESTARTING);
}

void pause_state::on_restart()
{
	pause_state& self{g_state_machine.get<pause_state>()};

	self.m_timer = 0;
	self.m_substate = substate_base::RESTARTING | to_type(self.m_substate);
	self.set_up_exit_animation();
	if (to_type(self.m_substate) == game_type::REPLAY) {
		prepare_next_game_state<replay_game>(game_type::REPLAY, true, (replay_game&)*self.m_game);
	}
	else {
		if (to_type(self.m_substate) == game_type::REGULAR) {
			const score_flags score_flags{true, g_cli_settings.game_speed != 1};
			const score_entry score{{}, current_timestamp(), self.m_game->final_score(), self.m_game->final_time(), score_flags};

			g_scorefile.add_score(self.m_game->gamemode(), score);
		}
		prepare_next_game_state<active_game>(to_type(self.m_substate), true, self.m_game->gamemode());
	}
}

void pause_state::on_save_and_quit()
{
	pause_state& self{g_state_machine.get<pause_state>()};

	self.m_timer = 0;
	self.m_substate = substate_base::SAVING | game_type::REGULAR;
	self.set_up_exit_animation();
	prepare_next_state<save_score_state>(self.m_game, self.m_start_mouse_pos, save_screen_flags::NONE);
}

void pause_state::on_quit()
{
	pause_state& self{g_state_machine.get<pause_state>()};

	self.m_timer = 0;
	self.m_substate = substate_base::QUITTING | to_type(self.m_substate);
	self.set_up_exit_animation();
	switch (to_type(self.m_substate)) {
	case game_type::REGULAR: {
		const score_flags score_flags{true, g_cli_settings.game_speed != 1};
		const score_entry score{{}, current_timestamp(), self.m_game->final_score(), self.m_game->final_time(), score_flags};

		g_scorefile.add_score(self.m_game->gamemode(), score);
		prepare_next_state<title_state>();
	} break;
	case game_type::GAMEMODE_DESIGNER_TEST:
		prepare_next_state<gamemode_designer_state>(self.m_game->gamemode());
		break;
	case game_type::REPLAY:
		prepare_next_state<replays_state>();
		break;
	}
}