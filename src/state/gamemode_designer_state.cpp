#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

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
	{{tr::sys::keycode::B}, T_BALL_SETTINGS},
	{{tr::sys::keycode::TOP_ROW_1}, T_BALL_SETTINGS},
	{{tr::sys::keycode::P}, T_PLAYER_SETTINGS},
	{{tr::sys::keycode::TOP_ROW_2}, T_PLAYER_SETTINGS},
	{{tr::sys::keycode::T}, T_TEST},
	{{tr::sys::keycode::TOP_ROW_3}, T_TEST},
	{{tr::sys::keycode::S}, T_SAVE},
	{{tr::sys::keycode::ENTER}, T_SAVE},
	{{tr::sys::keycode::TOP_ROW_4}, T_SAVE},
	{{tr::sys::keycode::C}, T_DISCARD},
	{{tr::sys::keycode::Q}, T_DISCARD},
	{{tr::sys::keycode::E}, T_DISCARD},
	{{tr::sys::keycode::ESCAPE}, T_DISCARD},
	{{tr::sys::keycode::TOP_ROW_5}, T_DISCARD},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> NAME_MOVE_IN{tween::CUBIC, glm::vec2{400, 240}, {500, 240}, 0.5_s};
constexpr tweener<glm::vec2> AUTHOR_MOVE_IN{tween::CUBIC, glm::vec2{600, 315}, glm::vec2{500, 315}, 0.5_s};
constexpr tweener<glm::vec2> DESCRIPTION_MOVE_IN{tween::CUBIC, glm::vec2{400, 365}, glm::vec2{500, 365}, 0.5_s};
constexpr tweener<glm::vec2> BALL_SETTINGS_MOVE_IN{tween::CUBIC, glm::vec2{600, 450}, glm::vec2{500, 450}, 0.5_s};
constexpr tweener<glm::vec2> PLAYER_SETTINGS_MOVE_IN{tween::CUBIC, glm::vec2{400, 550}, glm::vec2{500, 550}, 0.5_s};
constexpr tweener<glm::vec2> SONG_MOVE_IN{tween::CUBIC, glm::vec2{600, 650}, glm::vec2{500, 650}, 0.5_s};
constexpr tweener<glm::vec2> SONG_C_MOVE_IN{tween::CUBIC, glm::vec2{400, 700}, glm::vec2{500, 700}, 0.5_s};

// clang-format on

gamemode_designer_state::gamemode_designer_state(std::shared_ptr<playerless_game> game, const gamemode& gamemode,
												 bool returning_from_subscreen)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_GAMEMODE_DESIGNER}
	, m_pending{gamemode}
	, m_available_songs{engine::create_available_song_list()}
{
	set_up_ui(returning_from_subscreen);
}

gamemode_designer_state::gamemode_designer_state(const gamemode& gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS}
	, m_substate{substate::RETURNING_FROM_TEST_GAME}
	, m_pending{gamemode}
	, m_available_songs{engine::create_available_song_list()}
{
	set_up_ui(false);
}

//

std::unique_ptr<tr::state> gamemode_designer_state::update(tr::duration)
{
	main_menu_state::update({});
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_GAMEMODE_DESIGNER;
			m_timer = 0;
		}
		return nullptr;
	case substate::IN_GAMEMODE_DESIGNER:
		return nullptr;
	case substate::ENTERING_TEST_GAME:
		return m_timer >= 0.5_s
				   ? std::make_unique<game_state>(std::make_unique<active_game>(m_pending), game_type::GAMEMODE_DESIGNER_TEST, true)
				   : nullptr;
	case substate::ENTERING_SUBMENU_OR_TITLE:
		return m_timer >= 0.5_s ? m_next_state.get() : nullptr;
	}
}

//

float gamemode_designer_state::fade_overlay_opacity()
{
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		return 1 - m_timer / 0.5_sf;
	case substate::IN_GAMEMODE_DESIGNER:
	case substate::ENTERING_SUBMENU_OR_TITLE:
		return 0;
	case substate::ENTERING_TEST_GAME:
		return m_timer / 0.5_sf;
	}
}

