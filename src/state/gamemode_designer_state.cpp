#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"gamemode_designer"};
constexpr tag T_NAME{"name"};
constexpr tag T_AUTHOR{"author"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_BALL_SETTINGS{"ball_settings"};
constexpr tag T_PLAYER_SETTINGS{"player_settings"};
constexpr tag T_SONG{"song"};
constexpr tag T_SONG_C{"song_c"};
constexpr tag T_TEST{"test"};
constexpr tag T_SAVE{"save"};
constexpr tag T_DISCARD{"discard"};

// Buttons on the bottom of the screen.
constexpr std::array<tag, 3> BOTTOM_BUTTONS{T_TEST, T_SAVE, T_DISCARD};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_NAME},
	selection_tree_row{T_DESCRIPTION},
	selection_tree_row{T_BALL_SETTINGS},
	selection_tree_row{T_PLAYER_SETTINGS},
	selection_tree_row{T_SONG_C},
	selection_tree_row{T_TEST},
	selection_tree_row{T_SAVE},
	selection_tree_row{T_DISCARD},
};

constexpr shortcut_table SHORTCUTS{
	{"B"_kc, T_BALL_SETTINGS},
	{"1"_kc, T_BALL_SETTINGS},
	{"P"_kc, T_PLAYER_SETTINGS},
	{"2"_kc, T_PLAYER_SETTINGS},
	{"T"_kc, T_TEST},
	{"3"_kc, T_TEST},
	{"S"_kc, T_SAVE},
	{"Enter"_kc, T_SAVE},
	{"4"_kc, T_SAVE},
	{"C"_kc, T_DISCARD},
	{"Q"_kc, T_DISCARD},
	{"E"_kc, T_DISCARD},
	{"Escape"_kc, T_DISCARD},
	{"5"_kc, T_DISCARD},
};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweened_position NAME_MOVE_IN{glm::vec2{400, 240}, {500, 240}, 0.5_s};
constexpr tweened_position AUTHOR_MOVE_IN{glm::vec2{600, 315}, glm::vec2{500, 315}, 0.5_s};
constexpr tweened_position DESCRIPTION_MOVE_IN{glm::vec2{400, 365}, glm::vec2{500, 365}, 0.5_s};
constexpr tweened_position BALL_SETTINGS_MOVE_IN{glm::vec2{600, 450}, glm::vec2{500, 450}, 0.5_s};
constexpr tweened_position PLAYER_SETTINGS_MOVE_IN{glm::vec2{400, 550}, glm::vec2{500, 550}, 0.5_s};
constexpr tweened_position SONG_MOVE_IN{glm::vec2{600, 650}, glm::vec2{500, 650}, 0.5_s};
constexpr tweened_position SONG_C_MOVE_IN{glm::vec2{400, 700}, glm::vec2{500, 700}, 0.5_s};

// clang-format on
///////////////////////////////////////////////////////// GAMEMODE DESIGNER STATE /////////////////////////////////////////////////////////

gamemode_designer_state::gamemode_designer_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode,
												 returning_from_subscreen returning_from_subscreen)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_GAMEMODE_DESIGNER}
	, m_pending{gamemode}
	, m_available_songs{create_available_song_list()}
{
	set_up_ui(returning_from_subscreen);
}

gamemode_designer_state::gamemode_designer_state(const gamemode& gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS}
	, m_substate{substate::RETURNING_FROM_TEST_GAME}
	, m_pending{gamemode}
	, m_available_songs{create_available_song_list()}
{
	set_up_ui(returning_from_subscreen::NO);
}

//

tr::next_state gamemode_designer_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		if (m_elapsed >= 0.5_s) {
			m_substate = substate::IN_GAMEMODE_DESIGNER;
			m_elapsed = 0;
		}
		return tr::KEEP_STATE;
	case substate::IN_GAMEMODE_DESIGNER:
		return tr::KEEP_STATE;
	case substate::ENTERING_TEST_GAME:
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

//

float gamemode_designer_state::fade_overlay_opacity()
{
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		return 1 - m_elapsed / 0.5_sf;
	case substate::IN_GAMEMODE_DESIGNER:
	case substate::EXITING:
		return 0;
	case substate::ENTERING_TEST_GAME:
		return m_elapsed / 0.5_sf;
	}
}

