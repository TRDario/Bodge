#include "../../include/renderer.hpp"
#include "../../include/state.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_REPLAY{"replay"};
constexpr tag T_INDICATOR{"indicator"};

//////////////////////////////////////////////////////////////// GAME STATE ///////////////////////////////////////////////////////////////

game_state::game_state(std::shared_ptr<game> game, game_state_data data, fade_in fade_in)
	: state{{}, {}}
	, m_substate{(fade_in == fade_in::YES ? substate::FADING_IN : substate::ONGOING)}
	, m_data{std::move(data)}
	, m_song_speed{1}
	, m_game{std::move(game)}
{
	if (std::holds_alternative<replay_game_data>(m_data)) {
		m_ui.emplace<label_widget>(T_REPLAY, glm::vec2{4, 1000}, tr::align::BOTTOM_LEFT, 0, NO_TOOLTIP, loc_text_callback{T_REPLAY},
								   tr::sys::ttf_style::NORMAL, 48);
		m_ui.emplace<replay_playback_indicator_widget>(T_INDICATOR, glm::vec2{992, 994}, tr::align::BOTTOM_RIGHT, 0);
	}
}

//

tr::next_state game_state::handle_event(const tr::sys::event& event)
{
	if (m_substate != substate::FADING_IN && event.is<tr::sys::key_down_event>() && event.as<tr::sys::key_down_event>().key == "Escape"_k) {
		g_audio.play_sound(sound::PAUSE, 0.8f, 0.0f);
		g_audio.pause_song();
		return std::make_unique<pause_state>(m_game, m_data, g_mouse_pos, blur_in::YES);
	}
	else {
		return tr::KEEP_STATE;
	}
}

tr::next_state game_state::tick()
{
	state::tick();
	switch (m_substate) {
	case substate::FADING_IN:
		if (m_elapsed >= 0.5_s) {
			m_substate = substate::ONGOING;
			m_elapsed = 0;
			g_audio.play_song(m_game->gamemode().song, 0.1s);
			g_audio.play_sound(sound::BALL_SPAWN, 0.25f, 0);
		}
		return tr::KEEP_STATE;
	case substate::ONGOING:
		if (std::holds_alternative<replay_game_data>(m_data)) {
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
					m_substate = substate::GAME_OVER;
				}
				else {
					m_substate = substate::EXITING;
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
				m_substate = substate::GAME_OVER;
				m_elapsed = 0;
				g_audio.fade_song_out(0.5s);
				if (std::holds_alternative<regular_game_data>(m_data)) {
					m_next_state = make_async<game_over_state>(m_game, blur_in::YES);
				}
			}
		}
		return tr::KEEP_STATE;
	case substate::GAME_OVER:
		m_game->tick();
		if (m_elapsed >= 0.75_s) {
			g_renderer->set_default_transform(TRANSFORM);
			switch (m_data.index()) {
			case tr::type_index<regular_game_data, game_state_data>:
				return m_next_state.get();
			case tr::type_index<test_game_data, game_state_data>:
				m_substate = substate::EXITING;
				m_elapsed = 0;
				m_next_state = make_async<gamemode_editor_state>(tr::get<test_game_data>(m_data).editor_data, m_game->gamemode());
				break;
			case tr::type_index<replay_game_data, game_state_data>:
				m_substate = substate::EXITING;
				m_elapsed = 0;
				m_next_state = make_async<replays_state>();
				break;
			}
		}
		return tr::KEEP_STATE;
	case substate::EXITING:
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
	if (std::holds_alternative<replay_game_data>(m_data)) {
		m_ui.add_to_renderer();
		add_replay_cursor_to_renderer(((replay_game&)*m_game).cursor_pos());
	}
	g_renderer->add_fade_overlay(fade_overlay_opacity());
	g_renderer->draw_layers(g_renderer->screen);
}

//

float game_state::fade_overlay_opacity() const
{
	switch (m_substate) {
	case substate::FADING_IN:
		return 1 - m_elapsed / 0.5_sf;
	case substate::ONGOING:
	case substate::GAME_OVER:
		return 0;
	case substate::EXITING:
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