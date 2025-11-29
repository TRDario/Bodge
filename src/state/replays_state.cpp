#include "../../include/state/state.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr usize REPLAYS_PER_PAGE{6};

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
	{"1"_kc, T_REPLAY_0},
	{"2"_kc, T_REPLAY_1},
	{"3"_kc, T_REPLAY_2},
	{"4"_kc, T_REPLAY_3},
	{"5"_kc, T_REPLAY_4},
	{"Enter"_kc, T_EXIT},
	{"Left"_kc, T_PAGE_D},
	{"Right"_kc, T_PAGE_I},
};

constexpr tweener<glm::vec2> TITLE_MOVE_IN{tween::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr tweener<glm::vec2> NO_REPLAYS_FOUND_MOVE_IN{tween::CUBIC, {600, 467}, {500, 467}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_D_MOVE_IN{tween::CUBIC, {-50, 942.5}, {10, 942.5}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_C_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr tweener<glm::vec2> PAGE_I_MOVE_IN{tween::CUBIC, {1050, 942.5}, {990, 942.5}, 0.5_s};
constexpr tweener<glm::vec2> EXIT_MOVE_IN{tween::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on

replays_state::replays_state()
	: main_menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::RETURNING_FROM_REPLAY}, m_page{0}, m_replays{load_replay_headers()}
{
}

replays_state::replays_state(std::shared_ptr<playerless_game> game)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_REPLAYS}
	, m_page{0}
	, m_replays{load_replay_headers()}
{
}

//

void replays_state::set_up_ui()
{
	using enum tr::align;

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_TITLE}, text_style::NORMAL, 64);
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_EXIT}, font::LANGUAGE, 48,
									 interactible, on_exit, sound::CANCEL);
	if (m_replays.empty()) {
		m_ui.emplace<label_widget>(T_NO_REPLAYS_FOUND, NO_REPLAYS_FOUND_MOVE_IN, TOP_CENTER, 0.5_s, NO_TOOLTIP, tag_loc{T_NO_REPLAYS_FOUND},
								   text_style::NORMAL, 64, "80808080"_rgba8);
		return;
	}
	std::map<std::string, replay_header>::iterator it{m_replays.begin()};
	for (usize i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const std::optional<std::map<std::string, replay_header>::iterator> opt_it{it != m_replays.end() ? std::optional{it++}
																										 : std::nullopt};
		const tweener<glm::vec2> move_in{tween::CUBIC, {i % 2 == 0 ? 400 : 600, 183 + 125 * i}, {500, 183 + 125 * i}, 0.5_s};
		m_ui.emplace<replay_widget>(REPLAY_TAGS[i], move_in, CENTER, 0.5_s, interactible, on_replay, opt_it);
	}
	m_ui.emplace<arrow_widget>(T_PAGE_D, PAGE_D_MOVE_IN, BOTTOM_LEFT, 0.5_s, false, page_d_interactible, on_page_d);
	m_ui.emplace<label_widget>(T_PAGE_C, PAGE_C_MOVE_IN, BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, page_c_text, text_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_PAGE_I, PAGE_I_MOVE_IN, BOTTOM_RIGHT, 0.5_s, true, page_i_interactible, on_page_i);
}

next_state replays_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::RETURNING_FROM_REPLAY:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate::IN_REPLAYS;
		}
		return tr::KEEP_STATE;
	case substate::IN_REPLAYS:
		return tr::KEEP_STATE;
	case substate::SWITCHING_PAGE:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate::IN_REPLAYS;
		}
		else if (m_timer == 0.25_s) {
			m_ui.replace(m_next_widgets.get());
		}
		return tr::KEEP_STATE;
	case substate::STARTING_REPLAY:
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? g_next_state.get() : tr::KEEP_STATE;
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

