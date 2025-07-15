#include "../../include/state/start_game_state.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Gamemode display widgets.
constexpr std::array<const char*, 4> GAMEMODE_WIDGETS{"name", "author", "description", "pb"};
// Shortcuts of the previous gamemode button.
constexpr std::initializer_list<tr::key_chord> PREV_CHORDS{{tr::keycode::LEFT}};
// Shortcuts of the next gamemode button.
constexpr std::initializer_list<tr::key_chord> NEXT_CHORDS{{tr::keycode::RIGHT}};
// Shortcuts of the start button.
constexpr std::initializer_list<tr::key_chord> START_CHORDS{{tr::keycode::ENTER}, {tr::keycode::TOP_ROW_1}};
// Shortcuts of the exit button.
constexpr std::initializer_list<tr::key_chord> EXIT_CHORDS{
	{tr::keycode::ESCAPE},
	{tr::keycode::Q},
	{tr::keycode::E},
	{tr::keycode::TOP_ROW_2},
};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

start_game_state::start_game_state(std::unique_ptr<game>&& game) noexcept
	: _substate{substate::IN_START_GAME}, _timer{0}, _game{std::move(game)}, _gamemodes{load_gamemodes()}, _cur{_gamemodes.begin()}
{
	std::vector<gamemode>::iterator last_selected_it{std::ranges::find(_gamemodes, scorefile.last_selected_gamemode)};
	if (last_selected_it != _gamemodes.end()) {
		_cur = last_selected_it;
	}

	const status_callback status_cb{[this] { return _substate == substate::IN_START_GAME; }};

	widget& title{_ui.emplace<text_widget>("start_game", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	text_callback name_text_cb{[name = std::string{_cur->name_loc()}](auto&) { return name; }};
	widget& name{_ui.emplace<text_widget>("name", glm::vec2{500, 275}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 120,
										  std::move(name_text_cb))};
	name.pos.change({500, 375}, 0.5_s);
	name.unhide(0.5_s);

	text_callback author_text_cb{[str = std::format("{}: {}", localization["by"], _cur->author)](auto&) { return str; }};
	widget& author{_ui.emplace<text_widget>("author", glm::vec2{400, 450}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 32,
											std::move(author_text_cb))};
	author.pos.change({500, 450}, 0.5_s);
	author.unhide(0.5_s);

	text_callback description_text_cb{[desc = std::string{_cur->description_loc()}](auto&) { return desc; }};
	widget& description{_ui.emplace<text_widget>("description", glm::vec2{600, 500}, tr::align::CENTER, font::LANGUAGE,
												 tr::ttf_style::ITALIC, 32, std::move(description_text_cb), "80808080"_rgba8)};
	description.pos.change({500, 500}, 0.5_s);
	description.unhide(0.5_s);

	text_callback pb_text_cb{
		[pb = std::format("{}:\n{}", localization["pb"], timer_text(scorefile.category_pb(*_cur)))](const auto&) { return pb; }};
	widget& pb{_ui.emplace<text_widget>("pb", glm::vec2{500, 695}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 48,
										std::move(pb_text_cb), "FFFF00C0"_rgba8)};
	pb.pos.change({500, 595}, 0.5_s);
	pb.unhide(0.5_s);

	const action_callback prev_action_cb{[this] {
		_cur = _cur == _gamemodes.begin() ? _cur = _gamemodes.end() - 1 : std::prev(_cur);
		_substate = substate::SWITCHING_GAMEMODE;
		_timer = 0;
		for (const char* tag : GAMEMODE_WIDGETS) {
			widget& widget{_ui.get(tag)};
			widget.pos.change({750, glm::vec2{widget.pos}.y}, 0.25_s);
			widget.hide(0.25_s);
		}
	}};
	widget& prev{
		_ui.emplace<arrow_widget>("prev", glm::vec2{-100, 500}, tr::align::CENTER_LEFT, false, status_cb, prev_action_cb, PREV_CHORDS)};
	prev.pos.change({10, 500}, 0.5_s);
	prev.unhide(0.5_s);

	const action_callback next_action_cb{[this] {
		if (++_cur == _gamemodes.end()) {
			_cur = _gamemodes.begin();
		}
		_substate = substate::SWITCHING_GAMEMODE;
		_timer = 0;
		for (const char* tag : GAMEMODE_WIDGETS) {
			widget& widget{_ui.get(tag)};
			widget.pos.change({250, glm::vec2{widget.pos}.y}, 0.25_s);
			widget.hide(0.25_s);
		}
	}};
	widget& next{
		_ui.emplace<arrow_widget>("next", glm::vec2{1100, 500}, tr::align::CENTER_RIGHT, true, status_cb, next_action_cb, NEXT_CHORDS)};
	next.pos.change({990, 500}, 0.5_s);
	next.unhide(0.5_s);

	const action_callback start_action_cb{[this] {
		_substate = substate::ENTERING_GAME;
		_timer = 0;
		set_up_exit_animation();
		scorefile.last_selected_gamemode = *_cur;
	}};
	widget& start{_ui.emplace<clickable_text_widget>("start", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													 DEFAULT_TEXT_CALLBACK, status_cb, start_action_cb, NO_TOOLTIP, START_CHORDS)};
	start.pos.change({500, 950}, 0.5_s);
	start.unhide(0.5_s);

	const action_callback exit_action_cb{[this] {
		_substate = substate::ENTERING_TITLE;
		_timer = 0;
		set_up_exit_animation();
		scorefile.last_selected_gamemode = *_cur;
	}};
	widget& exit{_ui.emplace<clickable_text_widget>("exit", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													DEFAULT_TEXT_CALLBACK, status_cb, exit_action_cb, NO_TOOLTIP, EXIT_CHORDS,
													sfx::CANCEL)};
	exit.pos.change({500, 1000}, 0.5_s);
	exit.unhide(0.5_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> start_game_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> start_game_state::update(tr::duration)
{
	++_timer;
	_game->update({});
	_ui.update();

	switch (_substate) {
	case substate::IN_START_GAME:
		return nullptr;
	case substate::SWITCHING_GAMEMODE:
		if (_timer >= 0.5_s) {
			_substate = substate::IN_START_GAME;
			_timer = 0;
		}
		else if (_timer == 0.25_s) {
			std::array<text_callback, GAMEMODE_WIDGETS.size()> new_cbs{
				[name = std::string{_cur->name_loc()}](auto&) { return name; },
				[author = std::format("{}: {}", localization["by"], _cur->author)](auto&) { return author; },
				[desc = std::string{_cur->description_loc()}](auto&) { return desc; },
				[pb = std::format("{}:\n{}", localization["pb"], timer_text(scorefile.category_pb(*_cur)))](auto&) { return pb; },
			};
			for (std::size_t i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
				text_widget& widget{_ui.get<text_widget>(GAMEMODE_WIDGETS[i])};
				const glm::vec2 old_pos{widget.pos};
				widget.text_cb = std::move(new_cbs[i]);
				widget.pos = glm::vec2{old_pos.x < 500 ? 600 : 400, old_pos.y};
				widget.pos.change({500, old_pos.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
	case substate::ENTERING_TITLE:
		return _timer >= 0.5_s ? std::make_unique<title_state>(std::move(_game)) : nullptr;
	case substate::ENTERING_GAME:
		return _timer >= 0.5_s ? std::make_unique<game_state>(std::make_unique<active_game>(*_cur), game_type::REGULAR, true) : nullptr;
	}
}

void start_game_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(_substate == substate::ENTERING_GAME ? _timer / 0.5_sf : 0);
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void start_game_state::set_up_exit_animation() noexcept
{

	widget& name{_ui.get("name")};
	widget& author{_ui.get("author")};
	widget& description{_ui.get("description")};
	widget& pb{_ui.get("pb")};
	name.pos.change(glm::vec2{name.pos} - glm::vec2{0, 100}, 0.5_s);
	author.pos.change(glm::vec2{author.pos} + glm::vec2{100, 0}, 0.5_s);
	description.pos.change(glm::vec2{description.pos} - glm::vec2{100, 0}, 0.5_s);
	pb.pos.change(glm::vec2{pb.pos} + glm::vec2{0, 100}, 0.5_s);
	_ui.get("start_game").pos.change(TOP_START_POS, 0.5_s);
	_ui.get("start").pos.change(BOTTOM_START_POS, 0.5_s);
	_ui.get("exit").pos.change(BOTTOM_START_POS, 0.5_s);
	_ui.get("prev").pos.change({-100, 500}, 0.5_s);
	_ui.get("next").pos.change({1100, 500}, 0.5_s);
	_ui.hide_all(0.5_s);
}