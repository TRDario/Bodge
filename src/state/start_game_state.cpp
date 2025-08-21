#include "../../include/state/start_game_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr tag T_TITLE{"start_game"};
constexpr tag T_NAME{"name"};
constexpr tag T_AUTHOR{"author"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_PB{"pb"};
constexpr tag T_PREV{"prev"};
constexpr tag T_NEXT{"next"};
constexpr tag T_START{"start"};
constexpr tag T_EXIT{"exit"};

// Gamemode display widgets.
constexpr std::array<tag, 4> GAMEMODE_WIDGETS{T_NAME, T_AUTHOR, T_DESCRIPTION, T_PB};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_START},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::LEFT}, T_PREV},
	{{tr::system::keycode::RIGHT}, T_NEXT},
	{{tr::system::keycode::ENTER}, T_START},
	{{tr::system::keycode::TOP_ROW_1}, T_START},
	{{tr::system::keycode::ESCAPE}, T_EXIT},
	{{tr::system::keycode::TOP_ROW_2}, T_EXIT},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> NAME_MOVE_IN{tween::CUBIC, {500, 275}, {500, 375}, 0.5_s};
constexpr tweener<glm::vec2> AUTHOR_MOVE_IN{tween::CUBIC, {400, 450}, {500, 450}, 0.5_s};
constexpr tweener<glm::vec2> DESCRIPTION_MOVE_IN{tween::CUBIC, {600, 500}, {500, 500}, 0.5_s};
constexpr tweener<glm::vec2> PB_MOVE_IN{tween::CUBIC, {500, 695}, {500, 595}, 0.5_s};
constexpr tweener<glm::vec2> PREV_MOVE_IN{tween::CUBIC, {-50, 500}, {10, 500}, 0.5_s};
constexpr tweener<glm::vec2> NEXT_MOVE_IN{tween::CUBIC, {1050, 500}, {990, 500}, 0.5_s};
constexpr tweener<glm::vec2> START_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

start_game_state::start_game_state(std::unique_ptr<game>&& game)
	: menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::ENTERING_START_GAME}
	, m_gamemodes{load_gamemodes()}
	, m_selected{m_gamemodes.begin()}
{
	std::vector<gamemode>::iterator last_selected_it{std::ranges::find(m_gamemodes, engine::scorefile.last_selected)};
	if (last_selected_it != m_gamemodes.end()) {
		m_selected = last_selected_it;
	}

	// TEXT CALLBACKS

	text_callback name_tcb{string_text_callback{std::string{name(*m_selected)}}};
	text_callback author_tcb{string_text_callback{std::format("{}: {}", engine::loc["by"], m_selected->author)}};
	text_callback description_tcb{string_text_callback{std::string{description(*m_selected)}}};
	text_callback pb_tcb{string_text_callback{std::format("{}:\n{}", engine::loc["pb"], timer_text(pb(engine::scorefile, *m_selected)))}};

	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_START_GAME || m_substate == substate::ENTERING_START_GAME; },
	};
	const status_callback arrow_scb{
		[this] { return m_substate == substate::IN_START_GAME; },
	};

	// ACTION CALLBACKS

	const action_callback prev_acb{
		[this] {
			m_selected = m_selected == m_gamemodes.begin() ? m_selected = m_gamemodes.end() - 1 : std::prev(m_selected);
			m_substate = substate::SWITCHING_GAMEMODE;
			m_timer = 0;
			for (tag tag : GAMEMODE_WIDGETS) {
				widget& widget{m_ui[tag]};
				widget.pos.change(tween::CUBIC, {750, glm::vec2{widget.pos}.y}, 0.25_s);
				widget.hide(0.25_s);
			}
		},
	};
	const action_callback next_acb{
		[this] {
			if (++m_selected == m_gamemodes.end()) {
				m_selected = m_gamemodes.begin();
			}
			m_substate = substate::SWITCHING_GAMEMODE;
			m_timer = 0;
			for (tag tag : GAMEMODE_WIDGETS) {
				widget& widget{m_ui[tag]};
				widget.pos.change(tween::CUBIC, {250, glm::vec2{widget.pos}.y}, 0.25_s);
				widget.hide(0.25_s);
			}
		},
	};
	const action_callback start_acb{
		[this] {
			m_substate = substate::ENTERING_GAME;
			m_timer = 0;
			set_up_exit_animation();
			engine::scorefile.last_selected = *m_selected;
			engine::fade_song_out(0.5s);
		},
	};
	const action_callback exit_acb{
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			set_up_exit_animation();
			engine::scorefile.last_selected = *m_selected;
		},
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_NAME, NAME_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(name_tcb),
							   tr::system::ttf_style::NORMAL, 120);
	m_ui.emplace<label_widget>(T_AUTHOR, AUTHOR_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(author_tcb),
							   tr::system::ttf_style::NORMAL, 32);
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(description_tcb),
							   tr::system::ttf_style::ITALIC, 32, "80808080"_rgba8);
	m_ui.emplace<label_widget>(T_PB, PB_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, std::move(pb_tcb), tr::system::ttf_style::NORMAL, 48,
							   "FFFF00C0"_rgba8);
	m_ui.emplace<arrow_widget>(T_PREV, PREV_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, arrow_scb, prev_acb);
	m_ui.emplace<arrow_widget>(T_NEXT, NEXT_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, arrow_scb, next_acb);
	m_ui.emplace<text_button_widget>(T_START, START_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_START},
									 font::LANGUAGE, 48, scb, start_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

