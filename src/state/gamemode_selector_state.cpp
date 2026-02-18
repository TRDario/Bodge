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

// Entry animation for the subtitle widget.
constexpr tweened_position SUBTITLE_ANIMATION{TOP_START_POS, {500, 64}, 0.5_s};
// Entry animation for the "no gamemodes found" widget.
constexpr tweened_position NO_GAMEMODES_FOUND_ANIMATION{{600, 467}, {500, 467}, 0.5_s};
// Entry animation for the previous page button widget.
constexpr tweened_position PAGE_D_ANIMATION{{-50, 942.5}, {10, 942.5}, 0.5_s};
// Entry animation for the current page widget.
constexpr tweened_position PAGE_C_ANIMATION{BOTTOM_START_POS, {500, 950}, 0.5_s};
// Entry animation for the next page button widget.
constexpr tweened_position PAGE_I_ANIMATION{{1050, 942.5}, {990, 942.5}, 0.5_s};
// Entry animation for the exit button widget.
constexpr tweened_position EXIT_ANIMATION{BOTTOM_START_POS, {500, 1000}, 0.5_s};

// clang-format on
///////////////////////////////////////////////////////// GAMEMODE SELECTOR STATE /////////////////////////////////////////////////////////

gamemode_selector_state::gamemode_selector_state(std::shared_ptr<playerless_game> game, gamemode_selector_type type,
												 animate_subtitle animate_subtitle)
	: main_menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}
	, m_substate{substate::IN_GAMEMODE_SELECTOR}
	, m_type{type}
	, m_gamemodes{load_gamemodes()}
	, m_page{0}
{
	switch (m_type) {
	case gamemode_selector_type::CLONE:
		break;
	case gamemode_selector_type::EDIT:
		std::erase_if(m_gamemodes,
					  [](const gamemode_with_path& gp) { return gp.gamemode.builtin || gp.gamemode.author != g_scorefile.name; });
		break;
	case gamemode_selector_type::DELETE:
		std::erase_if(m_gamemodes, [](const gamemode_with_path& gp) { return gp.gamemode.builtin; });
		break;
	}

	// STATUS CALLBACKS

	const status_callback scb{[this] { return m_substate == substate::IN_GAMEMODE_SELECTOR; }};
	const status_callback page_d_scb{[this] { return m_substate == substate::IN_GAMEMODE_SELECTOR && m_page > 0; }};
	const status_callback page_i_scb{[this] {
		const u16 last_page{u16(std::max(ssize(m_gamemodes.size()) - 1, 0_z) / GAMEMODES_PER_PAGE)};
		return m_substate == substate::IN_GAMEMODE_SELECTOR && m_page < last_page;
	}};

	// ACTION CALLBACKS

	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING;
		m_elapsed = 0;
		set_up_exit_animation(animate_subtitle::YES);
		m_next_state = make_async<gamemode_manager_state>(m_game, animate_title::NO);
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
	const gamemode_widget_action_callback gamemode_acb{this->gamemode_acb()};

	// TEXT CALLBACKS

	text_callback subtitle_tcb{loc_text_callback{m_type == gamemode_selector_type::CLONE  ? "clone_gamemode"
												 : m_type == gamemode_selector_type::EDIT ? "edit_gamemode"
																						  : "delete_gamemodes"}};
	const text_callback page_c_tcb{[this] {
		const usize total_pages{std::max(ssize(m_gamemodes.size()) - 1, 0_z) / GAMEMODES_PER_PAGE + 1};
		return TR_FMT::format("{}/{}", m_page + 1, total_pages);
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::sys::ttf_style::NORMAL, 64);
	if (bool(animate_subtitle)) {
		m_ui.emplace<label_widget>(T_SUBTITLE, SUBTITLE_ANIMATION, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, std::move(subtitle_tcb),
								   tr::sys::ttf_style::NORMAL, 32);
	}
	else {
		m_ui.emplace<label_widget>(T_SUBTITLE, glm::vec2{500, 64}, tr::align::TOP_CENTER, 0, NO_TOOLTIP, std::move(subtitle_tcb),
								   tr::sys::ttf_style::NORMAL, 32);
	}
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_ANIMATION, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
	if (m_gamemodes.empty()) {
		m_ui.emplace<label_widget>(T_NO_GAMEMODES_FOUND, NO_GAMEMODES_FOUND_ANIMATION, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP,
								   loc_text_callback{T_NO_GAMEMODES_FOUND}, tr::sys::ttf_style::NORMAL, 64, DARK_GRAY);
		return;
	}
	for (usize i = 0; i < GAMEMODES_PER_PAGE; ++i) {
		std::optional<gamemode_with_path> opt_gp{i < m_gamemodes.size() ? std::optional{m_gamemodes[i]} : std::nullopt};
		const tweened_position animation{{i % 2 == 0 ? 400 : 600, 160 + 75 * i}, {500, 160 + 75 * i}, 0.5_s};
		m_ui.emplace<gamemode_widget>(GAMEMODE_TAGS[i], animation, tr::align::CENTER, 0.5_s, scb, gamemode_acb, std::move(opt_gp));
	}
	m_ui.emplace<arrow_widget>(T_PAGE_D, PAGE_D_ANIMATION, tr::valign::BOTTOM, 0.5_s, arrow_type::LEFT, page_d_scb, page_d_acb);
	m_ui.emplace<label_widget>(T_PAGE_C, PAGE_C_ANIMATION, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, page_c_tcb,
							   tr::sys::ttf_style::NORMAL, 48);
	m_ui.emplace<arrow_widget>(T_PAGE_I, PAGE_I_ANIMATION, tr::valign::BOTTOM, 0.5_s, arrow_type::RIGHT, page_i_scb, page_i_acb);
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

gamemode_widget_action_callback gamemode_selector_state::gamemode_acb()
{
	switch (m_type) {
	case gamemode_selector_type::CLONE:
		return [this](const gamemode_with_path& gp) {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			set_up_exit_animation(animate_subtitle::NO);

			::gamemode clone{gp.gamemode};
			if (clone.builtin) {
				clone.name = clone.name_loc();
				clone.description = clone.description_loc();
				clone.builtin = false;
			}
			m_next_state = make_async<gamemode_editor_state>(m_game, cloned_gamemode_editor_data{}, std::move(clone), animate_subtitle::NO);
		};
	case gamemode_selector_type::EDIT:
		return [this](const gamemode_with_path& gp) {
			m_substate = substate::EXITING;
			m_elapsed = 0;
			set_up_exit_animation(animate_subtitle::NO);

			m_next_state =
				make_async<gamemode_editor_state>(m_game, edited_gamemode_editor_data{gp.path}, gp.gamemode, animate_subtitle::NO);
		};
	case gamemode_selector_type::DELETE:
		return [this](const gamemode_with_path& gp) {
			const ssize index{std::distance(m_gamemodes.begin(), std::ranges::find(m_gamemodes, gp))};
			const usize page_index{usize(index) % GAMEMODES_PER_PAGE};
			const usize max_index{m_page * GAMEMODES_PER_PAGE + 9_uz};
			m_gamemodes.erase(m_gamemodes.begin() + index);
			std::filesystem::remove(gp.path);
			for (usize i = page_index; i < GAMEMODES_PER_PAGE - 1; ++i) {
				std::swap(m_ui.as<gamemode_widget>(GAMEMODE_TAGS[i]).gp, m_ui.as<gamemode_widget>(GAMEMODE_TAGS[i + 1]).gp);
			}
			m_ui.as<gamemode_widget>(T_GAMEMODE_9).gp =
				max_index < m_gamemodes.size() ? std::optional{m_gamemodes[max_index]} : std::nullopt;
		};
	}
}

std::unordered_map<tag, std::unique_ptr<widget>> gamemode_selector_state::prepare_next_widgets()
{
	const status_callback scb{[this] { return m_substate == substate::IN_GAMEMODE_SELECTOR; }};
	const gamemode_widget_action_callback acb{gamemode_acb()};

	std::unordered_map<tag, std::unique_ptr<widget>> map;
	std::vector<gamemode_with_path>::iterator gamemode_it{std::next(m_gamemodes.begin(), GAMEMODES_PER_PAGE * m_page)};
	for (usize i = 0; i < GAMEMODES_PER_PAGE; ++i) {
		std::optional<gamemode_with_path> opt_gp{gamemode_it != m_gamemodes.end() ? std::optional{*gamemode_it++} : std::nullopt};
		const tweened_position animation{{i % 2 == 0 ? 600 : 400, 160 + 75 * i}, {500, 160 + 75 * i}, 0.25_s};
		map.emplace(GAMEMODE_TAGS[i], std::make_unique<gamemode_widget>(animation, tr::align::CENTER, 0.25_s, scb, acb, opt_gp));
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