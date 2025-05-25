#include "../../include/state/title_state.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/replays_state.hpp"
#include "../../include/state/settings_state.hpp"
#include "../../include/state/start_game_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Title screen buttons.
constexpr array<const char*, 5> BUTTONS{"start_game", "replays", "scores", "settings", "exit"};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

title_state::title_state()
	: _substate{substate::ENTERING_GAME}, _timer{0}, _game{MENU_GAMEMODES[rand(rng, MENU_GAMEMODES.size())], rand<std::uint64_t>(rng)}
{
	set_up_ui();
}

title_state::title_state(game&& game)
	: _substate{substate::IN_TITLE}, _timer{0}, _game{std::move(game)}
{
	set_up_ui();
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

u32 title_state::type() const noexcept
{
	return ID;
}

unique_ptr<state> title_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

unique_ptr<state> title_state::update(tr::duration)
{
	++_timer;
	_game.update({});
	_ui.update();

	switch (_substate) {
	case substate::ENTERING_GAME:
		if (_timer >= 1.0_s) {
			_timer = 0;
			_substate = substate::IN_TITLE;
		}
		return nullptr;
	case substate::IN_TITLE:
		return nullptr;
	case substate::ENTERING_START_GAME:
		return _timer >= 0.5_s ? make_unique<start_game_state>(std::move(_game)) : nullptr;
	case substate::ENTERING_REPLAYS:
		return _timer >= 0.5_s ? make_unique<replays_state>(std::move(_game)) : nullptr;
	case substate::ENTERING_SETTINGS:
		return _timer >= 0.5_s ? make_unique<settings_state>(std::move(_game)) : nullptr;
	case substate::EXITING_GAME:
		return _timer >= 0.5_s ? make_unique<tr::drop_state>() : nullptr;
	}
}

void title_state::draw()
{
	_game.add_to_renderer();
	engine::layered_renderer().add_color_quad(layer::GAME_OVERLAY, MENU_GAME_OVERLAY_QUAD);
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(fade_overlay_opacity());

	engine::layered_renderer().draw_up_to_layer(layer::UI, engine::screen());
	engine::batched_renderer().draw(engine::screen());
	engine::layered_renderer().draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float title_state::fade_overlay_opacity() const noexcept
{
	switch (_substate) {
	case substate::ENTERING_GAME:
		return 1 - _timer / 1_sf;
	case substate::IN_TITLE:
	case substate::ENTERING_SETTINGS:
	case substate::ENTERING_START_GAME:
	case substate::ENTERING_REPLAYS:
		return 0;
	case substate::EXITING_GAME:
		return _timer / 0.5_sf;
	}
}

void title_state::set_up_ui()
{
	basic_text_widget& copyright{_ui.emplace<basic_text_widget>("copyright", vec2{4, 1000}, TOP_LEFT, font::DEFAULT, 24)};
	copyright.pos.change({4, 998 - copyright.size().y}, 1_s);
	copyright.unhide(1_s);
	basic_text_widget& version{_ui.emplace<basic_text_widget>("version", vec2{996, 1000}, TOP_RIGHT, font::DEFAULT, 24)};
	version.pos.change({996, 998 - version.size().y}, 1_s);
	version.unhide(1_s);

	array<vector<key_chord>, BUTTONS.size()> CHORDS{
		vector<key_chord>{{key::ENTER}, {key::TOP_ROW_1}},
		vector<key_chord>{{key::R}, {key::TOP_ROW_2}},
		vector<key_chord>{{key::S}, {key::TOP_ROW_3}},
		vector<key_chord>{{key::C}, {key::TOP_ROW_4}},
		vector<key_chord>{{key::ESCAPE}, {key::Q}, {key::E}, {key::TOP_ROW_5}},
	};
	const status_callback STATUS_CB{[this] { return _substate == substate::IN_TITLE || _substate == substate::ENTERING_GAME; }};
	array<action_callback, BUTTONS.size()> ACTION_CBS{
		[this] {
			_substate = substate::ENTERING_START_GAME;
			_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			_substate = substate::ENTERING_REPLAYS;
			_timer = 0;
			set_up_exit_animation();
		},
		[] {},
		[this] {
			_substate = substate::ENTERING_SETTINGS;
			_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			_substate = substate::EXITING_GAME;
			_timer = 0;
			set_up_exit_animation();
		},
	};

	vec2 end_pos{990, 965 - (BUTTONS.size() - 1) * 50};
	for (int i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * rand(rng, 35.0f, 75.0f)};
		const vec2 pos{end_pos.x + offset, end_pos.y};
		clickable_text_widget& widget{_ui.emplace<clickable_text_widget>(BUTTONS[i], pos, CENTER_RIGHT, 48, STATUS_CB,
																		 std::move(ACTION_CBS[i]), std::move(CHORDS[i]))};
		widget.pos.change(end_pos, 1_s);
		widget.unhide(1_s);
		end_pos += vec2{-25, 50};
	}
}

void title_state::set_up_exit_animation() noexcept
{
	int i = 0;
	for (const char* tag : BUTTONS) {
		const float offset{(i++ % 2 != 0 ? -1.0f : 1.0f) * rand(rng, 35.0f, 75.0f)};
		widget& widget{_ui.get(tag)};
		widget.pos.change(vec2{widget.pos} + vec2{offset, 0}, 0.5_s);
	}
	_ui.get("copyright").pos.change({4, 1000}, 0.5_s);
	_ui.get("version").pos.change({996, 1000}, 0.5_s);
	_ui.hide_all(0.5_s);
}