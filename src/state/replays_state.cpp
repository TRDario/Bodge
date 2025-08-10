#include "../../include/state/replays_state.hpp"
#include "../../include/graphics.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The number of replays per page.
constexpr std::size_t REPLAYS_PER_PAGE{5};

constexpr tag TAG_TITLE{"replays"};
constexpr tag TAG_NO_REPLAYS_FOUND{"no_replays_found"};
constexpr tag TAG_REPLAY_0{"replay0"};
constexpr tag TAG_REPLAY_1{"replay1"};
constexpr tag TAG_REPLAY_2{"replay2"};
constexpr tag TAG_REPLAY_3{"replay3"};
constexpr tag TAG_REPLAY_4{"replay4"};
constexpr std::array<const char*, REPLAYS_PER_PAGE> REPLAY_TAGS{TAG_REPLAY_0, TAG_REPLAY_1, TAG_REPLAY_2, TAG_REPLAY_3, TAG_REPLAY_4};
constexpr tag TAG_PAGE_DEC{"page_dec"};
constexpr tag TAG_CUR_PAGE{"cur_page"};
constexpr tag TAG_PAGE_INC{"page_inc"};
constexpr tag TAG_EXIT{"exit"};

// Shortcuts of the exit button.
constexpr std::initializer_list<tr::system::key_chord> EXIT_SHORTCUTS{
	{tr::system::keycode::ESCAPE}, {tr::system::keycode::Q}, {tr::system::keycode::E}};
// Shortcuts of the page decrement button.
constexpr std::initializer_list<tr::system::key_chord> PAGE_DEC_SHORTCUTS{{tr::system::keycode::LEFT}};
// Shortcuts of the page increment button.
constexpr std::initializer_list<tr::system::key_chord> PAGE_INC_SHORTCUTS{{tr::system::keycode::RIGHT}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

replays_state::replays_state()
	: m_substate{substate::RETURNING_FROM_REPLAY}
	, m_page{0}
	, m_timer{0}
	, m_background_game{std::make_unique<game>(pick_menu_gamemode(), engine::rng.generate<std::uint64_t>())}
	, m_replays{load_replay_headers()}
{
	set_up_ui();
	engine::play_song("menu", SKIP_MENU_SONG_INTRO, 0.5s);
}

replays_state::replays_state(std::unique_ptr<game>&& game)
	: m_substate{substate::IN_REPLAYS}, m_page{0}, m_timer{0}, m_background_game{std::move(game)}, m_replays{load_replay_headers()}
{
	set_up_ui();
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> replays_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> replays_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

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
				replay_widget& widget{m_ui.get<replay_widget>(REPLAY_TAGS[i])};
				widget.it = it != m_replays.end() ? std::optional{it++} : std::nullopt;
				widget.pos = {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y};
				widget.pos.change(interp_mode::CUBE, {500, glm::vec2{widget.pos}.y}, 0.25_s);
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
		return m_timer >= 0.5_s ? std::make_unique<title_state>(std::move(m_background_game)) : nullptr;
	}
}

