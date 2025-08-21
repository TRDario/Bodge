#include "../../include/state/replays_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr std::size_t REPLAYS_PER_PAGE{5};

constexpr tag T_TITLE{"replays"};
constexpr tag T_NO_REPLAYS_FOUND{"no_replays_found"};
constexpr tag T_REPLAY_0{"replay0"};
constexpr tag T_REPLAY_1{"replay1"};
constexpr tag T_REPLAY_2{"replay2"};
constexpr tag T_REPLAY_3{"replay3"};
constexpr tag T_REPLAY_4{"replay4"};
constexpr tag T_PAGE_D{"page_d"};
constexpr tag T_PAGE_C{"page_c"};
constexpr tag T_PAGE_I{"page_i"};
constexpr tag T_EXIT{"exit"};

constexpr std::array<tag, REPLAYS_PER_PAGE> REPLAY_TAGS{T_REPLAY_0, T_REPLAY_1, T_REPLAY_2, T_REPLAY_3, T_REPLAY_4};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_REPLAY_0},
	selection_tree_row{T_REPLAY_1},
	selection_tree_row{T_REPLAY_2},
	selection_tree_row{T_REPLAY_3},
	selection_tree_row{T_REPLAY_4},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::TOP_ROW_1}, T_REPLAY_0},
	{{tr::system::keycode::TOP_ROW_2}, T_REPLAY_1},
	{{tr::system::keycode::TOP_ROW_3}, T_REPLAY_2},
	{{tr::system::keycode::TOP_ROW_4}, T_REPLAY_3},
	{{tr::system::keycode::TOP_ROW_5}, T_REPLAY_4},
	{{tr::system::keycode::ESCAPE}, T_EXIT},
	{{tr::system::keycode::LEFT}, T_PAGE_D},
	{{tr::system::keycode::RIGHT}, T_PAGE_I},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> NO_REPLAYS_FOUND_MOVE_IN{tween::CUBIC, {600, 467}, {500, 467}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_D_MOVE_IN{tween::CUBIC, {-50, 942.5}, {10, 942.5}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_C_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_I_MOVE_IN{tween::CUBIC, {1050, 942.5}, {990, 942.5}, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

replays_state::replays_state()
	: menu_state{SELECTION_TREE, SHORTCUTS}
	, m_substate{substate::RETURNING_FROM_REPLAY}
	, m_page{0}
	, m_replays{engine::load_replay_headers()}
{
	set_up_ui();
	engine::play_song("menu", SKIP_MENU_SONG_INTRO, 0.5s);
}

replays_state::replays_state(std::unique_ptr<game>&& game)
	: menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_REPLAYS}
	, m_page{0}
	, m_replays{engine::load_replay_headers()}
{
	set_up_ui();
}

//

std::unique_ptr<tr::state> replays_state::update(tr::duration)
{
	menu_state::update({});
	switch (m_substate) {
	case substate::RETURNING_FROM_REPLAY:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate::IN_REPLAYS;
		}
		return nullptr;
	case substate::IN_REPLAYS:
		return nullptr;
	case substate::SWITCHING_PAGE:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate::IN_REPLAYS;
		}
		else if (m_timer == 0.25_s) {
			std::map<std::string, replay_header>::iterator it{std::next(m_replays.begin(), REPLAYS_PER_PAGE * m_page)};
			for (std::size_t i = 0; i < REPLAYS_PER_PAGE; ++i) {
				replay_widget& widget{m_ui.as<replay_widget>(REPLAY_TAGS[i])};
				widget.it = it != m_replays.end() ? std::optional{it++} : std::nullopt;
				widget.pos = {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y};
				widget.pos.change(tween::CUBIC, {500, glm::vec2{widget.pos}.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
		return nullptr;
	case substate::STARTING_REPLAY:
		return m_timer >= 0.5_s
				   ? std::make_unique<game_state>(std::make_unique<replay_game>(m_selected->second.gamemode, replay{m_selected->first}),
												  game_type::REPLAY, true)
				   : nullptr;
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? std::make_unique<title_state>(release_game()) : nullptr;
	}
}

//

float replays_state::fade_overlay_opacity()
{
	switch (m_substate) {
	case substate::RETURNING_FROM_REPLAY:
		return 1 - m_timer / 0.5_sf;
	case substate::IN_REPLAYS:
	case substate::SWITCHING_PAGE:
	case substate::ENTERING_TITLE:
		return 0;
	case substate::STARTING_REPLAY:
		return m_timer / 0.5_sf;
	}
}

void replays_state::set_up_ui()
{
	// STATUS CALLBACKS

	const status_callback scb{
		[this] { return m_substate == substate::IN_REPLAYS; },
	};
	const status_callback page_d_scb{
		[this] { return m_substate == substate::IN_REPLAYS && m_page > 0; },
	};
	const status_callback page_i_scb{
		[this] {
			return m_substate == substate::IN_REPLAYS && m_page < (std::max(m_replays.size() - 1, std::size_t{0}) / REPLAYS_PER_PAGE);
		},
	};

	// ACTION CALLBACKS

	const auto replay_acb{
		[this](std::map<std::string, replay_header>::iterator it) {
			m_substate = substate::STARTING_REPLAY;
			m_timer = 0;
			m_selected = it;
			set_up_exit_animation();
			engine::fade_song_out(0.5s);
		},
	};
	const action_callback exit_acb{
		[this] {
			m_substate = substate::ENTERING_TITLE;
			m_timer = 0;
			set_up_exit_animation();
		},
	};
	const action_callback page_d_acb{
		[this] {
			m_substate = substate::SWITCHING_PAGE;
			m_timer = 0;
			--m_page;
			set_up_page_switch_animation();
		},
	};
	const action_callback page_i_acb{
		[this] {
			m_substate = substate::SWITCHING_PAGE;
			m_timer = 0;
			++m_page;
			set_up_page_switch_animation();
		},
	};

	// TEXT CALLBACKS

	const text_callback page_c_tcb{
		[this] { return std::format("{}/{}", m_page + 1, std::max(m_replays.size() - 1, std::size_t{0}) / REPLAYS_PER_PAGE + 1); },
	};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	if (m_replays.empty()) {
		m_ui.emplace<label_widget>(T_NO_REPLAYS_FOUND, NO_REPLAYS_FOUND_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP,
								   loc_text_callback{T_NO_REPLAYS_FOUND}, tr::system::ttf_style::NORMAL, 64, "80808080"_rgba8);
		return;
	}
	std::map<std::string, replay_header>::iterator it{m_replays.begin()};
	for (std::size_t i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const std::optional<std::map<std::string, replay_header>::iterator> opt_it{it != m_replays.end() ? std::optional{it++}
																										 : std::nullopt};
		const tweener<glm::vec2> move_in{tween::CUBIC, {i % 2 == 0 ? 400 : 600, 183 + 150 * i}, {500, 183 + 150 * i}, 0.5_s};
		m_ui.emplace<replay_widget>(REPLAY_TAGS[i], move_in, tr::align::CENTER, 0.5_s, scb, replay_acb, opt_it);
	}
	m_ui.emplace<arrow_widget>(T_PAGE_D, PAGE_D_MOVE_IN, tr::align::BOTTOM_LEFT, 0.5_s, false, page_d_scb, page_d_acb);
	m_ui.emplace<label_widget>(T_PAGE_C, PAGE_C_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, page_c_tcb,
							   tr::system::ttf_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_PAGE_I, PAGE_I_MOVE_IN, tr::align::BOTTOM_RIGHT, 0.5_s, true, page_i_scb, page_i_acb);
}

void replays_state::set_up_page_switch_animation()
{
	for (std::size_t i = 0; i < REPLAYS_PER_PAGE; i++) {
		widget& widget{m_ui[REPLAY_TAGS[i]]};
		widget.pos.change(tween::CUBIC, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
}

void replays_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_EXIT].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	if (m_replays.empty()) {
		m_ui[T_NO_REPLAYS_FOUND].pos.change(tween::CUBIC, {400, 467}, 0.5_s);
	}
	else {
		for (std::size_t i = 0; i < REPLAYS_PER_PAGE; i++) {
			widget& widget{m_ui[REPLAY_TAGS[i]]};
			widget.pos.change(tween::CUBIC, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.5_s);
		}
		m_ui[T_PAGE_C].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
		m_ui[T_PAGE_D].pos.change(tween::CUBIC, {-50, 942.5}, 0.5_s);
		m_ui[T_PAGE_I].pos.change(tween::CUBIC, {1050, 942.5}, 0.5_s);
	}
	m_ui.hide_all(0.5_s);
}