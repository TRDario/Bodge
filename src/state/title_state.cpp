#include "../../include/audio.hpp"
#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr tag T_LOGO_TEXT{"logo_text"};
constexpr tag T_LOGO_OVERLAY{"logo_overlay"};
constexpr tag T_LOGO_BALL{"logo_ball"};
constexpr tag T_COPYRIGHT{"© 2025 TRDario"};
constexpr tag T_VERSION{VERSION_STRING};
constexpr tag T_START_GAME{"start_game"};
constexpr tag T_GAMEMODE_DESIGNER{"gamemode_designer"};
constexpr tag T_SCOREBOARDS{"scoreboards"};
constexpr tag T_REPLAYS{"replays"};
constexpr tag T_SETTINGS{"settings"};
constexpr tag T_CREDITS{"credits"};
constexpr tag T_EXIT{"exit"};

constexpr std::array<tag, 7> BUTTONS{T_START_GAME, T_GAMEMODE_DESIGNER, T_SCOREBOARDS, T_REPLAYS, T_SETTINGS, T_CREDITS, T_EXIT};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_START_GAME},
	selection_tree_row{T_GAMEMODE_DESIGNER},
	selection_tree_row{T_SCOREBOARDS},
	selection_tree_row{T_REPLAYS},
	selection_tree_row{T_SETTINGS},
	selection_tree_row{T_CREDITS},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{"Enter"_kc, T_START_GAME},	   {"1"_kc, T_START_GAME},
	{"G"_kc, T_GAMEMODE_DESIGNER}, {"2"_kc, T_GAMEMODE_DESIGNER},
	{"B"_kc, T_SCOREBOARDS},	   {"3"_kc, T_SCOREBOARDS},
	{"R"_kc, T_REPLAYS},		   {"4"_kc, T_REPLAYS},
	{"S"_kc, T_SETTINGS},		   {"5"_kc, T_SETTINGS},
	{"Enter"_kc, T_EXIT},		   {"6"_kc, T_EXIT},
};

constexpr tweener<glm::vec2> LOGO_TEXT_MOVE_IN{tween::CUBIC, {500, 100}, {500, 160}, 2.5_s};
constexpr tweener<glm::vec2> LOGO_BALL_MOVE_IN{tween::CUBIC, {-180, 644}, {327, 217}, 2.5_s};

// clang-format on

title_state::title_state()
	: main_menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::ENTERING_GAME}
{
}

title_state::title_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_TITLE}
{
}

//

void title_state::set_up_ui()
{
	using enum tr::align;

	m_ui.emplace<image_widget>(T_LOGO_TEXT, LOGO_TEXT_MOVE_IN, CENTER, 2.5_s, 0, "logo_text");
	m_ui.emplace<image_widget>(T_LOGO_OVERLAY, LOGO_TEXT_MOVE_IN, CENTER, 2.5_s, 1, "logo_overlay", g_settings.primary_hue);
	m_ui.emplace<image_widget>(T_LOGO_BALL, LOGO_BALL_MOVE_IN, CENTER, 2.5_s, 2, "logo_ball", g_settings.secondary_hue);

	widget& copyright{m_ui.emplace<label_widget>(T_COPYRIGHT, glm::vec2{4, 1000}, TOP_LEFT, 1_s, NO_TOOLTIP, string_text{T_COPYRIGHT},
												 text_style::NORMAL, 24)};
	copyright.pos.change(tween::CUBIC, {4, 998 - copyright.size().y}, 1_s);
	widget& version{m_ui.emplace<label_widget>(T_VERSION, glm::vec2{996, 1000}, TOP_RIGHT, 1_s, NO_TOOLTIP, string_text{T_VERSION},
											   text_style::NORMAL, 24)};
	version.pos.change(tween::CUBIC, {996, 998 - version.size().y}, 1_s);

	glm::vec2 end_pos{990, 965 - (BUTTONS.size() - 1) * 50};
	for (usize i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * g_rng.generate(35.0f, 75.0f)};
		const tweener<glm::vec2> move_in{tween::CUBIC, {end_pos.x + offset, end_pos.y}, end_pos, 1_s};
		const sound sound{i != BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL};
		m_ui.emplace<text_button_widget>(BUTTONS[i], move_in, CENTER_RIGHT, 1_s, NO_TOOLTIP, tag_loc{BUTTONS[i]}, font::LANGUAGE, 48,
										 interactible, ACTION_CALLBACKS[i], sound);
		end_pos += glm::vec2{-25, 50};
	}
}

