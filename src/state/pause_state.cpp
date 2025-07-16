#include "../../include/state/pause_state.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/replays_state.hpp"
#include "../../include/state/save_score_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The regular pause screen buttons.
constexpr std::array<const char*, 5> BUTTONS_REGULAR{"unpause", "save_and_restart", "restart", "save_and_quit", "quit"};
// The special pause screen buttons.
constexpr std::array<const char*, 3> BUTTONS_SPECIAL{"unpause", "restart", "quit"};
// Shortcuts of the regular button set.
constexpr std::array<std::initializer_list<tr::key_chord>, BUTTONS_REGULAR.size()> SHORTCUTS_REGULAR{{
	{{tr::keycode::ESCAPE}, {tr::keycode::TOP_ROW_1}},
	{{tr::keycode::R, tr::keymod::SHIFT}, {tr::keycode::TOP_ROW_2}},
	{{tr::keycode::R}, {tr::keycode::TOP_ROW_3}},
	{{tr::keycode::Q, tr::keymod::SHIFT}, {tr::keycode::E, tr::keymod::SHIFT}, {tr::keycode::TOP_ROW_4}},
	{{tr::keycode::Q}, {tr::keycode::E}, {tr::keycode::TOP_ROW_5}},
}};
// Shortcuts of the special button set.
constexpr std::array<std::initializer_list<tr::key_chord>, BUTTONS_SPECIAL.size()> SHORTCUTS_SPECIAL{{
	{{tr::keycode::ESCAPE}, {tr::keycode::TOP_ROW_1}},
	{{tr::keycode::R}, {tr::keycode::TOP_ROW_2}},
	{{tr::keycode::Q}, {tr::keycode::E}, {tr::keycode::TOP_ROW_3}},
}};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