void gamemode_designer_state::set_up_ui(returning_from_subscreen returning_from_subscreen)
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_GAMEMODE_DESIGNER; },
	};
	const std::array<status_callback, BOTTOM_BUTTONS.size()> bottom_scbs{
		scb,
		[this] { return m_substate == substate::IN_GAMEMODE_DESIGNER && !m_ui.as<line_input_widget<12>>(T_NAME).buffer.empty(); },
		scb,
	};

	// ACTION CALLBACKS

	const action_callback name_enter_cb{
		[this] { m_ui.select_next_widget(); },
	};
	const action_callback description_enter_cb{
		[this] { m_ui.clear_selection(); },
	};
	const action_callback ball_settings_acb{
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			set_up_subscreen_animation();
			m_next_state = make_async<ball_settings_editor_state>(m_game, m_pending);
		},
	};
	const action_callback player_settings_acb{
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			set_up_subscreen_animation();
			m_next_state = make_async<player_settings_editor_state>(m_game, m_pending);
		},
	};
	const action_callback song_c_acb{
		[this] {
			std::vector<std::string>::iterator it{std::ranges::find(m_available_songs, m_pending.song)};
			if (it == m_available_songs.end() || ++it == m_available_songs.end()) {
				it = m_available_songs.begin();
			}
			m_pending.song = *it;
		},
	};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_acbs{
		[this] {
			m_substate = substate::ENTERING_TEST_GAME;
			m_elapsed = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			set_up_exit_animation();
			g_audio.fade_song_out(0.5s);
			m_next_state = make_game_state_async<active_game>(game_type::GAMEMODE_DESIGNER_TEST, fade_in::YES, m_pending);
		},
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			m_pending.save_to_file();
			set_up_exit_animation();
			m_next_state = make_async<title_state>(m_game);
		},
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			set_up_exit_animation();
			m_next_state = make_async<title_state>(m_game);
		},
	};

	// TEXT CALLBACKS

	text_callback song_c_tcb{[&] { return std::string{m_pending.song}; }};

	//

	if (returning_from_subscreen == returning_from_subscreen::YES) {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
	}
	m_ui.emplace<line_input_widget<12>>(T_NAME, NAME_MOVE_IN, tr::align::CENTER, 0.5_s, tr::sys::ttf_style::NORMAL, 120, scb, name_enter_cb,
										m_pending.name);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{TR_FMT::format("{}: {}", g_loc["by"], g_scorefile.name)}, tr::sys::ttf_style::NORMAL,
							   32);
	m_ui.emplace<line_input_widget<40>>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, tr::sys::ttf_style::ITALIC, 32, scb,
										description_enter_cb, m_pending.description);
	m_ui.emplace<text_button_widget>(T_BALL_SETTINGS, BALL_SETTINGS_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 loc_text_callback{T_BALL_SETTINGS}, font::LANGUAGE, 64, scb, ball_settings_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_PLAYER_SETTINGS, PLAYER_SETTINGS_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 loc_text_callback{T_PLAYER_SETTINGS}, font::LANGUAGE, 64, scb, player_settings_acb, sound::CONFIRM);
	m_ui.emplace<label_widget>(T_SONG, SONG_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SONG},
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<text_button_widget>(T_SONG_C, SONG_C_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, song_c_tcb, font::LANGUAGE, 64, scb,
									 song_c_acb, sound::CONFIRM);
	for (usize i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sound sound{i != BOTTOM_BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL};
		const tweened_position move_in{BOTTOM_START_POS, {500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s};
		m_ui.emplace<text_button_widget>(BOTTOM_BUTTONS[i], move_in, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP,
										 loc_text_callback{BOTTOM_BUTTONS[i]}, font::LANGUAGE, 48, bottom_scbs[i], bottom_acbs[i], sound);
	}
}

void gamemode_designer_state::set_up_subscreen_animation()
{
	m_ui[T_NAME].pos.move_x(600, 0.5_s);
	m_ui[T_AUTHOR].pos.move_x(400, 0.5_s);
	m_ui[T_DESCRIPTION].pos.move_x(600, 0.5_s);
	m_ui[T_BALL_SETTINGS].pos.move_x(400, 0.5_s);
	m_ui[T_PLAYER_SETTINGS].pos.move_x(600, 0.5_s);
	m_ui[T_SONG].pos.move_x(400, 0.5_s);
	m_ui[T_SONG_C].pos.move_x(600, 0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].pos.move(BOTTOM_START_POS, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
	m_ui[T_TITLE].unhide();
}

void gamemode_designer_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.move(TOP_START_POS, 0.5_s);
	m_ui[T_NAME].pos.move_x(600, 0.5_s);
	m_ui[T_AUTHOR].pos.move_x(400, 0.5_s);
	m_ui[T_DESCRIPTION].pos.move_x(600, 0.5_s);
	m_ui[T_BALL_SETTINGS].pos.move_x(400, 0.5_s);
	m_ui[T_PLAYER_SETTINGS].pos.move_x(600, 0.5_s);
	m_ui[T_SONG].pos.move_x(400, 0.5_s);
	m_ui[T_SONG_C].pos.move_x(600, 0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].pos.move(BOTTOM_START_POS, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
}