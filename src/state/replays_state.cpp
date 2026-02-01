#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

// Number of replays displayed per page.
constexpr usize REPLAYS_PER_PAGE{6};

constexpr tag T_TITLE{"replays"};
constexpr tag T_NO_REPLAYS_FOUND{"no_replays_found"};
constexpr tag T_REPLAY_0{"replay0"};
constexpr tag T_REPLAY_1{"replay1"};
constexpr tag T_REPLAY_2{"replay2"};
constexpr tag T_REPLAY_3{"replay3"};
constexpr tag T_REPLAY_4{"replay4"};
constexpr tag T_REPLAY_5{"replay5"};
constexpr tag T_PAGE_D{"page_d"};
constexpr tag T_PAGE_C{"page_c"};
constexpr tag T_PAGE_I{"page_i"};
constexpr tag T_EXIT{"exit"};

// Replay widget tags.
constexpr std::array<tag, REPLAYS_PER_PAGE> REPLAY_TAGS{T_REPLAY_0, T_REPLAY_1, T_REPLAY_2, T_REPLAY_3, T_REPLAY_4, T_REPLAY_5};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_REPLAY_0},
	selection_tree_row{T_REPLAY_1},
	selection_tree_row{T_REPLAY_2},
	selection_tree_row{T_REPLAY_3},
	selection_tree_row{T_REPLAY_4},
	selection_tree_row{T_REPLAY_5},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{"1"_kc, T_REPLAY_0},
	{"2"_kc, T_REPLAY_1},
	{"3"_kc, T_REPLAY_2},
	{"4"_kc, T_REPLAY_3},
	{"5"_kc, T_REPLAY_4},
	{"Escape"_kc, T_EXIT},
	{"Left"_kc, T_PAGE_D},
	{"Right"_kc, T_PAGE_I},
};