void gamemode_designer_state::set_up_ui(bool returning_from_subscreen)
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
			m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
			m_timer = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			set_up_subscreen_animation();
			m_next_state = make_async<ball_settings_editor_state>(m_game, m_pending);
		},
	};
	const action_callback player_settings_acb{
		[this] {
			m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
			m_timer = 0;
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
			m_timer = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			set_up_exit_animation();
			engine::fade_song_out(0.5s);
		},
		[this] {
			m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
			m_timer = 0;
			m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).buffer;
			m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
			m_pending.save_to_file();
			set_up_exit_animation();
			m_next_state = make_async<title_state>(m_game);
		},
		[this] {
			m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
			m_timer = 0;
			set_up_exit_animation();
			m_next_state = make_async<title_state>(m_game);
		},
	};

	// TEXT CALLBACKS

	text_callback author_tcb{
		string_text_callback{TR_FMT::format("{}: {}", engine::loc["by"], engine::scorefile.name)},
	};
	text_callback song_c_tcb{
		[&] { return std::string{m_pending.song}; },
	};

	//

	if (returning_from_subscreen) {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
								   tr::sys::ttf_style::NORMAL, 64);
	}
	m_ui.emplace<line_input_widget<12>>(T_NAME, NAME_MOVE_IN, tr::align::CENTER, 0.5_s, tr::sys::ttf_style::NORMAL, 120, scb, name_enter_cb,
										m_pending.name);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(author_tcb),
							   tr::sys::ttf_style::NORMAL, 32);
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
		const tweener<glm::vec2> move_in{tween::CUBIC, BOTTOM_START_POS, {500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s};
		m_ui.emplace<text_button_widget>(BOTTOM_BUTTONS[i], move_in, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP,
										 loc_text_callback{BOTTOM_BUTTONS[i]}, font::LANGUAGE, 48, bottom_scbs[i], bottom_acbs[i], sound);
	}
}

void gamemode_designer_state::set_up_subscreen_animation()
{
	widget& name{m_ui[T_NAME]};
	widget& author{m_ui[T_AUTHOR]};
	widget& description{m_ui[T_DESCRIPTION]};
	widget& ball_settings{m_ui[T_BALL_SETTINGS]};
	widget& player_settings{m_ui[T_PLAYER_SETTINGS]};
	widget& song{m_ui[T_SONG]};
	widget& song_c{m_ui[T_SONG_C]};
	name.pos.change(tween::CUBIC, {600, glm::vec2{name.pos}.y}, 0.5_s);
	author.pos.change(tween::CUBIC, {400, glm::vec2{author.pos}.y}, 0.5_s);
	description.pos.change(tween::CUBIC, {600, glm::vec2{description.pos}.y}, 0.5_s);
	ball_settings.pos.change(tween::CUBIC, {400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	player_settings.pos.change(tween::CUBIC, {600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	song.pos.change(tween::CUBIC, {400, glm::vec2{song.pos}.y}, 0.5_s);
	song_c.pos.change(tween::CUBIC, {600, glm::vec2{song_c.pos}.y}, 0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
	m_ui[T_TITLE].unhide();
}

void gamemode_designer_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	widget& name{m_ui[T_NAME]};
	widget& author{m_ui[T_AUTHOR]};
	widget& description{m_ui[T_DESCRIPTION]};
	widget& ball_settings{m_ui[T_BALL_SETTINGS]};
	widget& player_settings{m_ui[T_PLAYER_SETTINGS]};
	widget& song{m_ui[T_SONG]};
	widget& song_c{m_ui[T_SONG_C]};
	name.pos.change(tween::CUBIC, {600, glm::vec2{name.pos}.y}, 0.5_s);
	author.pos.change(tween::CUBIC, {400, glm::vec2{author.pos}.y}, 0.5_s);
	description.pos.change(tween::CUBIC, {600, glm::vec2{description.pos}.y}, 0.5_s);
	ball_settings.pos.change(tween::CUBIC, {400, glm::vec2{ball_settings.pos}.y}, 0.5_s);
	player_settings.pos.change(tween::CUBIC, {600, glm::vec2{player_settings.pos}.y}, 0.5_s);
	song.pos.change(tween::CUBIC, {400, glm::vec2{song.pos}.y}, 0.5_s);
	song_c.pos.change(tween::CUBIC, {600, glm::vec2{song_c.pos}.y}, 0.5_s);
	for (tag tag : BOTTOM_BUTTONS) {
		m_ui[tag].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
}