//

float start_game_state::fade_overlay_opacity()
{
	return m_substate == substate::ENTERING_GAME ? m_timer / 0.5_sf : 0;
}

std::unique_ptr<tr::state> start_game_state::update(tr::duration)
{
	menu_state::update({});
	switch (m_substate) {
	case substate::ENTERING_START_GAME:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_timer = 0;
		}
		return nullptr;
	case substate::IN_START_GAME:
		return nullptr;
	case substate::SWITCHING_GAMEMODE:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_timer = 0;
		}
		else if (m_timer == 0.25_s) {
			std::array<text_callback, GAMEMODE_WIDGETS.size()> new_cbs{
				string_text_callback{std::string{::name(*m_selected)}},
				string_text_callback{std::format("{}: {}", engine::loc["by"], m_selected->author)},
				string_text_callback{std::string{description(*m_selected)}},
				string_text_callback{std::format("{}:\n{}", engine::loc["pb"], timer_text(pb(engine::scorefile, *m_selected)))},
			};
			for (std::size_t i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
				text_widget& widget{m_ui.as<text_widget>(GAMEMODE_WIDGETS[i])};
				const glm::vec2 old_pos{widget.pos};
				widget.text_cb = std::move(new_cbs[i]);
				widget.pos = glm::vec2{old_pos.x < 500 ? 600 : 400, old_pos.y};
				widget.pos.change(tween::CUBIC, {500, old_pos.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? std::make_unique<title_state>(release_game()) : nullptr;
	case substate::ENTERING_GAME:
		return m_timer >= 0.5_s ? std::make_unique<game_state>(std::make_unique<active_game>(*m_selected), game_type::REGULAR, true)
								: nullptr;
	}
}

//

void start_game_state::set_up_exit_animation()
{
	widget& name{m_ui[T_NAME]};
	widget& author{m_ui[T_AUTHOR]};
	widget& description{m_ui[T_DESCRIPTION]};
	widget& pb{m_ui[T_PB]};
	name.pos.change(tween::CUBIC, glm::vec2{name.pos} - glm::vec2{0, 100}, 0.5_s);
	author.pos.change(tween::CUBIC, glm::vec2{author.pos} + glm::vec2{100, 0}, 0.5_s);
	description.pos.change(tween::CUBIC, glm::vec2{description.pos} - glm::vec2{100, 0}, 0.5_s);
	pb.pos.change(tween::CUBIC, glm::vec2{pb.pos} + glm::vec2{0, 100}, 0.5_s);
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_PREV].pos.change(tween::CUBIC, {-100, 500}, 0.5_s);
	m_ui[T_NEXT].pos.change(tween::CUBIC, {1100, 500}, 0.5_s);
	m_ui[T_START].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui[T_EXIT].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all(0.5_s);
}