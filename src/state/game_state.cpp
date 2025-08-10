#include "../../include/state/game_state.hpp"
#include "../../include/graphics.hpp"
#include "../../include/state/game_over_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/pause_state.hpp"
#include "../../include/state/replays_state.hpp"
#include "../../include/system.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag TAG_REPLAY{"replay"};
constexpr tag TAG_INDICATOR{"indicator"};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

game_state::game_state(std::unique_ptr<game>&& game, game_type type, bool fade_in)
	: m_substate{(fade_in ? substate_base::STARTING : substate_base::ONGOING) | type}, m_timer{0}, m_game{std::move(game)}
{
	if (!fade_in) {
		engine::unpause_song();
	}

	if (type == game_type::REPLAY) {
		widget& replay{m_ui.emplace<text_widget>(TAG_REPLAY, glm::vec2{4, 1000}, tr::align::BOTTOM_LEFT, font::LANGUAGE,
												 tr::system::ttf_style::NORMAL, 48, loc_text_callback{TAG_REPLAY})};
		replay.unhide();
		widget& indicator{m_ui.emplace<replay_playback_indicator_widget>(TAG_INDICATOR, glm::vec2{992, 994}, tr::align::BOTTOM_RIGHT)};
		indicator.unhide();
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> game_state::handle_event(const tr::system::event& event)
{
	if (to_base(m_substate) != substate_base::STARTING && event.type() == tr::system::key_down_event::ID &&
		tr::system::key_down_event{event}.key == tr::system::keycode::ESCAPE) {
		engine::play_sound(sound::PAUSE, 0.8f, 0.0f);
		return std::make_unique<pause_state>(std::move(m_game), to_type(m_substate), engine::mouse_pos(), true);
	}
	return nullptr;
}

std::unique_ptr<tr::state> game_state::update(tr::duration)
{
	++m_timer;
	m_ui.update();
	switch (to_base(m_substate)) {
	case substate_base::STARTING:
		if (m_timer >= 0.5_s) {
			m_substate = substate_base::ONGOING | to_type(m_substate);
			m_timer = 0;
			engine::play_song(m_game->gamemode().song, 0.1s);
		}
		return nullptr;
	case substate_base::ONGOING:
		if (to_type(m_substate) == game_type::REPLAY) {
			if (engine::held_keymods() & tr::system::keymod::SHIFT) {
				if (m_timer % 4 == 0) {
					m_game->update();
				}
			}
			else if (engine::held_keymods() & tr::system::keymod::CTRL) {
				for (int i = 0; i < 4; ++i) {
					m_game->update();
					if (static_cast<replay_game*>(m_game.get())->done()) {
						break;
					}
				}
			}
			else {
				m_game->update();
			}

			if (static_cast<replay_game*>(m_game.get())->done()) {
				if (m_game->game_over()) {
					m_substate = substate_base::GAME_OVER | game_type::REPLAY;
					engine::fade_song_out(0.5s);
				}
				else {
					m_substate = substate_base::EXITING | game_type::REPLAY;
				}
				m_timer = 0;
			}
			else if (m_timer % 120 == 60) {
				m_ui[TAG_REPLAY].hide();
			}
			else if (m_timer % 120 == 0) {
				m_ui[TAG_REPLAY].unhide();
			}
		}
		else {
			m_game->update();
			if (m_game->game_over()) {
				m_substate = substate_base::GAME_OVER | to_type(m_substate);
				m_timer = 0;
				engine::fade_song_out(0.5s);
			}
		}
		return nullptr;
	case substate_base::GAME_OVER:
		m_game->update();
		if (m_timer >= 0.75_s) {
			tr::gfx::renderer_2d::set_default_transform(TRANSFORM);
			switch (to_type(m_substate)) {
			case game_type::REGULAR: {
				const ticks prev_pb{pb(engine::scorefile, m_game->gamemode())};
				update_pb(engine::scorefile, m_game->gamemode(), m_game->result());
				return std::make_unique<game_over_state>(std::unique_ptr<active_game>{static_cast<active_game*>(m_game.release())}, true,
														 prev_pb);
			}
			case game_type::TEST:
			case game_type::REPLAY:
				m_substate = substate_base::EXITING | to_type(m_substate);
				m_timer = 0;
				break;
			}
		}
		return nullptr;
	case substate_base::EXITING:
		if (m_timer >= 1_s) {
			tr::gfx::renderer_2d::set_default_transform(TRANSFORM);
			if (to_type(m_substate) == game_type::REPLAY) {
				return std::make_unique<replays_state>();
			}
			else {
				return std::make_unique<gamemode_designer_state>(m_game->gamemode());
			}
		}
		return nullptr;
	}
}

void game_state::draw()
{
	m_game->add_to_renderer();
	if (to_type(m_substate) == game_type::REPLAY) {
		m_ui.add_to_renderer();
		add_replay_cursor_to_renderer(static_cast<replay_game*>(m_game.get())->cursor_pos());
	}
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

game_state::substate operator|(const game_state::substate_base& l, const game_type& r)
{
	return static_cast<game_state::substate>(static_cast<int>(l) | static_cast<int>(r));
}

game_state::substate_base to_base(game_state::substate state)
{
	return static_cast<game_state::substate_base>(static_cast<int>(state) & 0x3);
}

game_type to_type(game_state::substate state)
{
	return static_cast<game_type>(static_cast<int>(state) & 0x18);
}

float game_state::fade_overlay_opacity() const
{
	switch (to_base(m_substate)) {
	case substate_base::STARTING:
		return 1 - m_timer / 0.5_sf;
	case substate_base::ONGOING:
	case substate_base::GAME_OVER:
		return 0;
	case substate_base::EXITING:
		return std::max(static_cast<int>(m_timer - 0.5_s), 0) / 0.5_sf;
	}
}

void game_state::add_replay_cursor_to_renderer(glm::vec2 pos) const
{
	tr::gfx::simple_color_mesh_ref quad{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	fill_rotated_rect_vtx(quad.positions, pos, {6, 1}, {12, 2}, 45_deg);
	std::ranges::fill(quad.colors, color_cast<tr::rgba8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 1, 1}));
	quad = tr::gfx::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(quad.positions, pos, {6, 1}, {12, 2}, -45_deg);
	std::ranges::fill(quad.colors, color_cast<tr::rgba8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 1, 1}));
}