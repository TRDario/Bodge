#include "../../include/state/start_game_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag TAG_TITLE{"start_game"};
constexpr tag TAG_NAME{"name"};
constexpr tag TAG_AUTHOR{"author"};
constexpr tag TAG_DESCRIPTION{"description"};
constexpr tag TAG_PB{"pb"};
constexpr tag TAG_PREV{"prev"};
constexpr tag TAG_NEXT{"next"};
constexpr tag TAG_START{"start"};
constexpr tag TAG_EXIT{"exit"};

// Gamemode display widgets.
constexpr std::array<const char*, 4> GAMEMODE_WIDGETS{TAG_NAME, TAG_AUTHOR, TAG_DESCRIPTION, TAG_PB};
// Shortcuts of the previous gamemode button.
constexpr std::initializer_list<tr::system::key_chord> PREV_CHORDS{{tr::system::keycode::LEFT}};
// Shortcuts of the next gamemode button.
constexpr std::initializer_list<tr::system::key_chord> NEXT_CHORDS{{tr::system::keycode::RIGHT}};
// Shortcuts of the start button.
constexpr std::initializer_list<tr::system::key_chord> START_CHORDS{{tr::system::keycode::ENTER}, {tr::system::keycode::TOP_ROW_1}};
// Shortcuts of the exit button.
constexpr std::initializer_list<tr::system::key_chord> EXIT_CHORDS{
	{tr::system::keycode::ESCAPE},
	{tr::system::keycode::Q},
	{tr::system::keycode::E},
	{tr::system::keycode::TOP_ROW_2},
};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

