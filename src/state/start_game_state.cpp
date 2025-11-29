#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

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
	{"Enter"_kc, T_EXIT},
	{"2"_kc, T_EXIT},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> NAME_MOVE_IN{tween::CUBIC, {500, 300}, {500, 400}, 0.5_s};
constexpr tweener<glm::vec2> AUTHOR_MOVE_IN{tween::CUBIC, {400, 475}, {500, 475}, 0.5_s};
constexpr tweener<glm::vec2> DESCRIPTION_MOVE_IN{tween::CUBIC, {600, 525}, {500, 525}, 0.5_s};
constexpr tweener<glm::vec2> BEST_TIME_MOVE_IN{tween::CUBIC, {325, 725}, {325, 625}, 0.5_s};
constexpr tweener<glm::vec2> BEST_SCORE_MOVE_IN{tween::CUBIC, {675, 725}, {675, 625}, 0.5_s};
constexpr tweener<glm::vec2> PREV_MOVE_IN{tween::CUBIC, {-50, 500}, {10, 500}, 0.5_s};
constexpr tweener<glm::vec2> NEXT_MOVE_IN{tween::CUBIC, {1050, 500}, {990, 500}, 0.5_s};
constexpr tweener<glm::vec2> START_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

enum class starting_side : bool {
	LEFT,
	RIGHT
};

template <class... Args> void emplace_label_widget(std::unordered_map<tag, std::unique_ptr<widget>>& map, tag tag, Args&&... args)
{
	map.emplace(tag, std::make_unique<label_widget>(std::forward<Args>(args)...));
}

std::unordered_map<tag, std::unique_ptr<widget>> prepare_next_widgets(const gamemode& selected, starting_side side)
{
	using enum tr::align;
	using enum starting_side;

	// MOVE-INS

	const float label_h{621 - g_text_engine.line_skip(font::LANGUAGE, 32)};
	const tweener<glm::vec2> name_move_in{tween::CUBIC, {side == LEFT ? 250 : 750, 400}, {500, 400}, 0.25_s};
	const tweener<glm::vec2> author_move_in{tween::CUBIC, {side == LEFT ? 250 : 750, 475}, {500, 475}, 0.25_s};
	const tweener<glm::vec2> description_move_in{tween::CUBIC, {side == LEFT ? 250 : 750, 525}, {500, 525}, 0.25_s};
	const tweener<glm::vec2> best_time_label_move_in{tween::CUBIC, {side == LEFT ? 75 : 525, label_h}, {325, label_h}, 0.25_s};
	const tweener<glm::vec2> best_time_move_in{tween::CUBIC, {side == LEFT ? 75 : 525, 625}, {325, 625}, 0.25_s};
	const tweener<glm::vec2> best_score_label_move_in{tween::CUBIC, {side == LEFT ? 425 : 925, label_h}, {675, label_h}, 0.25_s};
	const tweener<glm::vec2> best_score_move_in{tween::CUBIC, {side == LEFT ? 425 : 925, 625}, {675, 625}, 0.25_s};

	// TEXT CALLBACKS

	string_text name_tcb{std::string{selected.name_loc()}};
	string_text author_tcb{TR_FMT::format("{}: {}", g_loc["by"], selected.author)};
	string_text description_tcb{
		std::string{!selected.description_loc().empty() ? selected.description_loc() : g_loc["no_description"]},
	};
	text_callback best_time_tcb{string_text{format_time(g_scorefile.bests(selected).time)}};
	text_callback best_score_tcb{string_text{format_score(g_scorefile.bests(selected).score)}};

	//

	std::unordered_map<tag, std::unique_ptr<widget>> map;
	emplace_label_widget(map, T_NAME, name_move_in, CENTER, 0.25_s, NO_TOOLTIP, std::move(name_tcb), text_style::NORMAL, 120);
	emplace_label_widget(map, T_AUTHOR, author_move_in, CENTER, 0.25_s, NO_TOOLTIP, std::move(author_tcb), text_style::NORMAL, 32);
	emplace_label_widget(map, T_DESCRIPTION, description_move_in, CENTER, 0.25_s, NO_TOOLTIP, std::move(description_tcb),
						 text_style::ITALIC, 32, "80808080"_rgba8);
	emplace_label_widget(map, T_BEST_TIME_LABEL, best_time_label_move_in, CENTER, 0.25_s, NO_TOOLTIP, tag_loc{T_BEST_TIME_LABEL},
						 text_style::NORMAL, 32, "FFFF00C0"_rgba8);
	emplace_label_widget(map, T_BEST_TIME, best_time_move_in, CENTER, 0.25_s, NO_TOOLTIP, std::move(best_time_tcb), text_style::NORMAL, 64,
						 "FFFF00C0"_rgba8);
	emplace_label_widget(map, T_BEST_SCORE_LABEL, best_score_label_move_in, CENTER, 0.25_s, NO_TOOLTIP, tag_loc{T_BEST_SCORE_LABEL},
						 text_style::NORMAL, 32, "FFFF00C0"_rgba8);
	emplace_label_widget(map, T_BEST_SCORE, best_score_move_in, CENTER, 0.25_s, NO_TOOLTIP, std::move(best_score_tcb), text_style::NORMAL,
						 64, "FFFF00C0"_rgba8);
	return map;
}