//

next_state title_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::ENTERING_GAME:
		if (m_timer >= 1.0_s) {
			m_timer = 0;
			m_substate = substate::IN_TITLE;
		}
		return tr::KEEP_STATE;
	case substate::IN_TITLE:
		return tr::KEEP_STATE;
	case substate::ENTERING_SUBMENU:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
	case substate::EXITING_GAME:
		return m_timer >= 0.5_s ? next_state{tr::DROP_STATE} : tr::KEEP_STATE;
	}
}

//

float title_state::fade_overlay_opacity()
{
	switch (m_substate) {
	case substate::ENTERING_GAME:
		return 1 - m_timer / 1_sf;
	case substate::IN_TITLE:
	case substate::ENTERING_SUBMENU:
		return 0;
	case substate::EXITING_GAME:
		return m_timer / 0.5_sf;
	}
}

//

void title_state::set_up_exit_animation()
{
	int i = 0;
	for (tag tag : BUTTONS) {
		const float offset{(i++ % 2 != 0 ? -1.0f : 1.0f) * g_rng.generate(35.0f, 75.0f)};
		widget& widget{m_ui[tag]};
		widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
	}
	m_ui[T_LOGO_TEXT].pos.change(tween::CUBIC, {500, 220}, 0.5_s);
	m_ui[T_LOGO_OVERLAY].pos.change(tween::CUBIC, {500, 220}, 0.5_s);
	m_ui[T_LOGO_BALL].pos.change(tween::CUBIC, {487, 57}, 0.5_s);
	m_ui[T_COPYRIGHT].pos.change(tween::CUBIC, {4, 1000}, 0.5_s);
	m_ui[T_VERSION].pos.change(tween::CUBIC, {996, 1000}, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}

//

bool title_state::interactible()
{
	const title_state& self{g_state_machine.get<title_state>()};

	return self.m_substate == substate::IN_TITLE || self.m_substate == substate::ENTERING_GAME;
}

void title_state::on_start_game()
{
	title_state& self{g_state_machine.get<title_state>()};

	self.m_substate = substate::ENTERING_SUBMENU;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<start_game_state>(self.m_game);
}

void title_state::on_gamemode_designer()
{
	title_state& self{g_state_machine.get<title_state>()};

	self.m_substate = substate::ENTERING_SUBMENU;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<gamemode_designer_state>(self.m_game, gamemode{.author = g_scorefile.name, .song = "classic"}, false);
}

void title_state::on_scoreboards()
{
	title_state& self{g_state_machine.get<title_state>()};

	self.m_substate = substate::ENTERING_SUBMENU;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<scoreboard_selection_state>(self.m_game, false);
}

void title_state::on_replays()
{
	title_state& self{g_state_machine.get<title_state>()};

	self.m_substate = substate::ENTERING_SUBMENU;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<replays_state>(self.m_game);
}

void title_state::on_settings()
{
	title_state& self{g_state_machine.get<title_state>()};

	self.m_substate = substate::ENTERING_SUBMENU;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<settings_state>(self.m_game);
}

void title_state::on_credits()
{
	title_state& self{g_state_machine.get<title_state>()};

	self.m_substate = substate::ENTERING_SUBMENU;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<credits_state>(self.m_game);
}

void title_state::on_exit()
{
	title_state& self{g_state_machine.get<title_state>()};

	self.m_substate = substate::EXITING_GAME;
	self.m_timer = 0;
	self.set_up_exit_animation();
	g_audio.fade_song_out(0.5s);
}