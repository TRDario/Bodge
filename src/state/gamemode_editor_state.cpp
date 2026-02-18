///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements gamemode_editor_state from state.hpp.                                                                                      //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"gamemode_manager"};
constexpr tag T_SUBTITLE{"subtitle"};
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
constexpr std::array BOTTOM_BUTTONS{T_TEST, T_SAVE, T_DISCARD};

// Selection tree for the gamemode editor menu.
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

// Shortcut table for the gamemode editor menu.
constexpr shortcut_table SHORTCUTS{
	{"B"_kc, T_BALL_SETTINGS}, {"1"_kc, T_BALL_SETTINGS},
	{"P"_kc, T_PLAYER_SETTINGS}, {"2"_kc, T_PLAYER_SETTINGS},
	{"T"_kc, T_TEST}, {"3"_kc, T_TEST},
	{"Ctrl+S"_kc, T_SAVE}, {"Enter"_kc, T_SAVE}, {"4"_kc, T_SAVE},
	{"Escape"_kc, T_DISCARD}, {"Q"_kc, T_DISCARD}, {"5"_kc, T_DISCARD},
};

// Entry animation for the title widget.
constexpr tweened_position TITLE_ANIMATION{TOP_START_POS, TITLE_POS, 0.5_s};
// Entry animation for the subtitle widget.
constexpr tweened_position SUBTITLE_ANIMATION{TOP_START_POS, {500, 64}, 0.5_s};
// Entry animation for the name input widget.
constexpr tweened_position NAME_ANIMATION{glm::vec2{400, 240}, {500, 240}, 0.5_s};
// Entry animation for the author label widget.
constexpr tweened_position AUTHOR_ANIMATION{glm::vec2{600, 315}, glm::vec2{500, 315}, 0.5_s};
// Entry animation for the description input widget.
constexpr tweened_position DESCRIPTION_ANIMATION{glm::vec2{400, 365}, glm::vec2{500, 365}, 0.5_s};
// Entry animation for the ball settings button widget.
constexpr tweened_position BALL_SETTINGS_ANIMATION{glm::vec2{600, 450}, glm::vec2{500, 450}, 0.5_s};
// Entry animation for the player settings button widget.
constexpr tweened_position PLAYER_SETTINGS_ANIMATION{glm::vec2{400, 550}, glm::vec2{500, 550}, 0.5_s};
// Entry animation for the "song" label widget.
constexpr tweened_position SONG_ANIMATION{glm::vec2{600, 650}, glm::vec2{500, 650}, 0.5_s};
// Entry animation for the current song widget.
constexpr tweened_position SONG_C_ANIMATION{glm::vec2{400, 700}, glm::vec2{500, 700}, 0.5_s};

// clang-format on
///////////////////////////////////////////////////////// GAMEMODE DESIGNER STATE /////////////////////////////////////////////////////////

gamemode_editor_state::gamemode_editor_state(gamemode_editor_data data, gamemode gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS}
	, m_substate{substate::RETURNING_FROM_TEST_GAME}
	, m_data{std::move(data)}
	, m_available_songs{create_available_song_list()}
	, m_pending{std::move(gamemode)}
{
	set_up_ui(animate_title::YES, animate_subtitle::YES);
}

gamemode_editor_state::gamemode_editor_state(std::shared_ptr<playerless_game> game, gamemode_editor_data data, gamemode gamemode,
											 animate_subtitle animate_subtitle)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_GAMEMODE_EDITOR}
	, m_data{std::move(data)}
	, m_available_songs{create_available_song_list()}
	, m_pending{std::move(gamemode)}
{
	m_pending.author = g_scorefile.name;

	set_up_ui(animate_title::NO, animate_subtitle);
}

//

tr::next_state gamemode_editor_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		if (m_elapsed == 1) {
			g_audio.play_song("menu", SKIP_MENU_SONG_INTRO_TIMESTAMP, 1.0s);
		}
		if (m_elapsed >= 0.5_s) {
			m_substate = substate::IN_GAMEMODE_EDITOR;
			m_elapsed = 0;
		}
		return tr::KEEP_STATE;
	case substate::IN_GAMEMODE_EDITOR:
		return tr::KEEP_STATE;
	case substate::ENTERING_TEST_GAME:
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

