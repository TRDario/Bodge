#include "../../include/state/start_game_state.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr array<const char*, 4> GAMEMODE_WIDGETS{"name", "author", "description", "pb"};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

start_game_state::start_game_state(game&& game) noexcept
	: _substate{substate::IN_START_GAME}, _timer{0}, _game{std::move(game)}, _gamemodes{load_gamemodes()}, _cur{_gamemodes.begin()}
{
	const status_callback STATUS_CB{[this] { return _substate == substate::IN_START_GAME; }};
	constexpr rgba8 DESC_COLOR{128, 128, 128, 128};
	constexpr rgba8 PB_COLOR{255, 255, 0, 192};

	basic_text_widget& title{_ui.emplace<basic_text_widget>("start_game", vec2{500, -50}, TOP_CENTER, font::LANGUAGE, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	text_callback name_text_cb{[name = _cur->name](const string&) { return name; }};
	basic_text_widget& name{_ui.emplace<basic_text_widget>("name", vec2{500, 275}, CENTER, font::LANGUAGE, 120, name_text_cb)};
	name.pos.change({500, 375}, 0.5_s);
	name.unhide(0.5_s);

	text_callback author_text_cb{[author = format("{}{}", localization["by"], _cur->author)](const string&) { return author; }};
	basic_text_widget& author{_ui.emplace<basic_text_widget>("author", vec2{400, 450}, CENTER, font::LANGUAGE, 32, author_text_cb)};
	author.pos.change({500, 450}, 0.5_s);
	author.unhide(0.5_s);

	string description_str{_cur->description.empty() ? localization["no_description"] : _cur->description};
	text_callback description_text_cb{[desc = std::move(description_str)](const string&) { return desc; }};
	basic_text_widget& description{
		_ui.emplace<basic_text_widget>("description", vec2{600, 500}, CENTER, font::LANGUAGE, 32, description_text_cb, DESC_COLOR, ITALIC)};
	description.pos.change({500, 500}, 0.5_s);
	description.unhide(0.5_s);

	const ticks pb_time{scorefile.scores.contains(*_cur) ? scorefile.scores.at(*_cur).front().result : 0};
	string pb_str{format("{}{}:{:02}:{:02}", localization["pb"], pb_time / 60_s, (pb_time % 60_s) / 1_s, (pb_time % 1_s) * 100 / 1_s)};
	text_callback pb_text_cb{[pb = std::move(pb_str)](const string&) { return pb; }};
	basic_text_widget& pb{_ui.emplace<basic_text_widget>("pb", vec2{500, 695}, CENTER, font::LANGUAGE, 48, pb_text_cb, PB_COLOR)};
	pb.pos.change({500, 595}, 0.5_s);
	pb.unhide(0.5_s);

	action_callback prev_action_cb{[this] {
		if (_cur == _gamemodes.begin()) {
			_cur = _gamemodes.end() - 1;
		}
		else {
			--_cur;
		}

		_substate = substate::SWITCHING_GAMEMODE;
		_timer = 0;
		for (const char* tag : GAMEMODE_WIDGETS) {
			widget& widget{_ui.get(tag)};
			widget.pos.change({750, vec2{widget.pos}.y}, 0.25_s);
			widget.hide(0.25_s);
		}
	}};
	arrow_widget& prev{
		_ui.emplace<arrow_widget>("prev", vec2{-100, 500}, CENTER_LEFT, false, STATUS_CB, prev_action_cb, vector<key_chord>{{key::LEFT}})};
	prev.pos.change({10, 500}, 0.5_s);
	prev.unhide(0.5_s);

	action_callback next_action_cb{[this] {
		if (++_cur == _gamemodes.end()) {
			_cur = _gamemodes.begin();
		}

		_substate = substate::SWITCHING_GAMEMODE;
		_timer = 0;
		for (const char* tag : GAMEMODE_WIDGETS) {
			widget& widget{_ui.get(tag)};
			widget.pos.change({250, vec2{widget.pos}.y}, 0.25_s);
			widget.hide(0.25_s);
		}
	}};
	arrow_widget& next{
		_ui.emplace<arrow_widget>("next", vec2{1100, 500}, CENTER_RIGHT, true, STATUS_CB, next_action_cb, vector<key_chord>{{key::RIGHT}})};
	next.pos.change({990, 500}, 0.5_s);
	next.unhide(0.5_s);

	action_callback start_action_cb{[this] {
		_substate = substate::ENTERING_GAME;
		_timer = 0;
		set_up_exit_animation();
	}};
	clickable_text_widget& start{_ui.emplace<clickable_text_widget>("start", vec2{500, 1050}, BOTTOM_CENTER, 48, STATUS_CB, start_action_cb,
																	vector<key_chord>{{key::ENTER}, {key::TOP_ROW_1}})};
	start.pos.change({500, 950}, 0.5_s);
	start.unhide(0.5_s);

	action_callback exit_action_cb{[this] {
		_substate = substate::ENTERING_TITLE;
		_timer = 0;
		set_up_exit_animation();
	}};
	clickable_text_widget& exit{_ui.emplace<clickable_text_widget>("exit", vec2{500, 1050}, BOTTOM_CENTER, 48, STATUS_CB, exit_action_cb,
																   vector<key_chord>{{key::ESCAPE}, {key::Q}, {key::E}, {key::TOP_ROW_2}})};
	exit.pos.change({500, 1000}, 0.5_s);
	exit.unhide(0.5_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

u32 start_game_state::type() const noexcept
{
	return ID;
}

unique_ptr<state> start_game_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

unique_ptr<state> start_game_state::update(tr::duration)
{
	++_timer;
	_game.update({});
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
			const ticks pb{scorefile.scores.contains(*_cur) ? scorefile.scores.at(*_cur).front().result : 0};
			string author_str{format("{}{}", localization["by"], _cur->author)};
			string desc_str{_cur->description.empty() ? localization["no_description"] : _cur->description};
			string pb_str{format("{}{}:{:02}:{:02}", localization["pb"], pb / 60_s, (pb % 60_s) / 1_s, (pb % 1_s) * 100 / 1_s)};
			array<text_callback, GAMEMODE_WIDGETS.size()> NEW_CBS{
				[name = _cur->name](const string&) { return name; },
				[author = std::move(author_str)](const string&) { return author; },
				[desc = std::move(desc_str)](const string&) { return desc; },
				[pb = std::move(pb_str)](const string&) { return pb; },
			};
			for (int i = 0; i < GAMEMODE_WIDGETS.size(); ++i) {
				basic_text_widget& widget{_ui.get<basic_text_widget>(GAMEMODE_WIDGETS[i])};
				const vec2 old_pos{widget.pos};
				widget.set_text_callback(NEW_CBS[i]);
				widget.pos = vec2{old_pos.x < 500 ? 750 : 250, old_pos.y};
				widget.pos.change({500, old_pos.y}, 0.25_s);
				widget.unhide(0.25_s);
			}
		}
	case substate::ENTERING_TITLE:
		return _timer >= 0.5_s ? make_unique<title_state>(std::move(_game)) : nullptr;
	case substate::ENTERING_GAME:
		return _timer >= 0.5_s ? make_unique<game_state>(active_game{*_cur}, true) : nullptr;
	}
}

void start_game_state::draw()
{
	_game.add_to_renderer();
	engine::layered_renderer().add_color_quad(layer::GAME_OVERLAY, MENU_GAME_OVERLAY_QUAD);
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(_substate == substate::ENTERING_GAME ? _timer / 0.5_sf : 0);

	engine::layered_renderer().draw_up_to_layer(layer::UI, engine::screen());
	engine::batched_renderer().draw(engine::screen());
	engine::layered_renderer().draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void start_game_state::set_up_exit_animation() noexcept
{

	widget& name{_ui.get("name")};
	widget& author{_ui.get("author")};
	widget& description{_ui.get("description")};
	widget& pb{_ui.get("pb")};
	name.pos.change(vec2{name.pos} - vec2{0, 100}, 0.5_s);
	author.pos.change(vec2{author.pos} - vec2{100, 0}, 0.5_s);
	description.pos.change(vec2{description.pos} + vec2{100, 0}, 0.5_s);
	pb.pos.change(vec2{pb.pos} + vec2{0, 100}, 0.5_s);
	_ui.get("start_game").pos.change({500, -50}, 0.5_s);
	_ui.get("start").pos.change({500, 1050}, 0.5_s);
	_ui.get("exit").pos.change({500, 1050}, 0.5_s);
	_ui.get("prev").pos.change({-100, 500}, 0.5_s);
	_ui.get("next").pos.change({1100, 500}, 0.5_s);
	_ui.hide_all(0.5_s);
}