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
	{"Enter"_kc, T_DISCARD},
	{"5"_kc, T_DISCARD},
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
	, m_substate{returning_from_subscreen ? substate::RETURNING_FROM_SUBSCREEN : substate::IN_GAMEMODE_DESIGNER}
	, m_pending{gamemode}
	, m_available_songs{create_available_song_list()}
{
}

gamemode_designer_state::gamemode_designer_state(const gamemode& gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS}
	, m_substate{substate::RETURNING_FROM_TEST_GAME}
	, m_pending{gamemode}
	, m_available_songs{create_available_song_list()}
{
}

//

void gamemode_designer_state::set_up_ui()
{
	using enum tr::align;

	// TEXT CALLBACKS

	text_callback author_tcb{string_text{TR_FMT::format("{}: {}", g_loc["by"], g_scorefile.name)}};

	//

	if (m_substate == substate::RETURNING_FROM_SUBSCREEN) {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, TOP_CENTER, 0_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	}
	else {
		m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	}
	m_ui.emplace<line_input_widget<12>>(T_NAME, NAME_MOVE_IN, CENTER, 0.5_s, text_style::NORMAL, 120, status_callback, on_name,
										m_pending.name);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, std::move(author_tcb), text_style::NORMAL, 32);
	m_ui.emplace<line_input_widget<40>>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, CENTER, 0.5_s, text_style::ITALIC, 32, status_callback,
										on_description, m_pending.description);
	m_ui.emplace<text_button_widget>(T_BALL_SETTINGS, BALL_SETTINGS_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_BALL_SETTINGS},
									 font::LANGUAGE, 64, status_callback, on_ball_settings, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_PLAYER_SETTINGS, PLAYER_SETTINGS_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_PLAYER_SETTINGS},
									 font::LANGUAGE, 64, status_callback, on_ball_settings, sound::CONFIRM);
	m_ui.emplace<label_widget>(T_SONG, SONG_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_SONG}, text_style::NORMAL, 48);
	m_ui.emplace<text_button_widget>(T_SONG_C, SONG_C_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, song_c_text, font::LANGUAGE, 64, status_callback,
									 on_song_c, sound::CONFIRM);
	for (usize i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		const sound sound{i != BOTTOM_BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL};
		const tweener<glm::vec2> move_in{tween::CUBIC, BOTTOM_START_POS, {500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s};
		m_ui.emplace<text_button_widget>(BOTTOM_BUTTONS[i], move_in, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{BOTTOM_BUTTONS[i]},
										 font::LANGUAGE, 48, BOTTOM_STATUS_CALLBACKS[i], BOTTOM_ACTION_CALLBACKS[i], sound);
	}
}

next_state gamemode_designer_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
	case substate::RETURNING_FROM_SUBSCREEN:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_GAMEMODE_DESIGNER;
			m_timer = 0;
		}
		return tr::KEEP_STATE;
	case substate::IN_GAMEMODE_DESIGNER:
		return tr::KEEP_STATE;
	case substate::ENTERING_TEST_GAME:
	case substate::ENTERING_SUBMENU_OR_TITLE:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
	}
}

//

float gamemode_designer_state::fade_overlay_opacity()
{
	switch (m_substate) {
	case substate::RETURNING_FROM_TEST_GAME:
		return 1 - m_timer / 0.5_sf;
	case substate::RETURNING_FROM_SUBSCREEN:
	case substate::IN_GAMEMODE_DESIGNER:
	case substate::ENTERING_SUBMENU_OR_TITLE:
		return 0;
	case substate::ENTERING_TEST_GAME:
		return m_timer / 0.5_sf;
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

//

std::string gamemode_designer_state::song_c_text()
{
	const gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	return std::string{self.m_pending.song};
}

bool gamemode_designer_state::status_callback()
{
	const gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	return self.m_substate == substate::IN_GAMEMODE_DESIGNER;
}

bool gamemode_designer_state::save_interactible()
{
	const gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	return self.m_substate == substate::IN_GAMEMODE_DESIGNER && !self.m_ui.as<line_input_widget<12>>(T_NAME).buffer.empty();
}

void gamemode_designer_state::on_name()
{
	gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	self.m_ui.select_next_widget();
}

void gamemode_designer_state::on_description()
{
	gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	self.m_ui.clear_selection();
}

void gamemode_designer_state::on_ball_settings()
{
	gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	self.m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
	self.m_timer = 0;
	self.m_pending.name = self.m_ui.as<line_input_widget<12>>(T_NAME).buffer;
	self.m_pending.description = self.m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
	self.set_up_subscreen_animation();
	prepare_next_state<ball_settings_editor_state>(self.m_game, self.m_pending);
}

void gamemode_designer_state::on_player_settings()
{
	gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	self.m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
	self.m_timer = 0;
	self.m_pending.name = self.m_ui.as<line_input_widget<12>>(T_NAME).buffer;
	self.m_pending.description = self.m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
	self.set_up_subscreen_animation();
	prepare_next_state<player_settings_editor_state>(self.m_game, self.m_pending);
}

void gamemode_designer_state::on_song_c()
{
	gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	std::vector<std::string>::iterator it{std::ranges::find(self.m_available_songs, self.m_pending.song)};
	if (it == self.m_available_songs.end() || ++it == self.m_available_songs.end()) {
		it = self.m_available_songs.begin();
	}
	self.m_pending.song = *it;
}

void gamemode_designer_state::on_test()
{
	gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	self.m_substate = substate::ENTERING_TEST_GAME;
	self.m_timer = 0;
	self.m_pending.name = self.m_ui.as<line_input_widget<12>>(T_NAME).buffer;
	self.m_pending.description = self.m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
	self.set_up_exit_animation();
	g_audio.fade_song_out(0.5s);
	prepare_next_game_state<active_game>(game_type::GAMEMODE_DESIGNER_TEST, true, self.m_pending);
}

void gamemode_designer_state::on_save()
{
	gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	self.m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
	self.m_timer = 0;
	self.m_pending.name = self.m_ui.as<line_input_widget<12>>(T_NAME).buffer;
	self.m_pending.description = self.m_ui.as<line_input_widget<40>>(T_DESCRIPTION).buffer;
	self.m_pending.save_to_file();
	self.set_up_exit_animation();
	prepare_next_state<title_state>(self.m_game);
}

void gamemode_designer_state::on_discard()
{
	gamemode_designer_state& self{g_state_machine.get<gamemode_designer_state>()};

	self.m_substate = substate::ENTERING_SUBMENU_OR_TITLE;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<title_state>(self.m_game);
}