///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements replays_state from state.hpp.                                                                                              //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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
constexpr std::array REPLAY_TAGS{T_REPLAY_0, T_REPLAY_1, T_REPLAY_2, T_REPLAY_3, T_REPLAY_4, T_REPLAY_5};

// Selection tree used for the replays menu.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_REPLAY_0},
	selection_tree_row{T_REPLAY_1},
	selection_tree_row{T_REPLAY_2},
	selection_tree_row{T_REPLAY_3},
	selection_tree_row{T_REPLAY_4},
	selection_tree_row{T_REPLAY_5},
	selection_tree_row{T_EXIT},
};

// Shortcut table used for the replays menu.
constexpr shortcut_table SHORTCUTS{
	{"1"_kc, T_REPLAY_0},
	{"2"_kc, T_REPLAY_1},
	{"3"_kc, T_REPLAY_2},
	{"4"_kc, T_REPLAY_3},
	{"5"_kc, T_REPLAY_4},
	{"Left"_kc, T_PAGE_D},
	{"Right"_kc, T_PAGE_I},
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT},
};

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

bool replays_state::transparent_cursor() const
{
	return m_substate == substate::STARTING_REPLAY;
}

tr::next_state replays_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::RETURNING_FROM_REPLAY:
		if (m_elapsed == 1) {
			audio::instance().play_song("menu", SKIP_MENU_SONG_INTRO_TIMESTAMP, 1.0s);
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
	replay_map::const_iterator replay_it{std::next(m_replays.begin(), REPLAYS_PER_PAGE * m_page)};
	for (usize i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const std::optional<replay_map::const_iterator> opt_it{replay_it != m_replays.end() ? std::optional{replay_it++} : std::nullopt};
		const tweened_position animation{{i % 2 == 0 ? 600 : 400, 183 + 125 * i}, {500, 183 + 125 * i}, 0.25_s};
		// clang-format off
		map.emplace(REPLAY_TAGS[i], std::make_unique<replay_widget>(replay_widget::properties{
			.animation = animation,
			.unhide_time = 0.25_s,
			.state = *this,
			.replay_it = opt_it
		}));
		// clang-format on
	}
	return map;
}

void replays_state::set_up_ui()
{
	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = {TOP_START_POS, TITLE_POS, 0.5_s},
		.alignment = tr::align::TOP_CENTER,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<text_button_widget>(T_EXIT, {
		.animation = {BOTTOM_START_POS, {500, 1000}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.text = localized_text{T_EXIT},
		.status = [this] { return m_substate == substate::IN_REPLAYS; },
		.action = [this] { on_exit(); },
		.action_sound = sound::CANCEL
	});
	if (m_replays.empty()) {
		m_ui.emplace<label_widget>(T_NO_REPLAYS_FOUND, {
			.animation = {{600, 467}, {500, 467}, 0.5_s},
			.alignment = tr::align::TOP_CENTER,
			.text = localized_text{T_NO_REPLAYS_FOUND},
			.font_size = 64,
			.color = DARK_GRAY
		});
		return;
	}
	replay_map::iterator replay_it{m_replays.begin()};
	for (usize i = 0; i < REPLAYS_PER_PAGE; ++i) {
		m_ui.emplace<replay_widget>(REPLAY_TAGS[i], {
			.animation = {{i % 2 == 0 ? 400 : 600, 183 + 125 * i}, {500, 183 + 125 * i}, 0.5_s},
			.state = *this,
			.replay_it = replay_it != m_replays.end() ? std::optional{replay_it++} : std::nullopt
		});
	}
	m_ui.emplace<arrow_widget>(T_PAGE_D, {
		.animation = {{-50, 942.5}, {10, 942.5}, 0.5_s},
		.alignment = tr::valign::BOTTOM,
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_REPLAYS && m_page > 0; },
		.action = [this] { on_page_decrement(); }
	});
	m_ui.emplace<label_widget>(T_PAGE_C, {
		.animation = {BOTTOM_START_POS, {500, 950}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.text = [this] {
			const usize total_pages{std::max(m_replays.size() - 1, 0_uz) / REPLAYS_PER_PAGE + 1};
			return TR_FMT::format("{}/{}", m_page + 1, total_pages);
		}
	});
	m_ui.emplace<arrow_widget>(T_PAGE_I, {
		.animation = {{1050, 942.5}, {990, 942.5}, 0.5_s},
		.alignment = tr::valign::BOTTOM,
		.type = arrow_type::RIGHT,
		.status = [this] {
			const u16 last_page{u16(std::max(m_replays.size() - 1, 0_uz) / REPLAYS_PER_PAGE)};
			return m_substate == substate::IN_REPLAYS && m_page < last_page;
		},
		.action = [this] { on_page_increment(); }
	});
	// clang-format on
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
	m_ui[T_TITLE].move_and_hide(TOP_START_POS, 0.5_s);
	m_ui[T_EXIT].move_and_hide(BOTTOM_START_POS, 0.5_s);
	if (m_replays.empty()) {
		m_ui[T_NO_REPLAYS_FOUND].move_x_and_hide(400, 0.5_s);
	}
	else {
		for (usize i = 0; i < REPLAYS_PER_PAGE; i++) {
			m_ui[REPLAY_TAGS[i]].move_x_and_hide(i % 2 == 0 ? 600 : 400, 0.5_s);
		}
		m_ui[T_PAGE_C].move_and_hide(BOTTOM_START_POS, 0.5_s);
		m_ui[T_PAGE_D].move_x_and_hide(-50, 0.5_s);
		m_ui[T_PAGE_I].move_x_and_hide(1050, 0.5_s);
	}
}

//

void replays_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation();
	m_next_state = make_async<title_state>(m_game);
}

void replays_state::on_page_decrement()
{
	m_substate = substate::SWITCHING_PAGE;
	m_elapsed = 0;
	--m_page;
	set_up_page_switch_animation();
}

void replays_state::on_page_increment()
{
	m_substate = substate::SWITCHING_PAGE;
	m_elapsed = 0;
	++m_page;
	set_up_page_switch_animation();
}