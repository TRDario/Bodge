///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements start_game_state from state.hpp.                                                                                           //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

constexpr tag T_TITLE{"start_game"};
constexpr tag T_NAME{"name"};
constexpr tag T_AUTHOR{"author"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_BEST_TIME_LABEL{"best_time"};
constexpr tag T_BEST_TIME{"best_time_display"};
constexpr tag T_BEST_SCORE_LABEL{"best_score"};
constexpr tag T_BEST_SCORE{"best_score_display"};
constexpr tag T_PREV{"prev"};
constexpr tag T_NEXT{"next"};
constexpr tag T_START{"start"};
constexpr tag T_EXIT{"exit"};

// Gamemode display widgets.
constexpr std::array GAMEMODE_WIDGETS{T_NAME, T_AUTHOR, T_DESCRIPTION, T_BEST_TIME_LABEL, T_BEST_TIME, T_BEST_SCORE_LABEL, T_BEST_SCORE};

// Selection tree for the start game menu.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_START},
	selection_tree_row{T_EXIT},
};

// Shortcut table for the start game menu.
constexpr shortcut_table SHORTCUTS{
	{"Left"_kc, T_PREV},
	{"Right"_kc, T_NEXT},
	{"Enter"_kc, T_START}, {"1"_kc, T_START},
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT}, {"2"_kc, T_EXIT},
};

// The base x positions of the gamemode information widgets.
constexpr std::array GAMEMODE_WIDGETS_BASE_X{500.0f, 500.0f, 500.0f, 325.0f, 325.0f, 675.0f, 675.0f};

// clang-format on
//////////////////////////////////////////////////////////// INTERNAL HELPERS /////////////////////////////////////////////////////////////

// Which side the widgets are moving from on page transition.
enum class starting_side : bool {
	LEFT,
	RIGHT
};

// Emplaces a label widget into a map.
static void emplace_label_widget(std::unordered_map<tag, std::unique_ptr<widget>>& map, tag tag, label_widget::properties&& properties)
{
	map.emplace(tag, std::make_unique<label_widget>(std::move(properties)));
}

// Creates a set of widgets for a different gamemode.
static std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets(const gamemode& selected, starting_side side)
{
	const float label_h{621 - g_text_engine.line_skip(font::LANGUAGE, 32)};

	// clang-format off
	std::unordered_map<tag, std::unique_ptr<widget>> map;
	emplace_label_widget(map, T_NAME, {
		.animation = {{side == starting_side::LEFT ? 250 : 750, 400}, {500, 400}, 0.25_s},
		.unhide_time = 0.25_s,
		.text = constant_text{std::string{selected.name_loc()}},
		.font_size = 120
	});
	emplace_label_widget(map, T_AUTHOR, {
		.animation = {{side == starting_side::LEFT ? 250 : 750, 475}, {500, 475}, 0.25_s},
		.unhide_time = 0.25_s,
		.text = constant_text{TR_FMT::format("{}: {}", g_loc["by"], selected.author)},
		.font_size = 32
	});
	emplace_label_widget(map, T_DESCRIPTION, {
		.animation = {{side == starting_side::LEFT ? 250 : 750, 525}, {500, 525}, 0.25_s},
		.unhide_time = 0.25_s,
		.text = constant_text{std::string{selected.description_loc_with_fallback()}},
		.font_style = tr::sys::ttf_style::ITALIC,
		.font_size = 32,
		.color = DARK_GRAY
	});
	emplace_label_widget(map, T_BEST_TIME_LABEL, {
		.animation = {{side == starting_side::LEFT ? 75 : 525, label_h}, {325, label_h}, 0.25_s},
		.unhide_time = 0.25_s,
		.text = localized_text{T_BEST_TIME_LABEL},
		.font_size = 32,
		.color = YELLOW
	});
	emplace_label_widget(map, T_BEST_TIME, {
		.animation = {{side == starting_side::LEFT ? 75 : 525, 625}, {325, 625}, 0.25_s},
		.unhide_time = 0.25_s,
		.text = constant_text{format_time(g_scorefile.bests(selected).time)},
		.font_size = 64,
		.color = YELLOW
	});
	emplace_label_widget(map, T_BEST_SCORE_LABEL, {
		.animation = {{side == starting_side::LEFT ? 425 : 925, label_h}, {675, label_h}, 0.25_s},
		.unhide_time = 0.25_s,
		.text = localized_text{T_BEST_SCORE_LABEL},
		.font_size = 32,
		.color = YELLOW
	});
	emplace_label_widget(map, T_BEST_SCORE, {
		.animation = {{side == starting_side::LEFT ? 425 : 925, 625}, {675, 625}, 0.25_s},
		.unhide_time = 0.25_s,
		.text = constant_text{format_score(g_scorefile.bests(selected).score)},
		.font_size = 64,
		.color = YELLOW
	});
	return map;
	// clang-format on
}

