///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements gamemode_selector_state from state.hpp.                                                                                    //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

// Number of gamemodes displayed per page.
constexpr usize GAMEMODES_PER_PAGE{10};

constexpr tag T_TITLE{"gamemode_manager"};
constexpr tag T_SUBTITLE{"subtitle"};
constexpr tag T_NO_GAMEMODES_FOUND{"no_gamemodes_found"};
constexpr tag T_GAMEMODE_0{"gamemode0"};
constexpr tag T_GAMEMODE_1{"gamemode1"};
constexpr tag T_GAMEMODE_2{"gamemode2"};
constexpr tag T_GAMEMODE_3{"gamemode3"};
constexpr tag T_GAMEMODE_4{"gamemode4"};
constexpr tag T_GAMEMODE_5{"gamemode5"};
constexpr tag T_GAMEMODE_6{"gamemode6"};
constexpr tag T_GAMEMODE_7{"gamemode7"};
constexpr tag T_GAMEMODE_8{"gamemode8"};
constexpr tag T_GAMEMODE_9{"gamemode9"};
constexpr tag T_PAGE_D{"page_d"};
constexpr tag T_PAGE_C{"page_c"};
constexpr tag T_PAGE_I{"page_i"};
constexpr tag T_EXIT{"exit"};

// Replay widget tags.
constexpr std::array GAMEMODE_TAGS{
    T_GAMEMODE_0,
    T_GAMEMODE_1,
    T_GAMEMODE_2,
    T_GAMEMODE_3,
    T_GAMEMODE_4,
    T_GAMEMODE_5,
    T_GAMEMODE_6,
    T_GAMEMODE_7,
    T_GAMEMODE_8,
    T_GAMEMODE_9,
};

// Selection tree for the gamemode selector menu.
constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_GAMEMODE_0},
	selection_tree_row{T_GAMEMODE_1},
	selection_tree_row{T_GAMEMODE_2},
	selection_tree_row{T_GAMEMODE_3},
	selection_tree_row{T_GAMEMODE_4},
	selection_tree_row{T_GAMEMODE_5},
    selection_tree_row{T_GAMEMODE_6},
    selection_tree_row{T_GAMEMODE_7},
    selection_tree_row{T_GAMEMODE_8},
    selection_tree_row{T_GAMEMODE_9},
	selection_tree_row{T_EXIT},
};

// Shortcut table for the gamemode selector menu.
constexpr shortcut_table SHORTCUTS{
	{"1"_kc, T_GAMEMODE_0},
	{"2"_kc, T_GAMEMODE_1},
	{"3"_kc, T_GAMEMODE_2},
	{"4"_kc, T_GAMEMODE_3},
	{"5"_kc, T_GAMEMODE_4},
	{"6"_kc, T_GAMEMODE_5},
	{"7"_kc, T_GAMEMODE_6},
	{"8"_kc, T_GAMEMODE_7},
	{"9"_kc, T_GAMEMODE_8},
	{"0"_kc, T_GAMEMODE_9},
	{"Left"_kc, T_PAGE_D},
	{"Right"_kc, T_PAGE_I},
	{"Escape"_kc, T_EXIT}, {"Q"_kc, T_EXIT},
};

// clang-format on
//////////////////////////////////////////////////////////// GAMEMODE SELECTOR ////////////////////////////////////////////////////////////

void clone_gamemode_selector::filter_gamemodes(std::vector<gamemode_with_path>&) const {}

localized_text clone_gamemode_selector::subtitle_text() const
{
	return localized_text{"clone_gamemode"};
}

void clone_gamemode_selector::on_gamemode_selected(gamemode_selector_state& state, const gamemode_with_path& gp) const
{
	state.m_substate = gamemode_selector_state::substate::EXITING;
	state.m_elapsed = 0;
	state.set_up_exit_animation(animate_subtitle::NO);

	::gamemode clone{gp.gamemode};
	if (clone.builtin) {
		clone.name = clone.name_loc();
		clone.description = clone.description_loc();
		clone.builtin = false;
	}
	state.m_next_state = make_async<gamemode_editor_state>(state.m_game, cloned_gamemode_editor{}, std::move(clone), animate_subtitle::NO);
}

//

void edit_gamemode_selector::filter_gamemodes(std::vector<gamemode_with_path>& gamemodes) const
{
	std::erase_if(gamemodes, [](const gamemode_with_path& gp) { return gp.gamemode.builtin || gp.gamemode.author != g_scorefile.name; });
}

localized_text edit_gamemode_selector::subtitle_text() const
{
	return localized_text{"edit_gamemode"};
}

void edit_gamemode_selector::on_gamemode_selected(gamemode_selector_state& state, const gamemode_with_path& gp) const
{
	state.m_substate = gamemode_selector_state::substate::EXITING;
	state.m_elapsed = 0;
	state.set_up_exit_animation(animate_subtitle::NO);

	state.m_next_state =
		make_async<gamemode_editor_state>(state.m_game, edited_gamemode_editor{gp.path}, gp.gamemode, animate_subtitle::NO);
}

