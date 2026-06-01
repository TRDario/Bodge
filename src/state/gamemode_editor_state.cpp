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

// clang-format on
/////////////////////////////////////////////////////////// GAMEMODE EDITOR TYPE //////////////////////////////////////////////////////////

localized_text new_gamemode_editor::subtitle_text() const
{
	return localized_text{"new_gamemode"};
}

void new_gamemode_editor::on_save(gamemode_editor_state& state) const
{
	g_new_gamemode_draft = gamemode{};
	state.m_pending.save_to_file();
	state.m_next_state = make_async<title_state>(state.m_game);
	state.set_up_exit_animation(animate_title::YES, animate_subtitle::YES);
}

void new_gamemode_editor::on_discard(gamemode_editor_state& state) const
{
	g_new_gamemode_draft = state.m_pending;
	state.m_next_state = make_async<gamemode_manager_state>(state.m_game, animate_title::NO);
	state.set_up_exit_animation(animate_title::NO, animate_subtitle::YES);
}

//

edited_gamemode_editor::edited_gamemode_editor(std::filesystem::path path)
	: m_path{std::move(path)}
{
}

localized_text edited_gamemode_editor::subtitle_text() const
{
	return localized_text{"edit_gamemode"};
}

void edited_gamemode_editor::on_save(gamemode_editor_state& state) const
{
	std::filesystem::remove(m_path);
	state.m_pending.save_to_file();
	state.m_next_state = make_async<gamemode_selector_state>(state.m_game, edit_gamemode_selector{}, animate_subtitle::NO);
	state.set_up_exit_animation(animate_title::NO, animate_subtitle::NO);
}

void edited_gamemode_editor::on_discard(gamemode_editor_state& state) const
{
	state.m_next_state = make_async<gamemode_selector_state>(state.m_game, edit_gamemode_selector{}, animate_subtitle::NO);
	state.set_up_exit_animation(animate_title::NO, animate_subtitle::NO);
}

//

localized_text cloned_gamemode_editor::subtitle_text() const
{
	return localized_text{"clone_gamemode"};
}

void cloned_gamemode_editor::on_save(gamemode_editor_state& state) const
{
	state.m_pending.save_to_file();
	state.m_next_state = make_async<title_state>(state.m_game);
	state.set_up_exit_animation(animate_title::YES, animate_subtitle::YES);
}

void cloned_gamemode_editor::on_discard(gamemode_editor_state& state) const
{
	state.m_next_state = make_async<gamemode_selector_state>(state.m_game, clone_gamemode_selector{}, animate_subtitle::NO);
	state.set_up_exit_animation(animate_title::NO, animate_subtitle::NO);
}

////////////////////////////////////////////////////////// GAMEMODE EDITOR STATE //////////////////////////////////////////////////////////

gamemode_editor_state::gamemode_editor_state(gamemode_editor data, gamemode gamemode)
	: main_menu_state{SELECTION_TREE, SHORTCUTS}
	, m_substate{substate::RETURNING_FROM_TEST_GAME}
	, m_type{std::move(data)}
	, m_available_songs{create_available_song_list()}
	, m_pending{std::move(gamemode)}
{
	set_up_ui(animate_title::YES, animate_subtitle::YES);
}

gamemode_editor_state::gamemode_editor_state(std::shared_ptr<playerless_game> game, gamemode_editor data, gamemode gamemode,
											 animate_subtitle animate_subtitle)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_GAMEMODE_EDITOR}
	, m_type{std::move(data)}
	, m_available_songs{create_available_song_list()}
	, m_pending{std::move(gamemode)}
{
	m_pending.author = g_scorefile.name;

	set_up_ui(animate_title::NO, animate_subtitle);
}

//