pause_state::pause_state(std::unique_ptr<game>&& game, game_type type, glm::vec2 mouse_pos, bool blur_in) noexcept
	: _substate{(blur_in ? substate_base::PAUSING : substate_base::PAUSED) | type}
	, _timer{0}
	, _game{std::move(game)}
	, _start_mouse_pos{mouse_pos}
{
	if (blur_in) {
		_game->add_to_renderer();
		tr::renderer_2d::draw(engine::blur_renderer().input());
	}

	if (type == game_type::REGULAR) {
		set_up_full_ui();
	}
	else {
		set_up_limited_ui();
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> pause_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> pause_state::update(tr::duration)
{
	++_timer;
	_ui.update();

	switch (to_base(_substate)) {
	case substate_base::PAUSING:
		if (_timer >= 0.5_s) {
			_timer = 0;
			_substate = substate_base::PAUSED | to_type(_substate);
		}
		return nullptr;
	case substate_base::PAUSED:
		return nullptr;
	case substate_base::UNPAUSING:
		if (to_type(_substate) != game_type::REPLAY) {
			float ratio{_timer / 0.5_sf};
			ratio = ratio < 0.5 ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
			engine::set_mouse_pos(_end_mouse_pos + (_start_mouse_pos - _end_mouse_pos) * ratio);
		}
		return _timer >= 0.5_s ? std::make_unique<game_state>(std::move(_game), to_type(_substate), false) : nullptr;
	case substate_base::RESTARTING:
		if (_timer < 0.5_s) {
			return nullptr;
		}
		tr::renderer_2d::set_default_transform(TRANSFORM);
		switch (to_type(_substate)) {
		case game_type::REGULAR:
		case game_type::TEST:
			return std::make_unique<game_state>(std::make_unique<active_game>(_game->gamemode()), to_type(_substate), true);
		case game_type::REPLAY:
			return std::make_unique<game_state>(std::make_unique<replay_game>(static_cast<replay_game&>(*_game)), game_type::REPLAY, true);
		}
	case substate_base::SAVING_AND_RESTARTING:
	case substate_base::SAVING_AND_QUITTING: {
		if (_timer >= 0.5_s) {
			const save_screen_flags state_flags{to_base(_substate) == substate_base::SAVING_AND_RESTARTING ? save_screen_flags::RESTARTING
																										   : save_screen_flags::NONE};
			return std::make_unique<save_score_state>(std::unique_ptr<active_game>{static_cast<active_game*>(_game.release())},
													  _start_mouse_pos, state_flags);
		}
		else {
			return nullptr;
		}
	}
	case substate_base::QUITTING:
		if (_timer < 0.5_s) {
			return nullptr;
		}
		tr::renderer_2d::set_default_transform(TRANSFORM);
		switch (to_type(_substate)) {
		case game_type::REGULAR:
			return std::make_unique<title_state>();
		case game_type::TEST:
			return std::make_unique<gamemode_designer_state>(_game->gamemode());
		case game_type::REPLAY:
			return std::make_unique<replays_state>();
		}
	}
}

void pause_state::draw()
{
	engine::blur_renderer().draw(saturation_factor(), blur_strength());
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(
		to_base(_substate) == substate_base::RESTARTING || to_base(_substate) == substate_base::QUITTING ? _timer / 0.5_sf : 0);
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

pause_state::substate operator|(const pause_state::substate_base& l, const game_type& r) noexcept
{
	return static_cast<pause_state::substate>(static_cast<int>(l) | static_cast<int>(r));
}

pause_state::substate_base to_base(pause_state::substate state) noexcept
{
	return static_cast<pause_state::substate_base>(static_cast<int>(state) & 0x7);
}

game_type to_type(pause_state::substate state) noexcept
{
	return static_cast<game_type>(static_cast<int>(state) & 0x18);
}

float pause_state::saturation_factor() const noexcept
{
	switch (to_base(_substate)) {
	case substate_base::PAUSED:
	case substate_base::SAVING_AND_RESTARTING:
	case substate_base::RESTARTING:
	case substate_base::SAVING_AND_QUITTING:
	case substate_base::QUITTING:
		return 0.35f;
	case substate_base::PAUSING:
		return 1 - _timer / 0.5_sf * 0.65f;
	case substate_base::UNPAUSING:
		return 0.35f + _timer / 0.5_sf * 0.65f;
	}
}

float pause_state::blur_strength() const noexcept
{
	switch (to_base(_substate)) {
	case substate_base::PAUSED:
	case substate_base::SAVING_AND_RESTARTING:
	case substate_base::RESTARTING:
	case substate_base::SAVING_AND_QUITTING:
	case substate_base::QUITTING:
		return 10;
	case substate_base::PAUSING:
		return _timer / 0.5_sf * 10;
	case substate_base::UNPAUSING:
		return (1 - _timer / 0.5_sf) * 10;
	}
}

void pause_state::set_up_full_ui()
{
	constexpr float TITLE_Y{500.0f - (BUTTONS_REGULAR.size() + 1) * 30};
	widget& title{
		_ui.emplace<text_widget>("paused", glm::vec2{500, TITLE_Y - 100}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, TITLE_Y}, 0.5_s);
	title.unhide(0.5_s);

	const status_callback status_cb{
		[this] { return to_base(_substate) == substate_base::PAUSED || to_base(_substate) == substate_base::PAUSING; }};
	const status_callback unpause_status_cb{[this] { return to_base(_substate) == substate_base::PAUSED; }};
	std::array<action_callback, BUTTONS_REGULAR.size()> action_cbs{
		[this] {
			_timer = 0;
			_substate = substate_base::UNPAUSING | game_type::REGULAR;
			_end_mouse_pos = engine::mouse_pos();
			set_up_exit_animation();
			audio::play(sfx::UNPAUSE, 0.8f, 0.0f);
		},
		[this] {
			_timer = 0;
			_substate = substate_base::SAVING_AND_RESTARTING | game_type::REGULAR;
			set_up_exit_animation();
		},
		[this] {
			_timer = 0;
			_substate = substate_base::RESTARTING | game_type::REGULAR;
			scorefile.playtime += _game->result();
			scorefile.update_category(_game->gamemode(), _game->result());
			set_up_exit_animation();
		},
		[this] {
			_timer = 0;
			_substate = substate_base::SAVING_AND_QUITTING | game_type::REGULAR;
			set_up_exit_animation();
		},
		[this] {
			_timer = 0;
			_substate = substate_base::QUITTING | game_type::REGULAR;
			scorefile.playtime += _game->result();
			scorefile.update_category(_game->gamemode(), _game->result());
			set_up_exit_animation();
		},
	};
	for (std::size_t i = 0; i < BUTTONS_REGULAR.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * tr::rand(rng, 50.0f, 150.0f)};
		const glm::vec2 pos{500 + offset, 500 - (BUTTONS_REGULAR.size() + 1) * 30 + (i + 2) * 60};
		widget& widget{_ui.emplace<clickable_text_widget>(BUTTONS_REGULAR[i], pos, tr::align::CENTER, font::LANGUAGE, 48,
														  DEFAULT_TEXT_CALLBACK, i == 0 ? unpause_status_cb : status_cb,
														  std::move(action_cbs[i]), NO_TOOLTIP, SHORTCUTS_REGULAR[i])};
		widget.pos.change({500, pos.y}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

void pause_state::set_up_limited_ui()
{
	constexpr float TITLE_Y{500.0f - (BUTTONS_SPECIAL.size() + 1) * 30};
	const char* const title_tag{to_type(_substate) == game_type::REPLAY ? "replay_paused" : "test_paused"};
	widget& title{
		_ui.emplace<text_widget>(title_tag, glm::vec2{500, TITLE_Y - 100}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, TITLE_Y}, 0.5_s);
	title.unhide(0.5_s);

	const status_callback status_cb{
		[this] { return to_base(_substate) == substate_base::PAUSED || to_base(_substate) == substate_base::PAUSING; }};
	const status_callback unpause_status_cb{[this] { return to_base(_substate) == substate_base::PAUSED; }};
	std::array<action_callback, BUTTONS_SPECIAL.size()> action_cbs{
		[this] {
			_timer = 0;
			_substate = substate_base::UNPAUSING | to_type(_substate);
			_end_mouse_pos = engine::mouse_pos();
			set_up_exit_animation();
		},
		[this] {
			_timer = 0;
			_substate = substate_base::RESTARTING | to_type(_substate);
			set_up_exit_animation();
		},
		[this] {
			_timer = 0;
			_substate = substate_base::QUITTING | to_type(_substate);
			set_up_exit_animation();
		},
	};
	for (std::size_t i = 0; i < BUTTONS_SPECIAL.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * tr::rand(rng, 50.0f, 150.0f)};
		const glm::vec2 pos{500 + offset, 500 - (BUTTONS_SPECIAL.size() + 1) * 30 + (i + 2) * 60};
		widget& widget{_ui.emplace<clickable_text_widget>(BUTTONS_SPECIAL[i], pos, tr::align::CENTER, font::LANGUAGE, 48,
														  DEFAULT_TEXT_CALLBACK, i == 0 ? unpause_status_cb : status_cb,
														  std::move(action_cbs[i]), NO_TOOLTIP, SHORTCUTS_SPECIAL[i])};
		widget.pos.change({500, pos.y}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

void pause_state::set_up_exit_animation() noexcept
{
	if (to_type(_substate) == game_type::REGULAR) {
		_ui.get("paused").pos.change({500, 400 - (BUTTONS_REGULAR.size() + 1) * 30}, 0.5_s);
		for (std::size_t i = 0; i < BUTTONS_REGULAR.size(); ++i) {
			const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * tr::rand(rng, 50.0f, 150.0f)};
			widget& widget{_ui.get(BUTTONS_REGULAR[i])};
			widget.pos.change(glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
		}
	}
	else {
		widget& title{_ui.get(to_type(_substate) == game_type::TEST ? "test_paused" : "replay_paused")};
		title.pos.change({500, 400 - (BUTTONS_SPECIAL.size() + 1) * 30}, 0.5_s);
		for (std::size_t i = 0; i < BUTTONS_SPECIAL.size(); ++i) {
			const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * tr::rand(rng, 50.0f, 150.0f)};
			widget& widget{_ui.get(BUTTONS_SPECIAL[i])};
			widget.pos.change(glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
		}
	}

	_ui.hide_all(0.5_s);
}