#include "../../include/graphics.hpp"
#include "../../include/state/state.hpp"
#include "../../include/system.hpp"
#include "../../include/ui/widget.hpp"

//

constexpr tag T_REPLAY{"replay"};
constexpr tag T_INDICATOR{"indicator"};

//

game_state::game_state(std::shared_ptr<game> game, game_type type, bool fade_in)
	: state{{}, {}}, m_substate{(fade_in ? substate_base::FADING_IN : substate_base::ONGOING) | type}, m_game{std::move(game)}
{
	if (!fade_in) {
		engine::unpause_song();
	}

	if (type == game_type::REPLAY) {
		m_ui.emplace<label_widget>(T_REPLAY, glm::vec2{4, 1000}, tr::align::BOTTOM_LEFT, 0, NO_TOOLTIP, loc_text_callback{T_REPLAY},
								   text_style::NORMAL, 48);
		m_ui.emplace<replay_playback_indicator_widget>(T_INDICATOR, glm::vec2{992, 994}, tr::align::BOTTOM_RIGHT, 0);
	}
}

//

std::unique_ptr<tr::state> game_state::handle_event(const tr::sys::event& event)
{
	return event.visit(tr::overloaded{
		[this](tr::sys::key_down_event event) -> std::unique_ptr<tr::state> {
			if (to_base(m_substate) != substate_base::FADING_IN && event.key == tr::sys::keycode::ESCAPE) {
				engine::play_sound(sound::PAUSE, 0.8f, 0.0f);
				engine::pause_song();
				return std::make_unique<pause_state>(m_game, to_type(m_substate), engine::mouse_pos(), true);
			}
			return nullptr;
		},
		[](auto) -> std::unique_ptr<tr::state> { return nullptr; },
	});
}

std::unique_ptr<tr::state> game_state::update(tr::duration)
{
	state::update({});
	switch (to_base(m_substate)) {
	case substate_base::FADING_IN:
		if (m_timer >= 0.5_s) {
			m_substate = substate_base::ONGOING | to_type(m_substate);
			m_timer = 0;
			engine::play_song(m_game->gamemode().song, 0.1s);
			engine::play_sound(sound::BALL_SPAWN, 0.25f, 0);
		}
		return nullptr;
	case substate_base::ONGOING:
		if (to_type(m_substate) == game_type::REPLAY) {
			if (engine::held_keymods() & tr::sys::keymod::SHIFT) {
				if (m_timer % 4 == 0) {
					m_game->update();
				}
			}
			else if (engine::held_keymods() & tr::sys::keymod::CTRL) {
				for (int i = 0; i < 4; ++i) {
					m_game->update();
					if (((replay_game*)m_game.get())->done()) {
						break;
					}
				}
			}
			else {
				m_game->update();
			}

			if (((replay_game*)m_game.get())->done()) {
				if (m_game->game_over()) {
					m_substate = substate_base::GAME_OVER | game_type::REPLAY;
				}
				else {
					m_substate = substate_base::EXITING | game_type::REPLAY;
					m_next_state = make_async<replays_state>();
				}
				engine::fade_song_out(0.5s);
				m_timer = 0;
			}
			else if (m_timer % 120 == 60) {
				m_ui[T_REPLAY].hide();
			}
			else if (m_timer % 120 == 0) {
				m_ui[T_REPLAY].unhide();
			}
		}
		else {
			m_game->update();
			if (m_game->game_over()) {
				m_substate = substate_base::GAME_OVER | to_type(m_substate);
				m_timer = 0;
				engine::fade_song_out(0.5s);
				if (to_type(m_substate) == game_type::REGULAR) {
					m_next_state = make_async<game_over_state>(m_game, true);
				}
			}
		}
		return nullptr;
	case substate_base::GAME_OVER:
		m_game->update();
		if (m_timer >= 0.75_s) {
			engine::basic_renderer().set_default_transform(TRANSFORM);
			switch (to_type(m_substate)) {
			case game_type::REGULAR: {
				return m_next_state.get();
			}
			case game_type::GAMEMODE_DESIGNER_TEST:
				m_substate = substate_base::EXITING | game_type::GAMEMODE_DESIGNER_TEST;
				m_timer = 0;
				m_next_state = make_async<replays_state>();
				break;
			case game_type::REPLAY:
				m_substate = substate_base::EXITING | game_type::REPLAY;
				m_timer = 0;
				m_next_state = make_async<gamemode_designer_state>(m_game->gamemode());
				break;
			}
		}
		return nullptr;
	case substate_base::EXITING:
		if (m_timer >= 1_s) {
			engine::basic_renderer().set_default_transform(TRANSFORM);
			engine::play_song("menu", SKIP_MENU_SONG_INTRO_TIMESTAMP, 0.5s);
			return m_next_state.get();
		}
		return nullptr;
	}
}

void game_state::draw()
{
	m_game->add_to_renderer();
	if (to_type(m_substate) == game_type::REPLAY) {
		m_ui.add_to_renderer();
		add_replay_cursor_to_renderer(((replay_game*)m_game.get())->cursor_pos());
	}
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	engine::basic_renderer().draw(engine::screen());
}

//

game_state::substate operator|(const game_state::substate_base& l, const game_type& r)
{
	return game_state::substate(int(l) | int(r));
}

game_state::substate_base to_base(game_state::substate state)
{
	return game_state::substate_base(int(state) & 0x3);
}

game_type to_type(game_state::substate state)
{
	return game_type(int(state) & 0x18);
}

float game_state::fade_overlay_opacity() const
{
	switch (to_base(m_substate)) {
	case substate_base::FADING_IN:
		return 1 - m_timer / 0.5_sf;
	case substate_base::ONGOING:
	case substate_base::GAME_OVER:
		return 0;
	case substate_base::EXITING:
		return std::max(int(m_timer - 0.5_s), 0) / 0.5_sf;
	}
}

void game_state::add_replay_cursor_to_renderer(glm::vec2 pos) const
{
	tr::gfx::simple_color_mesh_ref quad{engine::basic_renderer().new_color_fan(layer::UI, 4)};
	tr::fill_rectangle_vertices(quad.positions, pos, {6, 1}, {12, 2}, 45_deg);
	std::ranges::fill(quad.colors, color_cast<tr::rgba8>(tr::hsv{float(engine::settings.primary_hue), 1, 1}));
	quad = engine::basic_renderer().new_color_fan(layer::UI, 4);
	tr::fill_rectangle_vertices(quad.positions, pos, {6, 1}, {12, 2}, -45_deg);
	std::ranges::fill(quad.colors, color_cast<tr::rgba8>(tr::hsv{float(engine::settings.primary_hue), 1, 1}));
}