//

void delete_gamemode_selector::filter_gamemodes(std::vector<gamemode_with_path>& gamemodes) const
{
	std::erase_if(gamemodes, [](const gamemode_with_path& gp) { return gp.gamemode.builtin; });
}

localized_text delete_gamemode_selector::subtitle_text() const
{
	return localized_text{"delete_gamemode"};
}

void delete_gamemode_selector::on_gamemode_selected(gamemode_selector_state& state, const gamemode_with_path& gp) const
{
	const ssize index{std::distance(state.m_gamemodes.begin(), std::ranges::find(state.m_gamemodes, gp))};
	const usize page_index{usize(index) % GAMEMODES_PER_PAGE};
	const usize max_index{state.m_page * GAMEMODES_PER_PAGE + 9_uz};
	state.m_gamemodes.erase(state.m_gamemodes.begin() + index);
	std::filesystem::remove(gp.path);
	for (usize i = page_index; i < GAMEMODES_PER_PAGE - 1; ++i) {
		std::swap(state.m_ui.as<gamemode_widget>(GAMEMODE_TAGS[i]).gp, state.m_ui.as<gamemode_widget>(GAMEMODE_TAGS[i + 1]).gp);
	}
	state.m_ui.as<gamemode_widget>(T_GAMEMODE_9).gp =
		max_index < state.m_gamemodes.size() ? std::optional{state.m_gamemodes[max_index]} : std::nullopt;
}

///////////////////////////////////////////////////////// GAMEMODE SELECTOR STATE /////////////////////////////////////////////////////////

gamemode_selector_state::gamemode_selector_state(std::shared_ptr<playerless_game> game, gamemode_selector selector,
												 animate_subtitle animate_subtitle)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_GAMEMODE_SELECTOR}
	, m_selector{selector}
	, m_gamemodes{load_gamemodes()}
	, m_page{0}
{
	std::visit([&](auto& selector) { selector.filter_gamemodes(m_gamemodes); }, m_selector);

	// clang-format off
	m_ui.emplace<label_widget>(T_TITLE, {
		.animation = TITLE_POS,
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = 0_s,
		.text = localized_text{T_TITLE},
		.font_size = 64
	});
	m_ui.emplace<label_widget>(T_SUBTITLE, {
		.animation = bool(animate_subtitle) ? tweened_position{TOP_START_POS, {500, 64}, 0.5_s} : tweened_position{{500, 64}},
		.alignment = tr::align::TOP_CENTER,
		.unhide_time = bool(animate_subtitle) ? 0.5_s : 0_s,
		.text = std::visit([](const auto& selector) { return selector.subtitle_text(); }, m_selector),
		.font_size = 32
	});
	m_ui.emplace<text_button_widget>(T_EXIT,
		tweened_position{BOTTOM_START_POS, {500, 1000}, 0.5_s},
		tr::align::BOTTOM_CENTER,
		0.5_s,
		NO_TOOLTIP,
		localized_text{T_EXIT},
		font::LANGUAGE,
		48,
		[this] { return m_substate == substate::IN_GAMEMODE_SELECTOR; },
		[this] { on_exit(); },
		sound::CANCEL
	);
	if (m_gamemodes.empty()) {
		m_ui.emplace<label_widget>(T_NO_GAMEMODES_FOUND, {
			.animation = tweened_position{{600, 467}, {500, 467}, 0.5_s},
			.alignment = tr::align::TOP_CENTER,
			.text = localized_text{T_NO_GAMEMODES_FOUND},
			.font_size = 64,
			.color = DARK_GRAY
		});
		return;
	}
	for (usize i = 0; i < GAMEMODES_PER_PAGE; ++i) {
		m_ui.emplace<gamemode_widget>(GAMEMODE_TAGS[i],
			tweened_position{{i % 2 == 0 ? 400 : 600, 160 + 75 * i}, {500, 160 + 75 * i}, 0.5_s},
			tr::align::CENTER,
			0.5_s,
			[this] { return m_substate == substate::IN_GAMEMODE_SELECTOR; },
			[this] (const gamemode_with_path& gp) { on_gamemode_selected(gp); },
			i < m_gamemodes.size() ? std::optional{m_gamemodes[i]} : std::nullopt
		);
	}
	m_ui.emplace<arrow_widget>(T_PAGE_D, {
		.animation = {{-50, 942.5}, {10, 942.5}, 0.5_s},
		.alignment = tr::valign::BOTTOM,
		.type = arrow_type::LEFT,
		.status = [this] { return m_substate == substate::IN_GAMEMODE_SELECTOR && m_page > 0; },
		.action = [this] { on_page_decrement(); }
	});
	m_ui.emplace<label_widget>(T_PAGE_C, {
		.animation = {BOTTOM_START_POS, {500, 950}, 0.5_s},
		.alignment = tr::align::BOTTOM_CENTER,
		.text = [this] {
			const usize total_pages{std::max(ssize(m_gamemodes.size()) - 1, 0_z) / GAMEMODES_PER_PAGE + 1};
			return TR_FMT::format("{}/{}", m_page + 1, total_pages);
		},
	});
	m_ui.emplace<arrow_widget>(T_PAGE_I, {
		.animation = {{1050, 942.5}, {990, 942.5}, 0.5_s},
		.alignment = tr::valign::BOTTOM,
		.type = arrow_type::RIGHT,
		.status = [this] {
			const u16 last_page{u16(std::max(ssize(m_gamemodes.size()) - 1, 0_z) / GAMEMODES_PER_PAGE)};
			return m_substate == substate::IN_GAMEMODE_SELECTOR && m_page < last_page;
		},
		.action = [this] { on_page_increment(); }
	});
	// clang-format on
}

