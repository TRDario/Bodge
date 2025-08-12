#include "../../include/state/game_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/pause_state.hpp"
#include "../../include/state/replays_state.hpp"
#include "../../include/state/save_score_state.hpp"
#include "../../include/state/title_state.hpp"
#include "../../include/system.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_PAUSED{"paused"};
constexpr tag T_TEST_PAUSED{"test_paused"};
constexpr tag T_REPLAY_PAUSED{"replay_paused"};
constexpr tag T_UNPAUSE{"unpause"};
constexpr tag T_SAVE_AND_RESTART{"save_and_restart"};
constexpr tag T_RESTART{"restart"};
constexpr tag T_SAVE_AND_QUIT{"save_and_quit"};
constexpr tag T_QUIT{"quit"};

// The regular pause screen buttons.
constexpr std::array<tag, 5> BUTTONS_REGULAR{T_UNPAUSE, T_SAVE_AND_RESTART, T_RESTART, T_SAVE_AND_QUIT, T_QUIT};
// The special pause screen buttons.
constexpr std::array<tag, 3> BUTTONS_SPECIAL{T_UNPAUSE, T_RESTART, T_QUIT};

constexpr shortcut_table SHORTCUTS_REGULAR{
	{{tr::system::keycode::ESCAPE}, T_UNPAUSE},
	{{tr::system::keycode::TOP_ROW_1}, T_UNPAUSE},
	{{tr::system::keycode::R, tr::system::keymod::SHIFT}, T_SAVE_AND_RESTART},
	{{tr::system::keycode::TOP_ROW_2}, T_SAVE_AND_RESTART},
	{{tr::system::keycode::R}, T_RESTART},
	{{tr::system::keycode::TOP_ROW_3}, T_RESTART},
	{{tr::system::keycode::Q, tr::system::keymod::SHIFT}, T_SAVE_AND_QUIT},
	{{tr::system::keycode::TOP_ROW_4}, T_SAVE_AND_QUIT},
	{{tr::system::keycode::Q}, T_QUIT},
	{{tr::system::keycode::TOP_ROW_5}, T_QUIT},
};

constexpr shortcut_table SHORTCUTS_SPECIAL{
	{{tr::system::keycode::ESCAPE}, T_UNPAUSE}, {{tr::system::keycode::TOP_ROW_1}, T_UNPAUSE},
	{{tr::system::keycode::R}, T_RESTART},      {{tr::system::keycode::TOP_ROW_2}, T_RESTART},
	{{tr::system::keycode::Q}, T_QUIT},         {{tr::system::keycode::TOP_ROW_3}, T_QUIT},
};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