//////////////////////////////////////////////////////////// START GAME WIDGET ////////////////////////////////////////////////////////////

start_game_state::start_game_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::ENTERING_START_GAME}
	, m_gamemodes{load_gamemodes()}
	, m_selected{m_gamemodes.begin()}
{
	std::vector<gamemode_with_path>::iterator last_selected_it{
		std::ranges::find(m_gamemodes, g_scorefile.last_selected, &gamemode_with_path::gamemode),
	};
	if (last_selected_it != m_gamemodes.end()) {
		m_selected = last_selected_it;
	}

	const float label_h{621 - g_text_engine.line_skip(font::LANGUAGE, 32)};
	const bests bests{g_scorefile.bests(m_selected->gamemode)};

	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = {TOP_START_POS, TITLE_POS, 0.5_s},
		.alignment = tr::align::TOP_CENTER,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_NAME, {
		.animation = {{500, 300}, {500, 400}, 0.5_s},
		.text = constant_text{std::string{m_selected->gamemode.name_loc()}},
		.font_size = 120
	});
	m_ui.emplace<label_widget>(T_AUTHOR, {
		.animation = {{400, 475}, {500, 475}, 0.5_s},
		.text = constant_text{TR_FMT::format("{}: {}", g_loc["by"], m_selected->gamemode.author)},
		.font_size = 32
	});
	m_ui.emplace<label_widget>(T_DESCRIPTION, {
		.animation = {{600, 525}, {500, 525}, 0.5_s},
		.text = constant_text{m_selected->gamemode.description_loc_with_fallback()},
		.font_style = tr::sys::ttf_style::ITALIC,
		.font_size = 32,
		.color = DARK_GRAY
	});
	m_ui.emplace<label_widget>(T_BEST_TIME_LABEL, {
		.animation = {{325, label_h + 100}, {325, label_h}, 0.5_s},
		.text = localized_text{T_BEST_TIME_LABEL},
		.font_size = 32,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_BEST_TIME, {
		.animation = {{325, 725}, {325, 625}, 0.5_s},
		.text = constant_text{format_time(bests.time)},
		.font_size = 64,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_BEST_SCORE_LABEL, {
		.animation = {{675, label_h + 100}, {675, label_h}, 0.5_s},
		.text = localized_text{T_BEST_SCORE_LABEL},
		.font_size = 32,
		.color = YELLOW
	});
	m_ui.emplace<label_widget>(T_BEST_SCORE, {
		.animation = {{675, 725}, {675, 625}, 0.5_s},
		.text = constant_text{format_score(bests.score)},
		.font_size = 64,
		.color = YELLOW
	});
	m_ui.emplace<arrow_widget>(T_PREV, {
		.animation = {{-50, 500}, {10, 500}, 0.5_s},
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_START_GAME; },
		.action = [this] { on_previous_gamemode(); }
	});
	m_ui.emplace<arrow_widget>(T_NEXT, {
		.animation = {{1050, 500}, {990, 500}, 0.5_s},
		.type = arrow_type::RIGHT,
		.status = [this] { return m_substate == substate::IN_START_GAME; },
		.action = [this] { on_next_gamemode(); }
	});
	m_ui.emplace<text_button_widget>(T_START,
		tweened_position{BOTTOM_START_POS, {500, 950}, 0.5_s},
		tr::align::BOTTOM_CENTER,
		0.5_s,
		NO_TOOLTIP,
		localized_text{T_START},
		font::LANGUAGE,
		48,
		[this] { return m_substate == substate::IN_START_GAME || m_substate == substate::ENTERING_START_GAME; },
		[this] { on_start(); },
		sound::CONFIRM
	);
	m_ui.emplace<text_button_widget>(T_EXIT,
		tweened_position{BOTTOM_START_POS, {500, 1000}, 0.5_s},
		tr::align::BOTTOM_CENTER,
		0.5_s,
		NO_TOOLTIP,
		localized_text{T_EXIT},
		font::LANGUAGE,
		48,
		[this] { return m_substate == substate::IN_START_GAME || m_substate == substate::ENTERING_START_GAME; },
		[this] { on_exit(); },
		sound::CANCEL
	);
	// clang-format on
}