tr::next_state gamemode_selector_state::tick()
{
	main_menu_state::tick();
	switch (m_substate) {
	case substate::IN_GAMEMODE_SELECTOR:
		return tr::KEEP_STATE;
	case substate::SWITCHING_PAGE:
		if (m_elapsed >= 0.5_s) {
			m_elapsed = 0;
			m_substate = substate::IN_GAMEMODE_SELECTOR;
		}
		else if (m_elapsed == 0.25_s) {
			m_ui.replace(m_next_widgets.get());
		}
		return tr::KEEP_STATE;
	case substate::EXITING:
		return next_state_if_after(0.5_s);
	}
}

//

void gamemode_selector_state::on_gamemode_selected(const gamemode_with_path& gp)
{
	std::visit([&gp, this](const auto& selector) { selector.on_gamemode_selected(*this, gp); }, m_selector);
}

void gamemode_selector_state::on_exit()
{
	m_substate = substate::EXITING;
	m_elapsed = 0;
	set_up_exit_animation(animate_subtitle::YES);
	m_next_state = make_async<gamemode_manager_state>(m_game, animate_title::NO);
}

void gamemode_selector_state::on_page_decrement()
{
	m_substate = substate::SWITCHING_PAGE;
	m_elapsed = 0;
	--m_page;
	set_up_page_switch_animation();
}

void gamemode_selector_state::on_page_increment()
{
	m_substate = substate::SWITCHING_PAGE;
	m_elapsed = 0;
	++m_page;
	set_up_page_switch_animation();
}

//

std::unordered_map<tag, std::unique_ptr<widget>> gamemode_selector_state::prepare_next_widgets()
{
	std::unordered_map<tag, std::unique_ptr<widget>> map;
	std::vector<gamemode_with_path>::iterator gamemode_it{std::next(m_gamemodes.begin(), GAMEMODES_PER_PAGE * m_page)};
	for (usize i = 0; i < GAMEMODES_PER_PAGE; ++i) {
		// clang-format off
		map.emplace(GAMEMODE_TAGS[i], std::make_unique<gamemode_widget>(
			tweened_position{{i % 2 == 0 ? 600 : 400, 160 + 75 * i}, {500, 160 + 75 * i}, 0.25_s},
			tr::align::CENTER,
			0.25_s,
			[this] { return m_substate == substate::IN_GAMEMODE_SELECTOR; },
			[this] (const gamemode_with_path& gp) { on_gamemode_selected(gp); },
			gamemode_it != m_gamemodes.end() ? std::optional{*gamemode_it++} : std::nullopt
		));
		// clang-format on
	}
	return map;
}

void gamemode_selector_state::set_up_page_switch_animation()
{
	for (usize i = 0; i < GAMEMODES_PER_PAGE; i++) {
		m_ui[GAMEMODE_TAGS[i]].move_x_and_hide(i % 2 == 0 ? 600 : 400, 0.25_s);
	}
	m_next_widgets = std::async(std::launch::async, &gamemode_selector_state::prepare_next_widgets, this);
}

void gamemode_selector_state::set_up_exit_animation(animate_subtitle animate_subtitle)
{
	if (bool(animate_subtitle)) {
		m_ui[T_SUBTITLE].move_and_hide(TOP_START_POS, 0.5_s);
	}
	m_ui[T_EXIT].move_and_hide(BOTTOM_START_POS, 0.5_s);
	if (m_ui.contains(T_NO_GAMEMODES_FOUND)) {
		m_ui[T_NO_GAMEMODES_FOUND].move_x_and_hide(400, 0.5_s);
	}
	else {
		for (usize i = 0; i < GAMEMODES_PER_PAGE; i++) {
			m_ui[GAMEMODE_TAGS[i]].move_x_and_hide(i % 2 == 0 ? 600 : 400, 0.5_s);
		}
		m_ui[T_PAGE_C].move_and_hide(BOTTOM_START_POS, 0.5_s);
		m_ui[T_PAGE_D].move_x_and_hide(-50, 0.5_s);
		m_ui[T_PAGE_I].move_x_and_hide(1050, 0.5_s);
	}
}