constexpr tweened_position TITLE_MOVE_IN{TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweened_position NO_REPLAYS_FOUND_MOVE_IN{{600, 467}, {500, 467}, 0.5_s};
constexpr tweened_position PAGE_D_MOVE_IN{{-50, 942.5}, {10, 942.5}, 0.5_s};
constexpr tweened_position PAGE_C_MOVE_IN{BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweened_position PAGE_I_MOVE_IN{{1050, 942.5}, {990, 942.5}, 0.5_s};
constexpr tweened_position EXIT_MOVE_IN{BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on
////////////////////////////////////////////////////////////// REPLAYS STATE //////////////////////////////////////////////////////////////

replays_state::replays_state()
	: main_menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::RETURNING_FROM_REPLAY}, m_page{0}, m_replays{load_replay_headers()}
{
	set_up_ui();
}

replays_state::replays_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_REPLAYS}
	, m_page{0}
	, m_replays{load_replay_headers()}
{
	set_up_ui();
}

//

tr::next_state replays_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::RETURNING_FROM_REPLAY:
		if (m_elapsed == 1) {
			g_audio.play_song("menu", SKIP_MENU_SONG_INTRO_TIMESTAMP, 1.0s);
		}
		else if (m_elapsed >= 0.5_s) {
			m_elapsed = 0;
			m_substate = substate::IN_REPLAYS;
		}
		return tr::KEEP_STATE;
	case substate::IN_REPLAYS:
		return tr::KEEP_STATE;
	case substate::SWITCHING_PAGE:
		if (m_elapsed >= 0.5_s) {
			m_elapsed = 0;
			m_substate = substate::IN_REPLAYS;
		}
		else if (m_elapsed == 0.25_s) {
			m_ui.replace(m_next_widgets.get());
		}
		return tr::KEEP_STATE;
	case substate::STARTING_REPLAY:
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

//

float replays_state::fade_overlay_opacity()
{
	switch (m_substate) {
	case substate::RETURNING_FROM_REPLAY:
		return 1 - m_elapsed / 0.5_sf;
	case substate::IN_REPLAYS:
	case substate::SWITCHING_PAGE:
	case substate::EXITING:
		return 0;
	case substate::STARTING_REPLAY:
		return m_elapsed / 0.5_sf;
	}
}

std::unordered_map<tag, std::unique_ptr<widget>> replays_state::prepare_next_widgets()
{
	std::unordered_map<tag, std::unique_ptr<widget>> map;
	replay_map::const_iterator it{std::next(m_replays.begin(), REPLAYS_PER_PAGE * m_page)};
	for (usize i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const std::optional<replay_map::const_iterator> opt_it{it != m_replays.end() ? std::optional{it++} : std::nullopt};
		const tweened_position move_in{{i % 2 == 0 ? 600 : 400, 183 + 125 * i}, {500, 183 + 125 * i}, 0.25_s};
		map.emplace(REPLAY_TAGS[i], std::make_unique<replay_widget>(move_in, tr::align::CENTER, 0.25_s, *this, opt_it));
	}
	return map;
}

void replays_state::set_up_ui()
{
	// STATUS CALLBACKS

	const status_callback scb{[this] { return m_substate == substate::IN_REPLAYS; }};
	const status_callback page_d_scb{[this] { return m_substate == substate::IN_REPLAYS && m_page > 0; }};
	const status_callback page_i_scb{[this] {
		const u16 last_page{u16(std::max(m_replays.size() - 1, 0_uz) / REPLAYS_PER_PAGE)};
		return m_substate == substate::IN_REPLAYS && m_page < last_page;
	}};

	// ACTION CALLBACKS

	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING;
		m_elapsed = 0;
		set_up_exit_animation();
		m_next_state = make_async<title_state>(m_game);
	}};
	const action_callback page_d_acb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_elapsed = 0;
		--m_page;
		set_up_page_switch_animation();
	}};
	const action_callback page_i_acb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_elapsed = 0;
		++m_page;
		set_up_page_switch_animation();
	}};

	// TEXT CALLBACKS

	const text_callback page_c_tcb{[this] {
		const usize total_pages{std::max(m_replays.size() - 1, 0_uz) / REPLAYS_PER_PAGE + 1};
		return TR_FMT::format("{}/{}", m_page + 1, total_pages);
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	if (m_replays.empty()) {
		m_ui.emplace<label_widget>(T_NO_REPLAYS_FOUND, NO_REPLAYS_FOUND_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP,
								   loc_text_callback{T_NO_REPLAYS_FOUND}, tr::sys::ttf_style::NORMAL, 64, DARK_GRAY);
		return;
	}
	replay_map::iterator it{m_replays.begin()};
	for (usize i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const std::optional<replay_map::iterator> opt_it{it != m_replays.end() ? std::optional{it++} : std::nullopt};
		const tweened_position move_in{{i % 2 == 0 ? 400 : 600, 183 + 125 * i}, {500, 183 + 125 * i}, 0.5_s};
		m_ui.emplace<replay_widget>(REPLAY_TAGS[i], move_in, tr::align::CENTER, 0.5_s, *this, opt_it);
	}
	m_ui.emplace<arrow_widget>(T_PAGE_D, PAGE_D_MOVE_IN, tr::valign::BOTTOM, 0.5_s, arrow_type::LEFT, page_d_scb, page_d_acb);
	m_ui.emplace<label_widget>(T_PAGE_C, PAGE_C_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, page_c_tcb,
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_PAGE_I, PAGE_I_MOVE_IN, tr::valign::BOTTOM, 0.5_s, arrow_type::RIGHT, page_i_scb, page_i_acb);
}

void replays_state::set_up_page_switch_animation()
{
	for (usize i = 0; i < REPLAYS_PER_PAGE; i++) {
		m_ui[REPLAY_TAGS[i]].move_x_and_hide(i % 2 == 0 ? 600 : 400, 0.25_s);
	}
	m_next_widgets = std::async(std::launch::async, &replays_state::prepare_next_widgets, this);
}

void replays_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.move(TOP_START_POS, 0.5_s);
	m_ui[T_EXIT].pos.move(BOTTOM_START_POS, 0.5_s);
	if (m_replays.empty()) {
		m_ui[T_NO_REPLAYS_FOUND].pos.move_x(400, 0.5_s);
	}
	else {
		for (usize i = 0; i < REPLAYS_PER_PAGE; i++) {
			m_ui[REPLAY_TAGS[i]].pos.move_x(i % 2 == 0 ? 600 : 400, 0.5_s);
		}
		m_ui[T_PAGE_C].pos.move(BOTTOM_START_POS, 0.5_s);
		m_ui[T_PAGE_D].pos.move_x(-50, 0.5_s);
		m_ui[T_PAGE_I].pos.move_x(1050, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
}