pause_state::pause_state(std::unique_ptr<game>&& game, game_type type, glm::vec2 mouse_pos, bool blur_in)
	: m_substate{(blur_in ? substate_base::PAUSING : substate_base::PAUSED) | type}
	, m_timer{0}
	, m_ui{type == game_type::REGULAR ? SHORTCUTS_REGULAR : SHORTCUTS_SPECIAL}
	, m_game{std::move(game)}
	, m_start_mouse_pos{mouse_pos}
{
	if (blur_in) {
		m_game->add_to_renderer();
		tr::gfx::renderer_2d::draw(engine::blur().input());
		engine::pause_song();
	}

	if (type == game_type::REGULAR) {
		set_up_full_ui();
	}
	else {
		set_up_limited_ui();
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> pause_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> pause_state::update(tr::duration)
{
	++m_timer;
	m_ui.update();

	switch (to_base(m_substate)) {
	case substate_base::PAUSING:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate_base::PAUSED | to_type(m_substate);
		}
		return nullptr;
	case substate_base::PAUSED:
		return nullptr;
	case substate_base::UNPAUSING:
		if (to_type(m_substate) != game_type::REPLAY) {
			float ratio{m_timer / 0.5_sf};
			ratio = ratio < 0.5 ? 4 * std::pow(ratio, 3.0f) : 1 - std::pow(-2 * ratio + 2, 3.0f) / 2;
			engine::set_mouse_pos(m_end_mouse_pos + (m_start_mouse_pos - m_end_mouse_pos) * ratio);
		}
		return m_timer >= 0.5_s ? std::make_unique<game_state>(std::move(m_game), to_type(m_substate), false) : nullptr;
	case substate_base::RESTARTING:
		if (m_timer < 0.5_s) {
			return nullptr;
		}
		tr::gfx::renderer_2d::set_default_transform(TRANSFORM);
		switch (to_type(m_substate)) {
		case game_type::REGULAR:
		case game_type::TEST:
			return std::make_unique<game_state>(std::make_unique<active_game>(m_game->gamemode()), to_type(m_substate), true);
		case game_type::REPLAY:
			return std::make_unique<game_state>(std::make_unique<replay_game>(static_cast<replay_game&>(*m_game)), game_type::REPLAY, true);
		}
	case substate_base::SAVING_AND_RESTARTING:
	case substate_base::SAVING_AND_QUITTING: {
		if (m_timer >= 0.5_s) {
			const save_screen_flags state_flags{to_base(m_substate) == substate_base::SAVING_AND_RESTARTING ? save_screen_flags::RESTARTING
																											: save_screen_flags::NONE};
			return std::make_unique<save_score_state>(std::unique_ptr<active_game>{static_cast<active_game*>(m_game.release())},
													  m_start_mouse_pos, state_flags);
		}
		else {
			return nullptr;
		}
	}
	case substate_base::QUITTING:
		if (m_timer < 0.5_s) {
			return nullptr;
		}
		tr::gfx::renderer_2d::set_default_transform(TRANSFORM);
		switch (to_type(m_substate)) {
		case game_type::REGULAR:
			return std::make_unique<title_state>();
		case game_type::TEST:
			return std::make_unique<gamemode_designer_state>(m_game->gamemode());
		case game_type::REPLAY:
			return std::make_unique<replays_state>();
		}
	}
}

void pause_state::draw()
{
	engine::blur().draw(saturation_factor(), blur_strength());
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(
		to_base(m_substate) == substate_base::RESTARTING || to_base(m_substate) == substate_base::QUITTING ? m_timer / 0.5_sf : 0);
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

pause_state::substate operator|(const pause_state::substate_base& l, const game_type& r)
{
	return static_cast<pause_state::substate>(static_cast<int>(l) | static_cast<int>(r));
}

pause_state::substate_base to_base(pause_state::substate state)
{
	return static_cast<pause_state::substate_base>(static_cast<int>(state) & 0x7);
}

game_type to_type(pause_state::substate state)
{
	return static_cast<game_type>(static_cast<int>(state) & 0x18);
}

float pause_state::saturation_factor() const
{
	switch (to_base(m_substate)) {
	case substate_base::PAUSED:
	case substate_base::SAVING_AND_RESTARTING:
	case substate_base::RESTARTING:
	case substate_base::SAVING_AND_QUITTING:
	case substate_base::QUITTING:
		return 0.35f;
	case substate_base::PAUSING:
		return 1 - m_timer / 0.5_sf * 0.65f;
	case substate_base::UNPAUSING:
		return 0.35f + m_timer / 0.5_sf * 0.65f;
	}
}

float pause_state::blur_strength() const
{
	switch (to_base(m_substate)) {
	case substate_base::PAUSED:
	case substate_base::SAVING_AND_RESTARTING:
	case substate_base::RESTARTING:
	case substate_base::SAVING_AND_QUITTING:
	case substate_base::QUITTING:
		return 10;
	case substate_base::PAUSING:
		return m_timer / 0.5_sf * 10;
	case substate_base::UNPAUSING:
		return (1 - m_timer / 0.5_sf) * 10;
	}
}

void pause_state::set_up_full_ui()
{
	constexpr float TITLE_Y{500.0f - (BUTTONS_REGULAR.size() + 1) * 30};
	constexpr interpolator<glm::vec2> TITLE_MOVE_IN{interp::CUBIC, {500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};
	m_ui.emplace<text_widget>(T_PAUSED, TITLE_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE, tr::system::ttf_style::NORMAL, 64,
							  loc_text_callback{T_PAUSED});

	const status_callback status_cb{
		[this] { return to_base(m_substate) == substate_base::PAUSED || to_base(m_substate) == substate_base::PAUSING; }};
	const status_callback unpause_scb{[this] { return to_base(m_substate) == substate_base::PAUSED; }};
	std::array<action_callback, BUTTONS_REGULAR.size()> action_cbs{
		[this] {
			m_timer = 0;
			m_substate = substate_base::UNPAUSING | game_type::REGULAR;
			m_end_mouse_pos = engine::mouse_pos();
			set_up_exit_animation();
			engine::play_sound(sound::UNPAUSE, 0.8f, 0.0f);
		},
		[this] {
			m_timer = 0;
			m_substate = substate_base::SAVING_AND_RESTARTING | game_type::REGULAR;
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate_base::RESTARTING | game_type::REGULAR;
			engine::scorefile.playtime += m_game->result();
			update_pb(engine::scorefile, m_game->gamemode(), m_game->result());
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate_base::SAVING_AND_QUITTING | game_type::REGULAR;
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate_base::QUITTING | game_type::REGULAR;
			engine::scorefile.playtime += m_game->result();
			update_pb(engine::scorefile, m_game->gamemode(), m_game->result());
			set_up_exit_animation();
		},
	};
	for (std::size_t i = 0; i < BUTTONS_REGULAR.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS_REGULAR.size() + 1) * 30 + (i + 2) * 60};
		const interpolator<glm::vec2> move_in{interp::CUBIC, {500 + offset, y}, {500, y}, 0.5_s};
		m_ui.emplace<clickable_text_widget>(BUTTONS_REGULAR[i], move_in, tr::align::CENTER, 0.5_s, font::LANGUAGE, 48,
											loc_text_callback{BUTTONS_REGULAR[i]}, i == 0 ? unpause_scb : status_cb,
											std::move(action_cbs[i]));
	}
}

void pause_state::set_up_limited_ui()
{
	constexpr float TITLE_Y{500.0f - (BUTTONS_SPECIAL.size() + 1) * 30};
	constexpr interpolator<glm::vec2> TITLE_MOVE_IN{interp::CUBIC, {500, TITLE_Y - 100}, {500, TITLE_Y}, 0.5_s};
	const tag title_tag{to_type(m_substate) == game_type::REPLAY ? T_REPLAY_PAUSED : T_TEST_PAUSED};
	m_ui.emplace<text_widget>(title_tag, TITLE_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE, tr::system::ttf_style::NORMAL, 64,
							  loc_text_callback{title_tag});

	const status_callback status_cb{
		[this] { return to_base(m_substate) == substate_base::PAUSED || to_base(m_substate) == substate_base::PAUSING; }};
	const status_callback unpause_scb{[this] { return to_base(m_substate) == substate_base::PAUSED; }};
	std::array<action_callback, BUTTONS_SPECIAL.size()> action_cbs{
		[this] {
			m_timer = 0;
			m_substate = substate_base::UNPAUSING | to_type(m_substate);
			m_end_mouse_pos = engine::mouse_pos();
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate_base::RESTARTING | to_type(m_substate);
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate_base::QUITTING | to_type(m_substate);
			set_up_exit_animation();
		},
	};
	for (std::size_t i = 0; i < BUTTONS_SPECIAL.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
		const float y{500.0f - (BUTTONS_SPECIAL.size() + 1) * 30 + (i + 2) * 60};
		const interpolator<glm::vec2> move_in{interp::CUBIC, {500 + offset, y}, {500, y}, 0.5_s};
		m_ui.emplace<clickable_text_widget>(BUTTONS_SPECIAL[i], move_in, tr::align::CENTER, 0.5_s, font::LANGUAGE, 48,
											loc_text_callback{BUTTONS_SPECIAL[i]}, i == 0 ? unpause_scb : status_cb,
											std::move(action_cbs[i]));
	}
}

void pause_state::set_up_exit_animation()
{
	if (to_type(m_substate) == game_type::REGULAR) {
		m_ui[T_PAUSED].pos.change(interp::CUBIC, {500, 400 - (BUTTONS_REGULAR.size() + 1) * 30}, 0.5_s);
		for (std::size_t i = 0; i < BUTTONS_REGULAR.size(); ++i) {
			const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
			widget& widget{m_ui[BUTTONS_REGULAR[i]]};
			widget.pos.change(interp::CUBIC, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
		}
	}
	else {
		widget& title{m_ui[to_type(m_substate) == game_type::TEST ? T_TEST_PAUSED : T_REPLAY_PAUSED]};
		title.pos.change(interp::CUBIC, {500, 400 - (BUTTONS_SPECIAL.size() + 1) * 30}, 0.5_s);
		for (std::size_t i = 0; i < BUTTONS_SPECIAL.size(); ++i) {
			const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
			widget& widget{m_ui[BUTTONS_SPECIAL[i]]};
			widget.pos.change(interp::CUBIC, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
		}
	}

	m_ui.hide_all(0.5_s);
}