//

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
}

//

void start_game_state::set_up_ui()
{
	using enum tr::align;
	using enum text_style;

	// MOVE-INS

	const float label_h{621 - g_text_engine.line_skip(font::LANGUAGE, 32)};
	const tweener<glm::vec2> best_time_label_move_in{tween::CUBIC, {325, label_h + 100}, {325, label_h}, 0.5_s};
	const tweener<glm::vec2> best_score_label_move_in{tween::CUBIC, {675, label_h + 100}, {675, label_h}, 0.5_s};

	// TEXT CALLBACKS

	text_callback name_tcb{string_text{std::string{m_selected->name_loc()}}};
	text_callback author_tcb{string_text{TR_FMT::format("{}: {}", g_loc["by"], m_selected->author)}};
	text_callback description_tcb{
		string_text{std::string{!m_selected->description_loc().empty() ? m_selected->description_loc() : g_loc["no_description"]}}};
	text_callback best_time_tcb{string_text{format_time(g_scorefile.bests(*m_selected).time)}};
	text_callback best_score_tcb{string_text{format_score(g_scorefile.bests(*m_selected).score)}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_NAME, NAME_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, std::move(name_tcb), text_style::NORMAL, 120);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, std::move(author_tcb), text_style::NORMAL, 32);
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, std::move(description_tcb),
							   text_style::ITALIC, 32, "80808080"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_TIME_LABEL, best_time_label_move_in, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_BEST_TIME_LABEL},
							   text_style::NORMAL, 32, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_TIME, BEST_TIME_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, std::move(best_time_tcb), text_style::NORMAL, 64,
							   "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_SCORE_LABEL, best_score_label_move_in, CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_BEST_SCORE_LABEL},
							   text_style::NORMAL, 32, "FFFF00C0"_rgba8);
	m_ui.emplace<label_widget>(T_BEST_SCORE, BEST_SCORE_MOVE_IN, CENTER, 0.5_s, NO_TOOLTIP, std::move(best_score_tcb), text_style::NORMAL,
							   64, "FFFF00C0"_rgba8);
	m_ui.emplace<arrow_widget>(T_PREV, PREV_MOVE_IN, CENTER_LEFT, 0.5_s, false, arrow_interactible, on_prev);
	m_ui.emplace<arrow_widget>(T_NEXT, NEXT_MOVE_IN, CENTER_RIGHT, 0.5_s, true, arrow_interactible, on_next);
	m_ui.emplace<text_button_widget>(T_START, START_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_START}, font::LANGUAGE, 48,
									 interactible, on_start, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_EXIT}, font::LANGUAGE, 48,
									 interactible, on_exit, sound::CANCEL);
}

next_state start_game_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::ENTERING_START_GAME:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_timer = 0;
		}
		return tr::KEEP_STATE;
	case substate::IN_START_GAME:
		return tr::KEEP_STATE;
	case substate::SWITCHING_GAMEMODE:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_timer = 0;
		}
		else if (m_timer == 0.25_s) {
			m_ui.replace(m_next_widgets.get());
		}
	case substate::ENTERING_TITLE:
	case substate::ENTERING_GAME:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
	}
}

//

float start_game_state::fade_overlay_opacity()
{
	return m_substate == substate::ENTERING_GAME ? m_timer / 0.5_sf : 0;
}