//

float gamemode_editor_state::fade_overlay_opacity()
{
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		return 1 - m_elapsed / 0.5_sf;
	case substate::IN_GAMEMODE_EDITOR:
	case substate::EXITING:
		return 0;
	case substate::ENTERING_TEST_GAME:
		return m_elapsed / 0.5_sf;
	}
}

void gamemode_editor_state::set_up_ui(animate_title animate_title, animate_subtitle animate_subtitle)
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_GAMEMODE_EDITOR; },
	};
	const status_callback save_scb{
		[this] { return m_substate == substate::IN_GAMEMODE_EDITOR && !m_ui.as<line_input_widget<12>>(T_NAME).buffer.empty(); },
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
			set_up_exit_animation(animate_title::NO, animate_subtitle::YES);
			m_next_state = make_async<ball_settings_editor_state>(m_game, m_data, m_pending);
		},
	};
	const action_callback player_settings_acb{
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			set_up_exit_animation(animate_title::NO, animate_subtitle::YES);
			m_next_state = make_async<player_settings_editor_state>(m_game, m_data, m_pending);
		},
	};
	const action_callback song_c_acb{
		[this] {
			std::vector<std::string>::iterator song_it{std::ranges::find(m_available_songs, m_pending.song)};
			if (song_it == m_available_songs.end() || ++song_it == m_available_songs.end()) {
				song_it = m_available_songs.begin();
			}
			m_pending.song = *song_it;
		},
	};
	const std::array<action_callback, BOTTOM_BUTTONS.size()> bottom_acbs{
		[this] {
			m_substate = substate::ENTERING_TEST_GAME;
			m_elapsed = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			set_up_exit_animation(animate_title::YES, animate_subtitle::YES);
			g_audio.fade_song_out(0.5s);
			m_next_state = make_game_state_async<active_game>(test_game_data{m_data}, m_pending);
		},
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			switch (m_data.index()) {
			case tr::type_index<new_gamemode_editor_data, gamemode_editor_data>:
				g_new_gamemode_draft = gamemode{};
				m_pending.save_to_file();
				m_next_state = make_async<title_state>(m_game);
				set_up_exit_animation(animate_title::YES, animate_subtitle::YES);
				break;
			case tr::type_index<cloned_gamemode_editor_data, gamemode_editor_data>:
				m_pending.save_to_file();
				m_next_state = make_async<title_state>(m_game);
				set_up_exit_animation(animate_title::YES, animate_subtitle::YES);
				break;
			case tr::type_index<edited_gamemode_editor_data, gamemode_editor_data>:
				std::filesystem::remove(tr::get<edited_gamemode_editor_data>(m_data).path);
				m_pending.save_to_file();
				m_next_state = make_async<gamemode_selector_state>(m_game, gamemode_selector_type::EDIT, animate_subtitle::NO);
				set_up_exit_animation(animate_title::NO, animate_subtitle::NO);
				break;
			}
		},
		[this] {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			switch (m_data.index()) {
			case tr::type_index<new_gamemode_editor_data, gamemode_editor_data>:
				g_new_gamemode_draft = m_pending;
				m_next_state = make_async<gamemode_manager_state>(m_game, animate_title::NO);
				set_up_exit_animation(animate_title::NO, animate_subtitle::YES);
				break;
			case tr::type_index<cloned_gamemode_editor_data, gamemode_editor_data>:
				m_next_state = make_async<gamemode_selector_state>(m_game, gamemode_selector_type::CLONE, animate_subtitle::NO);
				set_up_exit_animation(animate_title::NO, animate_subtitle::NO);
				break;
			case tr::type_index<edited_gamemode_editor_data, gamemode_editor_data>:
				m_next_state = make_async<gamemode_selector_state>(m_game, gamemode_selector_type::EDIT, animate_subtitle::NO);
				set_up_exit_animation(animate_title::NO, animate_subtitle::NO);
				break;
			}
		},
	};

	// TEXT CALLBACKS

	text_callback subtitle_tcb{loc_text_callback{std::holds_alternative<new_gamemode_editor_data>(m_data)      ? "new_gamemode"
												 : std::holds_alternative<cloned_gamemode_editor_data>(m_data) ? "clone_gamemode"
																											   : "edit_gamemode"}};
	const text_callback song_c_tcb{[this] { return std::string{m_pending.song}; }};

	// TOOLTIP CALLBACKS

	const text_callback save_ttcb{
		[this] { return m_ui.as<line_input_widget<12>>(T_NAME).buffer.empty() ? std::string{g_loc["save_gamemode_tt"]} : std::string{}; },
	};

	//

	if (bool(animate_title)) {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_ANIMATION, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
	}

	if (bool(animate_subtitle)) {
		m_ui.emplace<label_widget>(T_SUBTITLE, SUBTITLE_ANIMATION, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, std::move(subtitle_tcb),
								   tr::sys::ttf_style::NORMAL, 32);
	}
	else {
		m_ui.emplace<label_widget>(T_SUBTITLE, glm::vec2{500, 64}, tr::align::TOP_CENTER, 0, NO_TOOLTIP, std::move(subtitle_tcb),
								   tr::sys::ttf_style::NORMAL, 32);
	}

	m_ui.emplace<line_input_widget<12>>(T_NAME, NAME_ANIMATION, tr::align::CENTER, 0.5_s, tr::sys::ttf_style::NORMAL, 120, scb,
										name_enter_cb, m_pending.name);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_ANIMATION, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{TR_FMT::format("{}: {}", g_loc["by"], g_scorefile.name)}, tr::sys::ttf_style::NORMAL,
							   32);
	m_ui.emplace<line_input_widget<40>>(T_DESCRIPTION, DESCRIPTION_ANIMATION, tr::align::CENTER, 0.5_s, tr::sys::ttf_style::ITALIC, 32, scb,
										description_enter_cb, m_pending.description);
	m_ui.emplace<text_button_widget>(T_BALL_SETTINGS, BALL_SETTINGS_ANIMATION, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 loc_text_callback{T_BALL_SETTINGS}, font::LANGUAGE, 64, scb, ball_settings_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_PLAYER_SETTINGS, PLAYER_SETTINGS_ANIMATION, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
									 loc_text_callback{T_PLAYER_SETTINGS}, font::LANGUAGE, 64, scb, player_settings_acb, sound::CONFIRM);
	m_ui.emplace<label_widget>(T_SONG, SONG_ANIMATION, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SONG},
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<text_button_widget>(T_SONG_C, SONG_C_ANIMATION, tr::align::CENTER, 0.5_s, NO_TOOLTIP, song_c_tcb, font::LANGUAGE, 64, scb,
									 song_c_acb, sound::CONFIRM);
	for (usize i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sound sound{i != BOTTOM_BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL};
		const tweened_position animation{BOTTOM_START_POS, {500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s};
		m_ui.emplace<text_button_widget>(BOTTOM_BUTTONS[i], animation, tr::align::BOTTOM_CENTER, 0.5_s,
										 BOTTOM_BUTTONS[i] == T_SAVE ? save_ttcb : NO_TOOLTIP, loc_text_callback{BOTTOM_BUTTONS[i]},
										 font::LANGUAGE, 48, BOTTOM_BUTTONS[i] == T_SAVE ? save_scb : scb, bottom_acbs[i], sound);
	}
}

void gamemode_editor_state::set_up_exit_animation(animate_title animate_title, animate_subtitle animate_subtitle)
{
	if (bool(animate_title)) {
		m_ui[T_TITLE].move_and_hide(TOP_START_POS, 0.5_s);
	}
	if (bool(animate_subtitle)) {
		m_ui[T_SUBTITLE].move_and_hide(TOP_START_POS, 0.5_s);
	}
	m_ui[T_NAME].move_x_and_hide(600, 0.5_s);
	m_ui[T_AUTHOR].move_x_and_hide(400, 0.5_s);
	m_ui[T_DESCRIPTION].move_x_and_hide(600, 0.5_s);
	m_ui[T_BALL_SETTINGS].move_x_and_hide(400, 0.5_s);
	m_ui[T_PLAYER_SETTINGS].move_x_and_hide(600, 0.5_s);
	m_ui[T_SONG].move_x_and_hide(400, 0.5_s);
	m_ui[T_SONG_C].move_x_and_hide(600, 0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].move_and_hide(BOTTOM_START_POS, 0.5_s);
	}
}