void replays_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float replays_state::fade_overlay_opacity() const
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

	const status_callback status_cb{[this] { return m_substate == substate::IN_REPLAYS; }};
	const status_callback page_dec_status_cb{[this] { return m_substate == substate::IN_REPLAYS && m_page > 0; }};
	const status_callback page_inc_status_cb{[this] {
		return m_substate == substate::IN_REPLAYS && m_page < (std::max(m_replays.size() - 1, std::size_t{0}) / REPLAYS_PER_PAGE);
	}};

	// ACTION CALLBACKS

	const auto replay_action_cb{[this](std::map<std::string, replay_header>::iterator it) {
		m_substate = substate::STARTING_REPLAY;
		m_timer = 0;
		m_selected = it;
		set_up_exit_animation();
		engine::fade_song_out(0.5s);
	}};
	action_callback exit_action_cb{[this] {
		m_substate = substate::ENTERING_TITLE;
		m_timer = 0;
		set_up_exit_animation();
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

	//

	widget& title{
		m_ui.emplace<text_widget>(TAG_TITLE, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL, 64)};
	title.pos.change(interp_mode::CUBE, {500, 0}, 0.5_s);
	title.unhide(0.5_s);

	widget& exit{m_ui.emplace<clickable_text_widget>(TAG_EXIT, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													 DEFAULT_TEXT_CALLBACK, status_cb, exit_action_cb, NO_TOOLTIP, EXIT_SHORTCUTS,
													 sound::CANCEL)};
	exit.pos.change(interp_mode::CUBE, {500, 1000}, 0.5_s);
	exit.unhide(0.5_s);

	if (m_replays.empty()) {
		widget& no_replays_found{m_ui.emplace<text_widget>(TAG_NO_REPLAYS_FOUND, glm::vec2{600, 467}, tr::align::TOP_CENTER, font::LANGUAGE,
														   tr::system::ttf_style::NORMAL, 64, DEFAULT_TEXT_CALLBACK, "80808080"_rgba8)};
		no_replays_found.pos.change(interp_mode::CUBE, {500, 467}, 0.5_s);
		no_replays_found.unhide(0.5_s);
		return;
	}

	std::map<std::string, replay_header>::iterator it{m_replays.begin()};
	for (std::size_t i = 0; i < REPLAYS_PER_PAGE; ++i) {
		const std::optional<std::map<std::string, replay_header>::iterator> opt_it{it != m_replays.end() ? std::optional{it++}
																										 : std::nullopt};
		const glm::vec2 pos{i % 2 == 0 ? 400 : 600, 183 + 150 * i};
		widget& widget{m_ui.emplace<replay_widget>(REPLAY_TAGS[i], pos, tr::align::CENTER, status_cb, replay_action_cb, opt_it,
												   tr::system::make_top_row_keycode(i + 1))};
		widget.pos.change(interp_mode::CUBE, {500, 183 + 150 * i}, 0.5_s);
		widget.unhide(0.5_s);
	}

	const text_callback cur_page_text_cb{
		[this](auto&) { return std::format("{}/{}", m_page + 1, std::max(m_replays.size() - 1, std::size_t{0}) / REPLAYS_PER_PAGE + 1); }};
	widget& cur_page{m_ui.emplace<text_widget>(TAG_CUR_PAGE, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE,
											   tr::system::ttf_style::NORMAL, 48, cur_page_text_cb)};
	cur_page.pos.change(interp_mode::CUBE, {500, 950}, 0.5_s);
	cur_page.unhide(0.5_s);

	widget& page_dec{m_ui.emplace<arrow_widget>(TAG_PAGE_DEC, glm::vec2{-50, 942.5}, tr::align::BOTTOM_LEFT, false, page_dec_status_cb,
												page_dec_action_cb, PAGE_DEC_SHORTCUTS)};
	page_dec.pos.change(interp_mode::CUBE, {10, 942.5}, 0.5_s);
	page_dec.unhide(0.5_s);

	widget& page_inc{m_ui.emplace<arrow_widget>(TAG_PAGE_INC, glm::vec2{1050, 942.5}, tr::align::BOTTOM_RIGHT, true, page_inc_status_cb,
												page_inc_action_cb, PAGE_INC_SHORTCUTS)};
	page_inc.pos.change(interp_mode::CUBE, {990, 942.5}, 0.5_s);
	page_inc.unhide(0.5_s);
}

void replays_state::set_up_page_switch_animation()
{
	for (std::size_t i = 0; i < REPLAYS_PER_PAGE; i++) {
		widget& widget{m_ui.get(REPLAY_TAGS[i])};
		widget.pos.change(interp_mode::CUBE, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.25_s);
		widget.hide(0.25_s);
	}
}

void replays_state::set_up_exit_animation()
{
	m_ui.get(TAG_TITLE).pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	m_ui.get(TAG_EXIT).pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	if (m_replays.empty()) {
		m_ui.get(TAG_NO_REPLAYS_FOUND).pos.change(interp_mode::CUBE, {400, 467}, 0.5_s);
	}
	else {
		for (std::size_t i = 0; i < REPLAYS_PER_PAGE; i++) {
			widget& widget{m_ui.get(REPLAY_TAGS[i])};
			widget.pos.change(interp_mode::CUBE, {i % 2 == 0 ? 600 : 400, glm::vec2{widget.pos}.y}, 0.5_s);
		}
		m_ui.get(TAG_CUR_PAGE).pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
		m_ui.get(TAG_PAGE_DEC).pos.change(interp_mode::CUBE, {-50, 942.5}, 0.5_s);
		m_ui.get(TAG_PAGE_INC).pos.change(interp_mode::CUBE, {1050, 942.5}, 0.5_s);
	}
	m_ui.hide_all(0.5_s);
}