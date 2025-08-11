#include "../../include/state/scoreboards_state.hpp"
#include "../../include/graphics.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The number of scores per page.
constexpr std::size_t SCORES_PER_PAGE{8};

constexpr tag T_TITLE{"scoreboards"};
constexpr tag T_PLAYER_INFO{"player_info"};
constexpr tag T_NO_SCORES_FOUND{"no_scores_found"};
constexpr tag T_SCORE_0{"score0"};
constexpr tag T_SCORE_1{"score1"};
constexpr tag T_SCORE_2{"score2"};
constexpr tag T_SCORE_3{"score3"};
constexpr tag T_SCORE_4{"score4"};
constexpr tag T_SCORE_5{"score5"};
constexpr tag T_SCORE_6{"score6"};
constexpr tag T_SCORE_7{"score7"};
constexpr tag T_GAMEMODE_DEC{"gamemode_dec"};
constexpr tag T_CUR_GAMEMODE{"cur_gamemode"};
constexpr tag T_GAMEMODE_INC{"gamemode_inc"};
constexpr tag T_PAGE_DEC{"page_dec"};
constexpr tag T_CUR_PAGE{"cur_page"};
constexpr tag T_PAGE_INC{"page_inc"};
constexpr tag T_EXIT{"exit"};

constexpr std::array<tag, SCORES_PER_PAGE> SCORE_TAGS{
	T_SCORE_0, T_SCORE_1, T_SCORE_2, T_SCORE_3, T_SCORE_4, T_SCORE_5, T_SCORE_6, T_SCORE_7,
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::ESCAPE}, T_EXIT},
	{{tr::system::keycode::LEFT, tr::system::keymod::SHIFT}, T_GAMEMODE_DEC},
	{{tr::system::keycode::RIGHT, tr::system::keymod::SHIFT}, T_GAMEMODE_INC},
	{{tr::system::keycode::LEFT}, T_PAGE_DEC},
	{{tr::system::keycode::RIGHT}, T_PAGE_INC},
};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