bool gamemode_editor_state::transparent_cursor() const
{
	return m_substate == substate::ENTERING_TEST_GAME;
}

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
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = bool(animate_title) ? tweened_position{TOP_START_POS, TITLE_POS, 0.5_s} : tweened_position{TITLE_POS},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = 0,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_SUBTITLE, {
		.animation = bool(animate_subtitle) ? tweened_position{TOP_START_POS, {500, 64}, 0.5_s} : tweened_position{{500, 64}},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = bool(animate_subtitle) ? 0.5_s : 0,
		.text = std::visit([](const auto& type) { return type.subtitle_text(); }, m_type),
		.font_size = 32
	});
	m_ui.emplace<line_input_widget<12>>(T_NAME, {
		.animation = {{400, 240}, {500, 240}, 0.5_s},
		.font_size = 120,
		.status = [this] { return m_substate == substate::IN_GAMEMODE_EDITOR; },
		.enter_action = [this] { m_ui.select_next_widget(); },
		.initial_text = m_pending.name
	});
	m_ui.emplace<label_widget>(T_AUTHOR, {
		.animation = {{600, 315}, {500, 315}, 0.5_s},
		.text = constant_text{TR_FMT::format("{}: {}", g_loc["by"], g_scorefile.name)},
		.font_size = 32
	});
	m_ui.emplace<line_input_widget<40>>(T_DESCRIPTION, {
		.animation = {{400, 365}, {500, 365}, 0.5_s},
		.font_style = tr::sys::ttf_style::ITALIC,
		.font_size = 32,
		.status = [this] { return m_substate == substate::IN_GAMEMODE_EDITOR; },
		.enter_action = [this] { m_ui.clear_selection(); },
		.initial_text = m_pending.description
	});
	m_ui.emplace<text_button_widget>(T_BALL_SETTINGS, {
		.animation = {{600, 450}, {500, 450}, 0.5_s},
		.text = localized_text{T_BALL_SETTINGS}, 
		.font_size = 64,
		.status = [this] { return m_substate == substate::IN_GAMEMODE_EDITOR; },
		.action = [this] { on_enter_ball_settings(); }
	});
	m_ui.emplace<text_button_widget>(T_PLAYER_SETTINGS, {
		.animation = {{400, 550}, {500, 550}, 0.5_s},
		.text = localized_text{T_PLAYER_SETTINGS},
		.font_size = 64,
		.status = [this] { return m_substate == substate::IN_GAMEMODE_EDITOR; },
		.action = [this] { on_enter_player_settings(); }
	});
	m_ui.emplace<label_widget>(T_SONG, {
		.animation = {{600, 650}, {500, 650}, 0.5_s},
		.text = localized_text{T_SONG}
	});
	m_ui.emplace<text_button_widget>(T_SONG_C, {
		.animation = {{400, 700}, {500, 700}, 0.5_s},
		.text = [this] { return std::string{m_pending.song}; },
		.font_size = 64,
		.status = [this] { return m_substate == substate::IN_GAMEMODE_EDITOR; },
		.action = [this] { on_change_song(); }
	});

	struct bottom_button_parameters {
		text_command tooltip_text;
		status_command status_command;
		action_command action_command;
		sound sound;
	};
	const std::array<bottom_button_parameters, BOTTOM_BUTTONS.size()> bottom_button_parameters{{
		{.tooltip_text = NO_TOOLTIP,
		 .status_command = [this] { return m_substate == substate::IN_GAMEMODE_EDITOR; },
		 .action_command = [this] { on_test(); },
		 .sound = sound::CONFIRM},
		{.tooltip_text = [this] { return m_ui.as<line_input_widget<12>>(T_NAME).contents().empty() ? std::string{g_loc["save_gamemode_tt"]} : std::string{}; },
		 .status_command = [this] { return m_substate == substate::IN_GAMEMODE_EDITOR && !m_ui.as<line_input_widget<12>>(T_NAME).contents().empty(); },
		 .action_command = [this] { on_save(); },
		 .sound = sound::CONFIRM},
		{.tooltip_text = NO_TOOLTIP,
		 .status_command = [this] { return m_substate == substate::IN_GAMEMODE_EDITOR; },
		 .action_command = [this] { on_discard(); },
		 .sound = sound::CANCEL}
	}};
	for (usize i = 0; i < BOTTOM_BUTTONS.size(); ++i) {
		m_ui.emplace<text_button_widget>(BOTTOM_BUTTONS[i], {
			.animation = {BOTTOM_START_POS, {500, 1000 - BOTTOM_BUTTONS.size() * 50 + (i + 1) * 50}, 0.5_s},
			.alignment = tr::align::BOTTOM_CENTER,
			.tooltip_text = bottom_button_parameters[i].tooltip_text,
			.text = localized_text{BOTTOM_BUTTONS[i]},
			.status = bottom_button_parameters[i].status_command,
			.action = bottom_button_parameters[i].action_command,
			.action_sound = bottom_button_parameters[i].sound
		});
	}
	// clang-format on
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

//

void gamemode_editor_state::on_enter_player_settings()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).contents();
	m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).contents();
	set_up_exit_animation(animate_title::NO, animate_subtitle::YES);
	m_next_state = make_async<player_settings_editor_state>(m_game, m_type, m_pending);
}

void gamemode_editor_state::on_enter_ball_settings()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).contents();
	m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).contents();
	set_up_exit_animation(animate_title::NO, animate_subtitle::YES);
	m_next_state = make_async<ball_settings_editor_state>(m_game, m_type, m_pending);
}

void gamemode_editor_state::on_change_song()
{
	std::vector<std::string>::iterator song_it{std::ranges::find(m_available_songs, m_pending.song)};
	if (song_it == m_available_songs.end() || ++song_it == m_available_songs.end()) {
		song_it = m_available_songs.begin();
	}
	m_pending.song = *song_it;
}

void gamemode_editor_state::on_test()
{
	m_substate = substate::ENTERING_TEST_GAME;
	m_elapsed = 0;
	m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).contents();
	m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).contents();
	set_up_exit_animation(animate_title::YES, animate_subtitle::YES);
	g_audio.fade_song_out(0.5s);
	m_next_state = make_game_state_async<active_game>(test_game_data{m_type}, m_pending);
}

void gamemode_editor_state::on_save()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).contents();
	m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).contents();
	std::visit([this](auto& type) { type.on_save(*this); }, m_type);
}

void gamemode_editor_state::on_discard()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	m_pending.name = m_ui.as<line_input_widget<12>>(T_NAME).contents();
	m_pending.description = m_ui.as<line_input_widget<40>>(T_DESCRIPTION).contents();
	std::visit([this](auto& type) { type.on_discard(*this); }, m_type);
}