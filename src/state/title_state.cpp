#include "../../include/state/title_state.hpp"
#include "../../include/audio.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/replays_state.hpp"
#include "../../include/state/scoreboards_state.hpp"
#include "../../include/state/settings_state.hpp"
#include "../../include/state/start_game_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Title screen buttons.
constexpr std::array<const char*, 7> BUTTONS{"start_game", "gamemode_designer", "scoreboards", "replays", "settings", "credits", "exit"};
// Shortcuts of the title screen buttons.
constexpr std::array<std::initializer_list<tr::key_chord>, BUTTONS.size()> SHORTCUTS{{
	{{tr::keycode::ENTER}, {tr::keycode::TOP_ROW_1}},
	{{tr::keycode::G}, {tr::keycode::TOP_ROW_2}},
	{{tr::keycode::B}, {tr::keycode::TOP_ROW_3}},
	{{tr::keycode::R}, {tr::keycode::TOP_ROW_4}},
	{{tr::keycode::S}, {tr::keycode::TOP_ROW_5}},
	{{tr::keycode::C}, {tr::keycode::TOP_ROW_6}},
	{{tr::keycode::ESCAPE}, {tr::keycode::Q}, {tr::keycode::E}, {tr::keycode::TOP_ROW_7}},
}};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

title_state::title_state()
	: _substate{substate::ENTERING_GAME}, _timer{0}, _game{std::make_unique<game>(pick_menu_gamemode(), tr::rand<std::uint64_t>(rng))}
{
	set_up_ui();
}

title_state::title_state(std::unique_ptr<game>&& game)
	: _substate{substate::IN_TITLE}, _timer{0}, _game{std::move(game)}
{
	set_up_ui();
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> title_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> title_state::update(tr::duration)
{
	++_timer;
	_game->update({});
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
		return _timer >= 0.5_s ? std::make_unique<start_game_state>(std::move(_game)) : nullptr;
	case substate::ENTERING_gamemode_designer:
		return _timer >= 0.5_s ? std::make_unique<gamemode_designer_state>(std::move(_game), gamemode{}, false) : nullptr;
	case substate::ENTERING_SCOREBOARDS:
		return _timer >= 0.5_s ? std::make_unique<scoreboards_state>(std::move(_game)) : nullptr;
	case substate::ENTERING_REPLAYS:
		return _timer >= 0.5_s ? std::make_unique<replays_state>(std::move(_game)) : nullptr;
	case substate::ENTERING_SETTINGS:
		return _timer >= 0.5_s ? std::make_unique<settings_state>(std::move(_game)) : nullptr;
	case substate::EXITING_GAME:
		return _timer >= 0.5_s ? std::make_unique<tr::drop_state>() : nullptr;
	}
}

void title_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float title_state::fade_overlay_opacity() const noexcept
{
	switch (_substate) {
	case substate::ENTERING_GAME:
		return 1 - _timer / 1_sf;
	case substate::IN_TITLE:
	case substate::ENTERING_START_GAME:
	case substate::ENTERING_gamemode_designer:
	case substate::ENTERING_REPLAYS:
	case substate::ENTERING_SCOREBOARDS:
	case substate::ENTERING_SETTINGS:
		return 0;
	case substate::EXITING_GAME:
		return _timer / 0.5_sf;
	}
}

void title_state::set_up_ui()
{
	widget& logo_text{_ui.emplace<image_widget>("logo_text", glm::vec2{500, 100}, tr::align::CENTER)};
	logo_text.pos.change({500, 160}, 2.5_s);
	logo_text.unhide(2.5_s);
	widget& logo_overlay{_ui.emplace<image_widget>("logo_overlay", glm::vec2{500, 100}, tr::align::CENTER, &settings.primary_hue)};
	logo_overlay.pos.change({500, 160}, 2.5_s);
	logo_overlay.unhide(2.5_s);
	widget& logo_ball{_ui.emplace<image_widget>("logo_ball", glm::vec2{-180, 644}, tr::align::CENTER, &settings.secondary_hue)};
	logo_ball.pos.change({327, 217}, 2.5_s);
	logo_ball.unhide(2.5_s);

	widget& copyright{
		_ui.emplace<text_widget>("copyright", glm::vec2{4, 1000}, tr::align::TOP_LEFT, font::DEFAULT, tr::ttf_style::NORMAL, 24)};
	copyright.pos.change({4, 998 - copyright.size().y}, 1_s);
	copyright.unhide(1_s);
	widget& version{
		_ui.emplace<text_widget>("version", glm::vec2{996, 1000}, tr::align::TOP_RIGHT, font::DEFAULT, tr::ttf_style::NORMAL, 24)};
	version.pos.change({996, 998 - version.size().y}, 1_s);
	version.unhide(1_s);

	const status_callback status_cb{[this] { return _substate == substate::IN_TITLE || _substate == substate::ENTERING_GAME; }};
	const std::array<action_callback, BUTTONS.size()> action_cbs{
		[this] {
			_substate = substate::ENTERING_START_GAME;
			_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			_substate = substate::ENTERING_gamemode_designer;
			_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			_substate = substate::ENTERING_SCOREBOARDS;
			_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			_substate = substate::ENTERING_REPLAYS;
			_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			_substate = substate::ENTERING_SETTINGS;
			_timer = 0;
			set_up_exit_animation();
		},
		[] { audio::play(sfx::CONFIRM, 0.5f, 0.0f); },
		[this] {
			_substate = substate::EXITING_GAME;
			_timer = 0;
			set_up_exit_animation();
		},
	};

	glm::vec2 end_pos{990, 965 - (BUTTONS.size() - 1) * 50};
	for (std::size_t i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * tr::rand(rng, 35.0f, 75.0f)};
		const glm::vec2 pos{end_pos.x + offset, end_pos.y};
		widget& widget{_ui.emplace<clickable_text_widget>(BUTTONS[i], pos, tr::align::CENTER_RIGHT, font::LANGUAGE, 48,
														  DEFAULT_TEXT_CALLBACK, status_cb, action_cbs[i], NO_TOOLTIP, SHORTCUTS[i],
														  i != BUTTONS.size() - 1 ? sfx::CONFIRM : sfx::CANCEL)};
		widget.pos.change(end_pos, 1_s);
		widget.unhide(1_s);
		end_pos += glm::vec2{-25, 50};
	}
}

void title_state::set_up_exit_animation() noexcept
{
	int i = 0;
	for (const char* tag : BUTTONS) {
		const float offset{(i++ % 2 != 0 ? -1.0f : 1.0f) * tr::rand(rng, 35.0f, 75.0f)};
		widget& widget{_ui.get(tag)};
		widget.pos.change(glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
	}
	_ui.get("logo_text").pos.change({500, 220}, 0.5_s);
	_ui.get("logo_overlay").pos.change({500, 220}, 0.5_s);
	_ui.get("logo_ball").pos.change({487, 57}, 0.5_s);
	_ui.get("copyright").pos.change({4, 1000}, 0.5_s);
	_ui.get("version").pos.change({996, 1000}, 0.5_s);
	_ui.hide_all(0.5_s);
}