//

float start_game_state::fade_overlay_opacity()
{
	return m_substate == substate::STARTING_GAME ? m_elapsed / 0.5_sf : 0;
}

bool start_game_state::transparent_cursor() const
{
	return m_substate == substate::STARTING_GAME;
}

tr::next_state start_game_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::ENTERING_START_GAME:
		if (m_elapsed >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_elapsed = 0;
		}
		return tr::KEEP_STATE;
	case substate::IN_START_GAME:
		return tr::KEEP_STATE;
	case substate::SWITCHING_GAMEMODE:
		if (m_elapsed >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_elapsed = 0;
		}
		else if (m_elapsed == 0.25_s) {
			m_ui.replace(m_next_widgets.get());
		}
		return tr::KEEP_STATE;
	case substate::EXITING_TO_TITLE:
	case substate::STARTING_GAME:
		return next_state_if_after(0.5_s);
	}
}

//

void start_game_state::set_up_exit_animation()
{
	const float label_h{621 - g_text_engine.line_skip(font::LANGUAGE, 32)};

	m_ui[T_NAME].move_y_and_hide(300, 0.5_s);
	m_ui[T_AUTHOR].move_x_and_hide(600, 0.5_s);
	m_ui[T_DESCRIPTION].move_x_and_hide(400, 0.5_s);
	m_ui[T_BEST_TIME_LABEL].move_y_and_hide(label_h + 100, 0.5_s);
	m_ui[T_BEST_TIME].move_y_and_hide(725, 0.5_s);
	m_ui[T_BEST_SCORE_LABEL].move_y_and_hide(label_h + 100, 0.5_s);
	m_ui[T_BEST_SCORE].move_y_and_hide(725, 0.5_s);
	m_ui[T_TITLE].move_and_hide(TOP_START_POS, 0.5_s);
	m_ui[T_PREV].move_x_and_hide(-100, 0.5_s);
	m_ui[T_NEXT].move_x_and_hide(1100, 0.5_s);
	m_ui[T_START].move_and_hide(BOTTOM_START_POS, 0.5_s);
	m_ui[T_EXIT].move_and_hide(BOTTOM_START_POS, 0.5_s);
}

//

void start_game_state::on_previous_gamemode()
{
	m_selected = m_selected == m_gamemodes.begin() ? m_selected = m_gamemodes.end() - 1 : std::prev(m_selected);
	m_substate = substate::SWITCHING_GAMEMODE;
	m_elapsed = 0;
	for (usize i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
		m_ui[GAMEMODE_WIDGETS[i]].move_x_and_hide(GAMEMODE_WIDGETS_BASE_X[i] + 250, 0.25_s);
	}
	m_next_widgets = std::async(std::launch::async, prepare_next_widgets, m_selected->gamemode, starting_side::LEFT);
}

void start_game_state::on_next_gamemode()
{
	if (++m_selected == m_gamemodes.end()) {
		m_selected = m_gamemodes.begin();
	}
	m_substate = substate::SWITCHING_GAMEMODE;
	m_elapsed = 0;
	for (usize i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
		m_ui[GAMEMODE_WIDGETS[i]].move_x_and_hide(GAMEMODE_WIDGETS_BASE_X[i] - 250, 0.25_s);
	}
	m_next_widgets = std::async(std::launch::async, prepare_next_widgets, m_selected->gamemode, starting_side::RIGHT);
}

void start_game_state::on_start()
{
	m_substate = substate::STARTING_GAME;
	m_elapsed = 0;
	set_up_exit_animation();
	g_scorefile.last_selected = m_selected->gamemode;
	g_audio.fade_song_out(0.5s);
	m_next_state = make_game_state_async<active_game>(regular_game_data{}, m_selected->gamemode);
}

void start_game_state::on_exit()
{
	m_substate = substate::EXITING_TO_TITLE;
	m_elapsed = 0;
	set_up_exit_animation();
	g_scorefile.last_selected = m_selected->gamemode;
	m_next_state = make_async<title_state>(m_game);
}