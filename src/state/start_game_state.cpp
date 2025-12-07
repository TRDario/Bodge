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
constexpr std::array<tag, 7> GAMEMODE_WIDGETS{T_NAME, T_AUTHOR, T_DESCRIPTION, T_BEST_TIME_LABEL, T_BEST_TIME, T_BEST_SCORE_LABEL, T_BEST_SCORE};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_START},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{"Left"_kc, T_PREV},
	{"Right"_kc, T_NEXT},
	{"Enter"_kc, T_START},
	{"1"_kc, T_START},
	{"Escape"_kc, T_EXIT},
	{"2"_kc, T_EXIT},
};

// The base x positions of the gamemode information widgets.
constexpr std::array<float, 7> GAMEMODE_WIDGETS_BASE_X{500, 500, 500, 325, 325, 675, 675};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweened_position NAME_MOVE_IN{{500, 300}, {500, 400}, 0.5_s};
constexpr tweened_position AUTHOR_MOVE_IN{{400, 475}, {500, 475}, 0.5_s};
constexpr tweened_position DESCRIPTION_MOVE_IN{{600, 525}, {500, 525}, 0.5_s};
constexpr tweened_position BEST_TIME_MOVE_IN{{325, 725}, {325, 625}, 0.5_s};
constexpr tweened_position BEST_SCORE_MOVE_IN{{675, 725}, {675, 625}, 0.5_s};
constexpr tweened_position PREV_MOVE_IN{{-50, 500}, {10, 500}, 0.5_s};
constexpr tweened_position NEXT_MOVE_IN{{1050, 500}, {990, 500}, 0.5_s};
constexpr tweened_position START_MOVE_IN{BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweened_position EXIT_MOVE_IN{BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on
//////////////////////////////////////////////////////////// INTERNAL HELPERS /////////////////////////////////////////////////////////////

// Which side the widgets are moving from on page transition.
enum class starting_side : bool {
	LEFT,
	RIGHT
};

// Emplaces a label widget into a map.
template <class... Args> static void emplace_label_widget(std::unordered_map<tag, std::unique_ptr<widget>>& map, tag tag, Args&&... args)
{
	map.emplace(tag, std::make_unique<label_widget>(std::forward<Args>(args)...));
}

// Creates a set of widgets for a different gamemode.
static std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets(const gamemode& selected, starting_side side)
{
	// MOVE-INS

	const float label_h{621 - g_text_engine.line_skip(font::LANGUAGE, 32)};
	const tweened_position name_move_in{{side == starting_side::LEFT ? 250 : 750, 400}, {500, 400}, 0.25_s};
	const tweened_position author_move_in{{side == starting_side::LEFT ? 250 : 750, 475}, {500, 475}, 0.25_s};
	const tweened_position description_move_in{{side == starting_side::LEFT ? 250 : 750, 525}, {500, 525}, 0.25_s};
	const tweened_position best_time_label_move_in{{side == starting_side::LEFT ? 75 : 525, label_h}, {325, label_h}, 0.25_s};
	const tweened_position best_time_move_in{{side == starting_side::LEFT ? 75 : 525, 625}, {325, 625}, 0.25_s};
	const tweened_position best_score_label_move_in{{side == starting_side::LEFT ? 425 : 925, label_h}, {675, label_h}, 0.25_s};
	const tweened_position best_score_move_in{{side == starting_side::LEFT ? 425 : 925, 625}, {675, 625}, 0.25_s};

	//

	std::unordered_map<tag, std::unique_ptr<widget>> map;
	emplace_label_widget(map, T_NAME, name_move_in, tr::align::CENTER, 0.25_s, NO_TOOLTIP,
						 const_text_callback{std::string{selected.name_loc()}}, tr::sys::ttf_style::NORMAL, 120);
	emplace_label_widget(map, T_AUTHOR, author_move_in, tr::align::CENTER, 0.25_s, NO_TOOLTIP,
						 const_text_callback{TR_FMT::format("{}: {}", g_loc["by"], selected.author)}, tr::sys::ttf_style::NORMAL, 32);
	emplace_label_widget(map, T_DESCRIPTION, description_move_in, tr::align::CENTER, 0.25_s, NO_TOOLTIP,
						 const_text_callback{std::string{selected.description_loc_with_fallback()}}, tr::sys::ttf_style::ITALIC, 32,
						 DARK_GRAY);
	emplace_label_widget(map, T_BEST_TIME_LABEL, best_time_label_move_in, tr::align::CENTER, 0.25_s, NO_TOOLTIP,
						 loc_text_callback{T_BEST_TIME_LABEL}, tr::sys::ttf_style::NORMAL, 32, YELLOW);
	emplace_label_widget(map, T_BEST_TIME, best_time_move_in, tr::align::CENTER, 0.25_s, NO_TOOLTIP,
						 const_text_callback{format_time(g_scorefile.bests(selected).time)}, tr::sys::ttf_style::NORMAL, 64, YELLOW);
	emplace_label_widget(map, T_BEST_SCORE_LABEL, best_score_label_move_in, tr::align::CENTER, 0.25_s, NO_TOOLTIP,
						 loc_text_callback{T_BEST_SCORE_LABEL}, tr::sys::ttf_style::NORMAL, 32, YELLOW);
	emplace_label_widget(map, T_BEST_SCORE, best_score_move_in, tr::align::CENTER, 0.25_s, NO_TOOLTIP,
						 const_text_callback{format_score(g_scorefile.bests(selected).score)}, tr::sys::ttf_style::NORMAL, 64, YELLOW);
	return map;
}

//////////////////////////////////////////////////////////// START GAME WIDGET ////////////////////////////////////////////////////////////

start_game_state::start_game_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::ENTERING_START_GAME}
	, m_gamemodes{load_gamemodes()}
	, m_selected{m_gamemodes.begin()}
{
	std::vector<gamemode>::iterator last_selected_it{std::ranges::find(m_gamemodes, g_scorefile.last_selected)};
	if (last_selected_it != m_gamemodes.end()) {
		m_selected = last_selected_it;
	}

	// MOVE-INS

	const float label_h{621 - g_text_engine.line_skip(font::LANGUAGE, 32)};
	const tweened_position best_time_label_move_in{{325, label_h + 100}, {325, label_h}, 0.5_s};
	const tweened_position best_score_label_move_in{{675, label_h + 100}, {675, label_h}, 0.5_s};

	// STATUS CALLBACKS

	const status_callback scb{[this] { return m_substate == substate::IN_START_GAME || m_substate == substate::ENTERING_START_GAME; }};
	const status_callback arrow_scb{[this] { return m_substate == substate::IN_START_GAME; }};

	// ACTION CALLBACKS

	const action_callback prev_acb{[this] {
		m_selected = m_selected == m_gamemodes.begin() ? m_selected = m_gamemodes.end() - 1 : std::prev(m_selected);
		m_substate = substate::SWITCHING_GAMEMODE;
		m_elapsed = 0;
		for (usize i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
			m_ui[GAMEMODE_WIDGETS[i]].move_x_and_hide(GAMEMODE_WIDGETS_BASE_X[i] + 250, 0.25_s);
		}
		m_next_widgets = std::async(std::launch::async, prepare_next_widgets, *m_selected, starting_side::LEFT);
	}};
	const action_callback next_acb{[this] {
		if (++m_selected == m_gamemodes.end()) {
			m_selected = m_gamemodes.begin();
		}
		m_substate = substate::SWITCHING_GAMEMODE;
		m_elapsed = 0;
		for (usize i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
			m_ui[GAMEMODE_WIDGETS[i]].move_x_and_hide(GAMEMODE_WIDGETS_BASE_X[i] - 250, 0.25_s);
		}
		m_next_widgets = std::async(std::launch::async, prepare_next_widgets, *m_selected, starting_side::RIGHT);
	}};
	const action_callback start_acb{[this] {
		m_substate = substate::STARTING_GAME;
		m_elapsed = 0;
		set_up_exit_animation();
		g_scorefile.last_selected = *m_selected;
		g_audio.fade_song_out(0.5s);
		m_next_state = make_game_state_async<active_game>(game_type::REGULAR, fade_in::YES, *m_selected);
	}};
	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING_TO_TITLE;
		m_elapsed = 0;
		set_up_exit_animation();
		g_scorefile.last_selected = *m_selected;
		m_next_state = make_async<title_state>(m_game);
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_NAME, NAME_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{std::string{m_selected->name_loc()}}, tr::sys::ttf_style::NORMAL, 120);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{TR_FMT::format("{}: {}", g_loc["by"], m_selected->author)}, tr::sys::ttf_style::NORMAL,
							   32);
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{m_selected->description_loc_with_fallback()}, tr::sys::ttf_style::ITALIC, 32, DARK_GRAY);
	m_ui.emplace<label_widget>(T_BEST_TIME_LABEL, best_time_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   loc_text_callback{T_BEST_TIME_LABEL}, tr::sys::ttf_style::NORMAL, 32, YELLOW);
	m_ui.emplace<label_widget>(T_BEST_TIME, BEST_TIME_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{format_time(g_scorefile.bests(*m_selected).time)}, tr::sys::ttf_style::NORMAL, 64,
							   YELLOW);
	m_ui.emplace<label_widget>(T_BEST_SCORE_LABEL, best_score_label_move_in, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   loc_text_callback{T_BEST_SCORE_LABEL}, tr::sys::ttf_style::NORMAL, 32, YELLOW);
	m_ui.emplace<label_widget>(T_BEST_SCORE, BEST_SCORE_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP,
							   const_text_callback{format_score(g_scorefile.bests(*m_selected).score)}, tr::sys::ttf_style::NORMAL, 64,
							   YELLOW);
	m_ui.emplace<arrow_widget>(T_PREV, PREV_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::LEFT, arrow_scb, prev_acb);
	m_ui.emplace<arrow_widget>(T_NEXT, NEXT_MOVE_IN, tr::valign::CENTER, 0.5_s, arrow_type::RIGHT, arrow_scb, next_acb);
	m_ui.emplace<text_button_widget>(T_START, START_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_START},
									 font::LANGUAGE, 48, scb, start_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

float start_game_state::fade_overlay_opacity()
{
	return m_substate == substate::STARTING_GAME ? m_elapsed / 0.5_sf : 0;
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

	m_ui[T_NAME].pos.move_y(300, 0.5_s);
	m_ui[T_AUTHOR].pos.move_x(600, 0.5_s);
	m_ui[T_DESCRIPTION].pos.move_x(400, 0.5_s);
	m_ui[T_BEST_TIME_LABEL].pos.move_y(label_h + 100, 0.5_s);
	m_ui[T_BEST_TIME].pos.move_y(725, 0.5_s);
	m_ui[T_BEST_SCORE_LABEL].pos.move_y(label_h + 100, 0.5_s);
	m_ui[T_BEST_SCORE].pos.move_y(725, 0.5_s);
	m_ui[T_TITLE].pos.move(TOP_START_POS, 0.5_s);
	m_ui[T_PREV].pos.move_x(-100, 0.5_s);
	m_ui[T_NEXT].pos.move_x(1100, 0.5_s);
	m_ui[T_START].pos.move(BOTTOM_START_POS, 0.5_s);
	m_ui[T_EXIT].pos.move(BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}