void start_game_state::set_up_exit_animation()
{
	widget& name{m_ui[T_NAME]};
	widget& author{m_ui[T_AUTHOR]};
	widget& description{m_ui[T_DESCRIPTION]};
	widget& best_time_label{m_ui[T_BEST_TIME_LABEL]};
	widget& best_time{m_ui[T_BEST_TIME]};
	widget& best_score_label{m_ui[T_BEST_SCORE_LABEL]};
	widget& best_score{m_ui[T_BEST_SCORE]};
	name.pos.change(tween::CUBIC, glm::vec2{name.pos} - glm::vec2{0, 100}, 0.5_s);
	author.pos.change(tween::CUBIC, glm::vec2{author.pos} + glm::vec2{100, 0}, 0.5_s);
	description.pos.change(tween::CUBIC, glm::vec2{description.pos} - glm::vec2{100, 0}, 0.5_s);
	best_time_label.pos.change(tween::CUBIC, glm::vec2{best_time_label.pos} + glm::vec2{0, 100}, 0.5_s);
	best_time.pos.change(tween::CUBIC, glm::vec2{best_time.pos} + glm::vec2{0, 100}, 0.5_s);
	best_score_label.pos.change(tween::CUBIC, glm::vec2{best_score_label.pos} + glm::vec2{0, 100}, 0.5_s);
	best_score.pos.change(tween::CUBIC, glm::vec2{best_score.pos} + glm::vec2{0, 100}, 0.5_s);
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_PREV].pos.change(tween::CUBIC, {-100, 500}, 0.5_s);
	m_ui[T_NEXT].pos.change(tween::CUBIC, {1100, 500}, 0.5_s);
	m_ui[T_START].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui[T_EXIT].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all_widgets(0.5_s);
}

//

bool start_game_state::interactible()
{
	const start_game_state& self{g_state_machine.get<start_game_state>()};

	return self.m_substate == substate::IN_START_GAME || self.m_substate == substate::ENTERING_START_GAME;
}

bool start_game_state::arrow_interactible()
{
	const start_game_state& self{g_state_machine.get<start_game_state>()};

	return self.m_substate == substate::IN_START_GAME;
}

void start_game_state::on_prev()
{
	start_game_state& self{g_state_machine.get<start_game_state>()};

	self.m_selected = self.m_selected == self.m_gamemodes.begin() ? self.m_gamemodes.end() - 1 : std::prev(self.m_selected);
	self.m_substate = substate::SWITCHING_GAMEMODE;
	self.m_timer = 0;
	for (tag tag : GAMEMODE_WIDGETS) {
		widget& widget{self.m_ui[tag]};
		widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} + glm::vec2{250, 0}, 0.25_s);
		widget.hide(0.25_s);
	}
	self.m_next_widgets = std::async(std::launch::async, prepare_next_widgets, *self.m_selected, starting_side::LEFT);
}

void start_game_state::on_next()
{
	start_game_state& self{g_state_machine.get<start_game_state>()};

	if (++self.m_selected == self.m_gamemodes.end()) {
		self.m_selected = self.m_gamemodes.begin();
	}
	self.m_substate = substate::SWITCHING_GAMEMODE;
	self.m_timer = 0;
	for (tag tag : GAMEMODE_WIDGETS) {
		widget& widget{self.m_ui[tag]};
		widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} - glm::vec2{250, 0}, 0.25_s);
		widget.hide(0.25_s);
	}
	self.m_next_widgets = std::async(std::launch::async, prepare_next_widgets, *self.m_selected, starting_side::RIGHT);
}

void start_game_state::on_start()
{
	start_game_state& self{g_state_machine.get<start_game_state>()};

	self.m_substate = substate::ENTERING_GAME;
	self.m_timer = 0;
	self.set_up_exit_animation();
	g_scorefile.last_selected = *self.m_selected;
	g_audio.fade_song_out(0.5s);
	prepare_next_game_state<active_game>(game_type::REGULAR, true, *self.m_selected);
}

void start_game_state::on_exit()
{
	start_game_state& self{g_state_machine.get<start_game_state>()};

	self.m_substate = substate::ENTERING_TITLE;
	self.m_timer = 0;
	self.set_up_exit_animation();
	g_scorefile.last_selected = *self.m_selected;
	prepare_next_state<title_state>(self.m_game);
}