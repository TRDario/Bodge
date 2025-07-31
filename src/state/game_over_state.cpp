#include "../../include/state/game_over_state.hpp"
#include "../../include/graphics.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/save_score_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The pause screen buttons.
constexpr std::array<const char*, 4> BUTTONS{"save_and_restart", "restart", "save_and_quit", "quit"};
// Shortcuts of the buttons.
constexpr std::array<std::initializer_list<tr::key_chord>, BUTTONS.size()> SHORTCUTS{{
	{{tr::keycode::R, tr::keymod::SHIFT}, {tr::keycode::TOP_ROW_1}},
	{{tr::keycode::R}, {tr::keycode::TOP_ROW_2}},
	{{tr::keycode::ESCAPE, tr::keymod::SHIFT},
	 {tr::keycode::Q, tr::keymod::SHIFT},
	 {tr::keycode::E, tr::keymod::SHIFT},
	 {tr::keycode::TOP_ROW_3}},
	{{tr::keycode::ESCAPE}, {tr::keycode::Q}, {tr::keycode::E}, {tr::keycode::TOP_ROW_4}},
}};

// The height of the title text.
constexpr float TITLE_Y{500.0f - (BUTTONS.size() + 3) * 30};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

game_over_state::game_over_state(std::unique_ptr<active_game>&& game, bool blur_in, ticks prev_pb)
	: m_substate{blur_in ? substate::BLURRING_IN : substate::GAME_OVER}, m_timer{0}, m_game{std::move(game)}, m_prev_pb{prev_pb}
{
	widget& title{m_ui.emplace<text_widget>("game_over", glm::vec2{500, TITLE_Y - 100}, tr::align::CENTER, font::LANGUAGE,
											tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, TITLE_Y}, 0.5_s);
	title.unhide(0.5_s);

	const float time_h{(500 - (BUTTONS.size() - 0.75f) * 30) -
					   (engine::line_skip(font::LANGUAGE, 48) + 4 + engine::line_skip(font::LANGUAGE, 24)) / 2};
	const ticks time_ticks{m_game->result()};
	text_callback time_text_cb{[time = timer_text(time_ticks)](const auto&) { return time; }};
	widget& time{m_ui.emplace<text_widget>("time", glm::vec2{400, time_h}, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64,
										   std::move(time_text_cb), "FFFF00C0"_rgba8)};
	time.pos.change({500, time_h}, 0.5_s);
	time.unhide(0.5_s);

	const float pb_h{time_h + engine::line_skip(font::LANGUAGE, 48) + 4};
	text_callback pb_text_cb;
	if (prev_pb < m_game->result()) {
		pb_text_cb = [](const auto&) -> std::string { return std::string{engine::loc["new_pb"]}; };
	}
	else {
		pb_text_cb = [str = std::format("{}: {}", engine::loc["pb"], timer_text(pb(engine::scorefile, m_game->gamemode())))](const auto&) {
			return str;
		};
	}
	widget& pb{m_ui.emplace<text_widget>("pb", glm::vec2{600, pb_h}, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 24,
										 std::move(pb_text_cb), "FFFF00C0"_rgba8)};
	pb.pos.change({500, pb_h}, 0.5_s);
	if (prev_pb >= m_game->result()) {
		pb.unhide(0.5_s);
	}

	const status_callback status_cb{[this] { return m_substate == substate::BLURRING_IN || m_substate == substate::GAME_OVER; }};
	std::array<action_callback, BUTTONS.size()> action_cbs{
		[this] {
			m_timer = 0;
			m_substate = substate::SAVING_AND_RESTARTING;
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate::RESTARTING;
			engine::scorefile.playtime += m_game->result();
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate::SAVING_AND_QUITTING;
			set_up_exit_animation();
		},
		[this] {
			m_timer = 0;
			m_substate = substate::QUITTING;
			engine::scorefile.playtime += m_game->result();
			set_up_exit_animation();
		},
	};
	for (std::size_t i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
		const glm::vec2 pos{500 + offset, 500 - (BUTTONS.size() + 3) * 30 + (i + 4) * 60};
		widget& widget{m_ui.emplace<clickable_text_widget>(BUTTONS[i], pos, tr::align::CENTER, font::LANGUAGE, 48, DEFAULT_TEXT_CALLBACK,
														   status_cb, std::move(action_cbs[i]), NO_TOOLTIP, SHORTCUTS[i])};
		widget.pos.change({500, pos.y}, 0.5_s);
		widget.unhide(0.5_s);
	}
}