start_game_state::start_game_state(std::unique_ptr<game>&& game)
	: m_substate{substate::IN_START_GAME}
	, m_timer{0}
	, m_background_game{std::move(game)}
	, m_gamemodes{load_gamemodes()}
	, m_selected{m_gamemodes.begin()}
{
	std::vector<gamemode>::iterator last_selected_it{std::ranges::find(m_gamemodes, engine::scorefile.last_selected)};
	if (last_selected_it != m_gamemodes.end()) {
		m_selected = last_selected_it;
	}

	const status_callback status_cb{[this] { return m_substate == substate::IN_START_GAME; }};

	widget& title{
		m_ui.emplace<text_widget>(TAG_TITLE, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL, 64)};
	title.pos.change(interp_mode::CUBE, {500, 0}, 0.5_s);
	title.unhide(0.5_s);

	text_callback name_text_cb{[name = std::string{::name(*m_selected)}](auto&) { return name; }};
	widget& name{m_ui.emplace<text_widget>(TAG_NAME, glm::vec2{500, 275}, tr::align::CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL,
										   120, std::move(name_text_cb))};
	name.pos.change(interp_mode::CUBE, {500, 375}, 0.5_s);
	name.unhide(0.5_s);

	text_callback author_text_cb{[str = std::format("{}: {}", engine::loc["by"], m_selected->author)](auto&) { return str; }};
	widget& author{m_ui.emplace<text_widget>(TAG_AUTHOR, glm::vec2{400, 450}, tr::align::CENTER, font::LANGUAGE,
											 tr::system::ttf_style::NORMAL, 32, std::move(author_text_cb))};
	author.pos.change(interp_mode::CUBE, {500, 450}, 0.5_s);
	author.unhide(0.5_s);

	text_callback description_text_cb{[desc = std::string{description(*m_selected)}](auto&) { return desc; }};
	widget& description{m_ui.emplace<text_widget>(TAG_DESCRIPTION, glm::vec2{600, 500}, tr::align::CENTER, font::LANGUAGE,
												  tr::system::ttf_style::ITALIC, 32, std::move(description_text_cb), "80808080"_rgba8)};
	description.pos.change(interp_mode::CUBE, {500, 500}, 0.5_s);
	description.unhide(0.5_s);

	text_callback pb_text_cb{
		[pb = std::format("{}:\n{}", engine::loc["pb"], timer_text(pb(engine::scorefile, *m_selected)))](const auto&) { return pb; }};
	widget& pb{m_ui.emplace<text_widget>(TAG_PB, glm::vec2{500, 695}, tr::align::CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL, 48,
										 std::move(pb_text_cb), "FFFF00C0"_rgba8)};
	pb.pos.change(interp_mode::CUBE, {500, 595}, 0.5_s);
	pb.unhide(0.5_s);

	const action_callback prev_action_cb{[this] {
		m_selected = m_selected == m_gamemodes.begin() ? m_selected = m_gamemodes.end() - 1 : std::prev(m_selected);
		m_substate = substate::SWITCHING_GAMEMODE;
		m_timer = 0;
		for (const char* tag : GAMEMODE_WIDGETS) {
			widget& widget{m_ui.get(tag)};
			widget.pos.change(interp_mode::CUBE, {750, glm::vec2{widget.pos}.y}, 0.25_s);
			widget.hide(0.25_s);
		}
	}};
	widget& prev{
		m_ui.emplace<arrow_widget>(TAG_PREV, glm::vec2{-100, 500}, tr::align::CENTER_LEFT, false, status_cb, prev_action_cb, PREV_CHORDS)};
	prev.pos.change(interp_mode::CUBE, {10, 500}, 0.5_s);
	prev.unhide(0.5_s);

	const action_callback next_action_cb{[this] {
		if (++m_selected == m_gamemodes.end()) {
			m_selected = m_gamemodes.begin();
		}
		m_substate = substate::SWITCHING_GAMEMODE;
		m_timer = 0;
		for (const char* tag : GAMEMODE_WIDGETS) {
			widget& widget{m_ui.get(tag)};
			widget.pos.change(interp_mode::CUBE, {250, glm::vec2{widget.pos}.y}, 0.25_s);
			widget.hide(0.25_s);
		}
	}};
	widget& next{
		m_ui.emplace<arrow_widget>(TAG_NEXT, glm::vec2{1100, 500}, tr::align::CENTER_RIGHT, true, status_cb, next_action_cb, NEXT_CHORDS)};
	next.pos.change(interp_mode::CUBE, {990, 500}, 0.5_s);
	next.unhide(0.5_s);

	const action_callback start_action_cb{[this] {
		m_substate = substate::ENTERING_GAME;
		m_timer = 0;
		set_up_exit_animation();
		engine::scorefile.last_selected = *m_selected;
		engine::fade_song_out(0.5s);
	}};
	widget& start{m_ui.emplace<clickable_text_widget>(TAG_START, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													  DEFAULT_TEXT_CALLBACK, status_cb, start_action_cb, NO_TOOLTIP, START_CHORDS)};
	start.pos.change(interp_mode::CUBE, {500, 950}, 0.5_s);
	start.unhide(0.5_s);

	const action_callback exit_action_cb{[this] {
		m_substate = substate::ENTERING_TITLE;
		m_timer = 0;
		set_up_exit_animation();
		engine::scorefile.last_selected = *m_selected;
	}};
	widget& exit{m_ui.emplace<clickable_text_widget>(TAG_EXIT, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													 DEFAULT_TEXT_CALLBACK, status_cb, exit_action_cb, NO_TOOLTIP, EXIT_CHORDS,
													 sound::CANCEL)};
	exit.pos.change(interp_mode::CUBE, {500, 1000}, 0.5_s);
	exit.unhide(0.5_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> start_game_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> start_game_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

	switch (m_substate) {
	case substate::IN_START_GAME:
		return nullptr;
	case substate::SWITCHING_GAMEMODE:
		if (m_timer >= 0.5_s) {
			m_substate = substate::IN_START_GAME;
			m_timer = 0;
		}
		else if (m_timer == 0.25_s) {
			std::array<text_callback, GAMEMODE_WIDGETS.size()> new_cbs{
				[name = std::string{::name(*m_selected)}](auto&) { return name; },
				[author = std::format("{}: {}", engine::loc["by"], m_selected->author)](auto&) { return author; },
				[desc = std::string{description(*m_selected)}](auto&) { return desc; },
				[pb = std::format("{}:\n{}", engine::loc["pb"], timer_text(pb(engine::scorefile, *m_selected)))](auto&) { return pb; },
			};
			for (std::size_t i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
				text_widget& widget{m_ui.get<text_widget>(GAMEMODE_WIDGETS[i])};
				const glm::vec2 old_pos{widget.pos};
				widget.text_cb = std::move(new_cbs[i]);
				widget.pos = glm::vec2{old_pos.x < 500 ? 600 : 400, old_pos.y};
				widget.pos.change(interp_mode::CUBE, {500, old_pos.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
	case substate::ENTERING_TITLE:
		return m_timer >= 0.5_s ? std::make_unique<title_state>(std::move(m_background_game)) : nullptr;
	case substate::ENTERING_GAME:
		return m_timer >= 0.5_s ? std::make_unique<game_state>(std::make_unique<active_game>(*m_selected), game_type::REGULAR, true)
								: nullptr;
	}
}

void start_game_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(m_substate == substate::ENTERING_GAME ? m_timer / 0.5_sf : 0);
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void start_game_state::set_up_exit_animation()
{

	widget& name{m_ui.get(TAG_NAME)};
	widget& author{m_ui.get(TAG_AUTHOR)};
	widget& description{m_ui.get(TAG_DESCRIPTION)};
	widget& pb{m_ui.get(TAG_PB)};
	name.pos.change(interp_mode::CUBE, glm::vec2{name.pos} - glm::vec2{0, 100}, 0.5_s);
	author.pos.change(interp_mode::CUBE, glm::vec2{author.pos} + glm::vec2{100, 0}, 0.5_s);
	description.pos.change(interp_mode::CUBE, glm::vec2{description.pos} - glm::vec2{100, 0}, 0.5_s);
	pb.pos.change(interp_mode::CUBE, glm::vec2{pb.pos} + glm::vec2{0, 100}, 0.5_s);
	m_ui.get(TAG_TITLE).pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	m_ui.get(TAG_PREV).pos.change(interp_mode::CUBE, {-100, 500}, 0.5_s);
	m_ui.get(TAG_NEXT).pos.change(interp_mode::CUBE, {1100, 500}, 0.5_s);
	m_ui.get(TAG_START).pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	m_ui.get(TAG_EXIT).pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all(0.5_s);
}