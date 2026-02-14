#include "../../include/renderer.hpp"
#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_REPLAY{"replay"};
constexpr tag T_INDICATOR{"indicator"};

//////////////////////////////////////////////////////////////// GAME STATE ///////////////////////////////////////////////////////////////

game_state::game_state(std::shared_ptr<game> game, game_type type, fade_in fade_in)
	: state{{}, {}}
	, m_substate{(fade_in == fade_in::YES ? substate_base::FADING_IN : substate_base::ONGOING) | type}
	, m_song_speed{1}
	, m_game{std::move(game)}
{
	if (type == game_type::REPLAY) {
		m_ui.emplace<label_widget>(T_REPLAY, glm::vec2{4, 1000}, tr::align::BOTTOM_LEFT, 0, NO_TOOLTIP, loc_text_callback{T_REPLAY},
								   tr::sys::ttf_style::NORMAL, 48);
		m_ui.emplace<replay_playback_indicator_widget>(T_INDICATOR, glm::vec2{992, 994}, tr::align::BOTTOM_RIGHT, 0);
	}
}

//

tr::next_state game_state::handle_event(const tr::sys::event& event)
{
	if (to_base(m_substate) != substate_base::FADING_IN && event.is<tr::sys::key_down_event>() &&
		event.as<tr::sys::key_down_event>().key == "Escape"_k) {
		g_audio.play_sound(sound::PAUSE, 0.8f, 0.0f);
		g_audio.pause_song();
		return std::make_unique<pause_state>(m_game, to_type(m_substate), g_mouse_pos, blur_in::YES);
	}
	else {
		return tr::KEEP_STATE;
	}
}

tr::next_state game_state::tick()
{
	state::tick();
	switch (to_base(m_substate)) {
	case substate_base::FADING_IN:
		if (m_elapsed >= 0.5_s) {
			m_substate = substate_base::ONGOING | to_type(m_substate);
			m_elapsed = 0;
			g_audio.play_song(m_game->gamemode().song, 0.1s);
			g_audio.play_sound(sound::BALL_SPAWN, 0.25f, 0);
		}
		return tr::KEEP_STATE;
	case substate_base::ONGOING:
		if (to_type(m_substate) == game_type::REPLAY) {
			if (g_held_keymods & tr::sys::keymod::SHIFT) {
				if (m_elapsed % 4 == 0) {
					m_game->tick();
				}
				set_song_speed_if_needed(0.25f);
			}
			else if (g_held_keymods & tr::sys::keymod::CTRL) {
				for (int i = 0; i < 4; ++i) {
					m_game->tick();
					if (((replay_game&)*m_game).done()) {
						break;
					}
				}
				set_song_speed_if_needed(4.0f);
			}
			else {
				m_game->tick();
				set_song_speed_if_needed(1.0f);
			}

			if (((replay_game&)*m_game).done()) {
				if (m_game->game_over()) {
					m_substate = substate_base::GAME_OVER | game_type::REPLAY;
				}
				else {
					m_substate = substate_base::EXITING | game_type::REPLAY;
					m_next_state = make_async<replays_state>();
				}
				g_audio.fade_song_out(0.5s);
				m_elapsed = 0;
			}
			else if (m_elapsed % 120 == 60) {
				m_ui[T_REPLAY].hide();
			}
			else if (m_elapsed % 120 == 0) {
				m_ui[T_REPLAY].unhide();
			}
		}
		else {
			m_game->tick();
			if (m_game->game_over()) {
				m_substate = substate_base::GAME_OVER | to_type(m_substate);
				m_elapsed = 0;
				g_audio.fade_song_out(0.5s);
				if (to_type(m_substate) == game_type::REGULAR) {
					m_next_state = make_async<game_over_state>(m_game, blur_in::YES);
				}
			}
		}
		return tr::KEEP_STATE;
	case substate_base::GAME_OVER:
		m_game->tick();
		if (m_elapsed >= 0.75_s) {
			g_renderer->set_default_transform(TRANSFORM);
			switch (to_type(m_substate)) {
			case game_type::REGULAR: {
				return m_next_state.get();
			}
			case game_type::GAMEMODE_DESIGNER_TEST:
				m_substate = substate_base::EXITING | game_type::GAMEMODE_DESIGNER_TEST;
				m_elapsed = 0;
				m_next_state = make_async<gamemode_designer_state>();
				break;
			case game_type::REPLAY:
				m_substate = substate_base::EXITING | game_type::REPLAY;
				m_elapsed = 0;
				m_next_state = make_async<replays_state>();
				break;
			}
		}
		return tr::KEEP_STATE;
	case substate_base::EXITING:
		if (m_elapsed >= 1_s) {
			g_renderer->set_default_transform(TRANSFORM);
			return m_next_state.get();
		}
		return tr::KEEP_STATE;
	}
}

void game_state::draw()
{
	m_game->add_to_renderer();
	if (to_type(m_substate) == game_type::REPLAY) {
		m_ui.add_to_renderer();
		add_replay_cursor_to_renderer(((replay_game&)*m_game).cursor_pos());
	}
	g_renderer->add_fade_overlay(fade_overlay_opacity());
	g_renderer->draw_layers(g_renderer->screen);
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
		return 1 - m_elapsed / 0.5_sf;
	case substate_base::ONGOING:
	case substate_base::GAME_OVER:
		return 0;
	case substate_base::EXITING:
		return std::max(int(m_elapsed - 0.5_s), 0) / 0.5_sf;
	}
}

void game_state::set_song_speed_if_needed(float speed)
{
	if (m_song_speed != speed) {
		g_audio.set_song_speed(speed);
		m_song_speed = speed;
	}
}

void game_state::add_replay_cursor_to_renderer(glm::vec2 pos) const
{
	constexpr glm::vec2 SIZE{12, 2};
	const tr::rgba8 color{color_cast<tr::rgba8>(tr::hsv{float(g_settings.primary_hue), 1, 1})};

	tr::gfx::simple_color_mesh_ref quad{g_renderer->basic.new_color_fan(layer::UI, 4)};
	tr::fill_rectangle_vertices(quad.positions, pos, SIZE / 2.0f, SIZE, 45_deg);
	std::ranges::fill(quad.colors, color);
	quad = g_renderer->basic.new_color_fan(layer::UI, 4);
	tr::fill_rectangle_vertices(quad.positions, pos, SIZE / 2.0f, SIZE, -45_deg);
	std::ranges::fill(quad.colors, color);
}