///////////////////////////////////////////////////////////////// METHODS /////////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> game_over_state::handle_event(const tr::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> game_over_state::update(tr::duration)
{
	++m_timer;
	m_ui.update();
	m_game->update();

	switch (m_substate) {
	case substate::BLURRING_IN:
		if (m_timer >= 0.5_s) {
			m_timer = 0;
			m_substate = substate::GAME_OVER;
		}
		[[fallthrough]];
	case substate::GAME_OVER:
		if (m_prev_pb < m_game->result()) {
			if (m_timer % 0.5_s == 0) {
				m_ui.get("pb").hide();
			}
			else if (m_timer % 0.5_s == 0.25_s) {
				m_ui.get("pb").unhide();
			}
		}
		return nullptr;
	case substate::RESTARTING:
		return m_timer >= 0.5_s ? std::make_unique<game_state>(std::make_unique<active_game>(m_game->gamemode()), game_type::REGULAR, true)
								: nullptr;
	case substate::SAVING_AND_RESTARTING:
	case substate::SAVING_AND_QUITTING: {
		if (m_timer >= 0.5_s) {
			const save_screen_flags state_flags{m_substate == substate::SAVING_AND_RESTARTING ? save_screen_flags::RESTARTING
																							  : save_screen_flags::NONE};
			return std::make_unique<save_score_state>(std::move(m_game), m_prev_pb, state_flags);
		}
		else {
			return nullptr;
		}
	}
	case substate::QUITTING:
		return m_timer >= 0.5_s ? std::make_unique<title_state>() : nullptr;
	}
}

void game_over_state::draw()
{
	m_game->add_to_renderer();
	tr::renderer_2d::draw(engine::blur().input());
	engine::blur().draw(saturation_factor(), blur_strength());
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(m_substate == substate::RESTARTING || m_substate == substate::QUITTING ? m_timer / 0.5_sf : 0);
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float game_over_state::saturation_factor() const
{
	switch (m_substate) {
	case substate::GAME_OVER:
	case substate::SAVING_AND_RESTARTING:
	case substate::RESTARTING:
	case substate::SAVING_AND_QUITTING:
	case substate::QUITTING:
		return 0.35f;
	case substate::BLURRING_IN:
		return 1 - m_timer / 0.5_sf * 0.65f;
	}
}

float game_over_state::blur_strength() const
{
	switch (m_substate) {
	case substate::GAME_OVER:
	case substate::SAVING_AND_RESTARTING:
	case substate::RESTARTING:
	case substate::SAVING_AND_QUITTING:
	case substate::QUITTING:
		return 10;
	case substate::BLURRING_IN:
		return m_timer / 0.5_sf * 10;
	}
}

void game_over_state::set_up_exit_animation()
{
	m_ui.get("game_over").pos.change({500, TITLE_Y - 100}, 0.5_s);
	widget& time{m_ui.get("time")};
	widget& pb{m_ui.get("pb")};
	time.pos.change({400, glm::vec2{time.pos}.y}, 0.5_s);
	pb.pos.change({600, glm::vec2{pb.pos}.y}, 0.5_s);
	for (std::size_t i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 != 0 ? -1.0f : 1.0f) * engine::rng.generate(50.0f, 150.0f)};
		widget& widget{m_ui.get(BUTTONS[i])};
		widget.pos.change(glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
	}
	m_ui.hide_all(0.5_s);
}