scoreboards_state::scoreboards_state(std::unique_ptr<game>&& game)
	: m_substate{substate::IN_SCORES}
	, m_page{0}
	, m_timer{0}
	, m_ui{SHORTCUTS}
	, m_background_game{std::move(game)}
	, m_selected{engine::scorefile.categories.begin()}
{
	// STATUS CALLBACKS

	const status_callback status_cb{[this] { return m_substate == substate::IN_SCORES; }};
	const status_callback gamemode_change_status_cb{
		[this] { return m_substate == substate::IN_SCORES && engine::scorefile.categories.size() != 1; }};
	const status_callback page_dec_status_cb{[this] { return m_substate == substate::IN_SCORES && m_page > 0; }};
	const status_callback page_inc_status_cb{[this] {
		return m_substate == substate::IN_SCORES &&
			   m_page < (std::max(std::ssize(m_selected->scores) - 1, std::ptrdiff_t{0}) / SCORES_PER_PAGE);
	}};

	// ACTION CALLBACKS

	const action_callback exit_action_cb{[this] {
		m_substate = substate::EXITING_TO_TITLE;
		m_timer = 0;
		set_up_exit_animation();
	}};
	const action_callback gamemode_dec_action_cb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_timer = 0;
		m_page = 0;
		if (m_selected == engine::scorefile.categories.begin()) {
			m_selected = engine::scorefile.categories.end();
		}
		--m_selected;
		set_up_page_switch_animation();
	}};
	const action_callback gamemode_inc_action_cb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_timer = 0;
		m_page = 0;
		if (++m_selected == engine::scorefile.categories.end()) {
			m_selected = engine::scorefile.categories.begin();
		}
		set_up_page_switch_animation();
	}};
	const action_callback page_dec_action_cb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_timer = 0;
		--m_page;
		set_up_page_switch_animation();
	}};
	const action_callback page_inc_action_cb{[this] {
		m_substate = substate::SWITCHING_PAGE;
		m_timer = 0;
		++m_page;
		set_up_page_switch_animation();
	}};

	// TEXT CALLBACKS

	const text_callback player_info_text_cb{[] {
		return std::format("{} {}: {}:{:02}:{:02}", engine::loc["total_playtime"], engine::scorefile.name,
						   engine::scorefile.playtime / (SECOND_TICKS * 3600),
						   (engine::scorefile.playtime % (SECOND_TICKS * 3600)) / (SECOND_TICKS * 60),
						   (engine::scorefile.playtime % (SECOND_TICKS * 60) / SECOND_TICKS));
	}};
	const text_callback cur_gamemode_text_cb{[this] { return std::string{name(m_selected->gamemode)}; }};
	const text_callback cur_page_text_cb{[this] {
		return std::format("{}/{}", m_page + 1, std::max(std::ssize(m_selected->scores) - 1, std::ptrdiff_t{0}) / SCORES_PER_PAGE + 1);
	}};

	//

	widget& title{m_ui.emplace<text_widget>(T_TITLE, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL,
											64, loc_text_callback{T_TITLE})};
	title.pos.change(interp_mode::CUBE, {500, 0}, 0.5_s);
	title.unhide(0.5_s);

	widget& player_info{m_ui.emplace<text_widget>(T_PLAYER_INFO, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE,
												  tr::system::ttf_style::NORMAL, 32, player_info_text_cb)};
	player_info.pos.change(interp_mode::CUBE, {500, 64}, 0.5_s);
	player_info.unhide(0.5_s);

	widget& exit{m_ui.emplace<clickable_text_widget>(T_EXIT, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													 loc_text_callback{T_EXIT}, status_cb, exit_action_cb, NO_TOOLTIP, sound::CANCEL)};
	exit.pos.change(interp_mode::CUBE, {500, 1000}, 0.5_s);
	exit.unhide(0.5_s);

	if (engine::scorefile.categories.empty()) {
		widget& no_scores_found{m_ui.emplace<text_widget>(T_NO_SCORES_FOUND, glm::vec2{600, 483}, tr::align::TOP_CENTER, font::LANGUAGE,
														  tr::system::ttf_style::NORMAL, 64, loc_text_callback{T_NO_SCORES_FOUND},
														  "80808080"_rgba8)};
		no_scores_found.pos.change(interp_mode::CUBE, {500, 483}, 0.5_s);
		no_scores_found.unhide(0.5_s);
		return;
	}

	for (std::size_t i = 0; i < SCORES_PER_PAGE; ++i) {
		const glm::vec2 start_pos{i % 2 == 0 ? 400 : 600, 173 + 86 * i};
		const std::size_t rank{m_page * SCORES_PER_PAGE + i + 1};
		score* const score{m_selected->scores.size() > i ? std::to_address(m_selected->scores.begin() + i) : nullptr};
		widget& widget{m_ui.emplace<score_widget>(SCORE_TAGS[i], start_pos, tr::align::CENTER, rank, score)};
		widget.pos.change(interp_mode::CUBE, {500, 173 + 86 * i}, 0.5_s);
		widget.unhide(0.5_s);
	}

	const text_callback cur_gamemode_tooltip_cb{[this] { return std::string{description(m_selected->gamemode)}; }};
	widget& cur_gamemode{m_ui.emplace<text_widget>(
		T_CUR_GAMEMODE, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, true, cur_gamemode_tooltip_cb, false, font::LANGUAGE,
		tr::system::ttf_style::NORMAL, tr::halign::CENTER, 48, tr::system::UNLIMITED_WIDTH, "A0A0A0A0"_rgba8, cur_gamemode_text_cb)};
	cur_gamemode.pos.change(interp_mode::CUBE, {500, 900}, 0.5_s);
	cur_gamemode.unhide(0.5_s);

	widget& gamemode_dec{m_ui.emplace<arrow_widget>(T_GAMEMODE_DEC, glm::vec2{-50, 892.5}, tr::align::BOTTOM_LEFT, false,
													gamemode_change_status_cb, gamemode_dec_action_cb)};
	gamemode_dec.pos.change(interp_mode::CUBE, {10, 892.5}, 0.5_s);
	gamemode_dec.unhide(0.5_s);

	widget& gamemode_inc{m_ui.emplace<arrow_widget>(T_GAMEMODE_INC, glm::vec2{1050, 892.5}, tr::align::BOTTOM_RIGHT, true,
													gamemode_change_status_cb, gamemode_inc_action_cb)};
	gamemode_inc.pos.change(interp_mode::CUBE, {990, 892.5}, 0.5_s);
	gamemode_inc.unhide(0.5_s);

	widget& cur_page{m_ui.emplace<text_widget>(T_CUR_PAGE, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE,
											   tr::system::ttf_style::NORMAL, 48, cur_page_text_cb)};
	cur_page.pos.change(interp_mode::CUBE, {500, 950}, 0.5_s);
	cur_page.unhide(0.5_s);

	widget& page_dec{m_ui.emplace<arrow_widget>(T_PAGE_DEC, glm::vec2{-50, 942.5}, tr::align::BOTTOM_LEFT, false, page_dec_status_cb,
												page_dec_action_cb)};
	page_dec.pos.change(interp_mode::CUBE, {10, 942.5}, 0.5_s);
	page_dec.unhide(0.5_s);

	widget& page_inc{m_ui.emplace<arrow_widget>(T_PAGE_INC, glm::vec2{1050, 942.5}, tr::align::BOTTOM_RIGHT, true, page_inc_status_cb,
												page_inc_action_cb)};
	page_inc.pos.change(interp_mode::CUBE, {990, 942.5}, 0.5_s);
	page_inc.unhide(0.5_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> scoreboards_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> scoreboards_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

	switch (m_substate) {
	case substate::IN_SCORES:
		return nullptr;
	case substate::SWITCHING_PAGE:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate::IN_SCORES;
		}
		else if (m_timer == 0.25_s) {
			for (std::size_t i = 0; i < SCORES_PER_PAGE; ++i) {
				const bool nonempty{m_selected->scores.size() > m_page * SCORES_PER_PAGE + i};
				score_widget& widget{m_ui.as<score_widget>(SCORE_TAGS[i])};
				widget.rank = m_page * SCORES_PER_PAGE + i + 1;
				widget.score = nonempty ? std::to_address(m_selected->scores.begin() + m_page * SCORES_PER_PAGE + i) : nullptr;
				widget.pos = {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y};
				widget.pos.change(interp_mode::CUBE, {500, glm::vec2{widget.pos}.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
		return nullptr;
	case substate::EXITING_TO_TITLE:
		return m_timer >= 0.5_s ? std::make_unique<title_state>(std::move(m_background_game)) : nullptr;
	}
}

void scoreboards_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void scoreboards_state::set_up_page_switch_animation()
{
	for (std::size_t i = 0; i < SCORES_PER_PAGE; i++) {
		widget& widget{m_ui[SCORE_TAGS[i]]};
		widget.pos.change(interp_mode::CUBE, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
}

void scoreboards_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	m_ui[T_PLAYER_INFO].pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	m_ui[T_EXIT].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	if (engine::scorefile.categories.empty()) {
		m_ui[T_NO_SCORES_FOUND].pos.change(interp_mode::CUBE, {400, 483}, 0.5_s);
	}
	else {
		for (std::size_t i = 0; i < SCORES_PER_PAGE; i++) {
			widget& widget{m_ui[SCORE_TAGS[i]]};
			widget.pos.change(interp_mode::CUBE, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.5_s);
		}
		m_ui[T_CUR_GAMEMODE].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
		m_ui[T_GAMEMODE_DEC].pos.change(interp_mode::CUBE, {-50, 892.5}, 0.5_s);
		m_ui[T_GAMEMODE_INC].pos.change(interp_mode::CUBE, {1050, 892.5}, 0.5_s);
		m_ui[T_CUR_PAGE].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
		m_ui[T_PAGE_DEC].pos.change(interp_mode::CUBE, {-50, 942.5}, 0.5_s);
		m_ui[T_PAGE_INC].pos.change(interp_mode::CUBE, {1050, 942.5}, 0.5_s);
	}
	m_ui.hide_all(0.5_s);
}