std::unordered_map<tag, std::unique_ptr<widget>> replays_state::prepare_next_widgets()
{
	using enum tr::align;

	std::unordered_map<tag, std::unique_ptr<widget>> map;
	std::map<std::string, replay_header>::const_iterator it{std::next(m_replays.begin(), REPLAYS_PER_PAGE * m_page)};
	for (usize i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const std::optional<std::map<std::string, replay_header>::const_iterator> opt_it{it != m_replays.end() ? std::optional{it++}
																											   : std::nullopt};
		const tweener<glm::vec2> move_in{tween::CUBIC, {i % 2 == 0 ? 600 : 400, 183 + 125 * i}, {500, 183 + 125 * i}, 0.25_s};
		map.emplace(REPLAY_TAGS[i], std::make_unique<replay_widget>(move_in, CENTER, 0.25_s, interactible, on_replay, opt_it));
	}
	return map;
}

void replays_state::set_up_page_switch_animation()
{
	for (usize i = 0; i < REPLAYS_PER_PAGE; i++) {
		widget& widget{m_ui[REPLAY_TAGS[i]]};
		widget.pos.change(tween::CUBIC, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
	m_next_widgets = std::async(std::launch::async, &replays_state::prepare_next_widgets, this);
}

void replays_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(tween::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_EXIT].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
	if (m_replays.empty()) {
		m_ui[T_NO_REPLAYS_FOUND].pos.change(tween::CUBIC, {400, 467}, 0.5_s);
	}
	else {
		for (usize i = 0; i < REPLAYS_PER_PAGE; i++) {
			widget& widget{m_ui[REPLAY_TAGS[i]]};
			widget.pos.change(tween::CUBIC, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.5_s);
		}
		m_ui[T_PAGE_C].pos.change(tween::CUBIC, BOTTOM_START_POS, 0.5_s);
		m_ui[T_PAGE_D].pos.change(tween::CUBIC, {-50, 942.5}, 0.5_s);
		m_ui[T_PAGE_I].pos.change(tween::CUBIC, {1050, 942.5}, 0.5_s);
	}
	m_ui.hide_all_widgets(0.5_s);
}

//

std::string replays_state::page_c_text()
{
	const replays_state& self{g_state_machine.get<replays_state>()};

	return TR_FMT::format("{}/{}", self.m_page + 1, std::max(self.m_replays.size() - 1, 0_uz) / REPLAYS_PER_PAGE + 1);
}

bool replays_state::interactible()
{
	const replays_state& self{g_state_machine.get<replays_state>()};

	return self.m_substate == substate::IN_REPLAYS;
}

bool replays_state::page_d_interactible()
{
	const replays_state& self{g_state_machine.get<replays_state>()};

	return self.m_substate == substate::IN_REPLAYS && self.m_page > 0;
}

bool replays_state::page_i_interactible()
{
	const replays_state& self{g_state_machine.get<replays_state>()};

	return self.m_substate == substate::IN_REPLAYS && self.m_page < (std::max(self.m_replays.size() - 1, 0_uz) / REPLAYS_PER_PAGE);
}

void replays_state::on_replay(std::map<std::string, replay_header>::const_iterator it)
{
	replays_state& self{g_state_machine.get<replays_state>()};

	self.m_substate = substate::STARTING_REPLAY;
	self.m_timer = 0;
	self.m_selected = it;
	self.set_up_exit_animation();
	g_audio.fade_song_out(0.5s);
	prepare_next_game_state<replay_game>(game_type::REPLAY, true, self.m_selected->second.gamemode, replay{self.m_selected->first});
}

void replays_state::on_page_d()
{
	replays_state& self{g_state_machine.get<replays_state>()};

	self.m_substate = substate::SWITCHING_PAGE;
	self.m_timer = 0;
	--self.m_page;
	self.set_up_page_switch_animation();
}

void replays_state::on_page_i()
{
	replays_state& self{g_state_machine.get<replays_state>()};

	self.m_substate = substate::SWITCHING_PAGE;
	self.m_timer = 0;
	++self.m_page;
	self.set_up_page_switch_animation();
}

void replays_state::on_exit()
{
	replays_state& self{g_state_machine.get<replays_state>()};

	self.m_substate = substate::ENTERING_TITLE;
	self.m_timer = 0;
	self.set_up_exit_animation();
	prepare_next_state<title_state>(self.m_game);
}