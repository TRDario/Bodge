#include "../../include/audio.hpp"
#include "../../include/graphics.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/replays_state.hpp"
#include "../../include/state/scoreboards_state.hpp"
#include "../../include/state/settings_state.hpp"
#include "../../include/state/start_game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr const char* TAG_LOGO_TEXT{"logo_text"};
constexpr const char* TAG_LOGO_OVERLAY{"logo_overlay"};
constexpr const char* TAG_LOGO_BALL{"logo_ball"};
constexpr const char* TAG_COPYRIGHT{"copyright"};
constexpr const char* TAG_VERSION{"version"};
constexpr const char* TAG_START_GAME{"start_game"};
constexpr const char* TAG_GAMEMODE_DESIGNER{"gamemode_designer"};
constexpr const char* TAG_SCOREBOARDS{"scoreboards"};
constexpr const char* TAG_REPLAYS{"replays"};
constexpr const char* TAG_SETTINGS{"settings"};
constexpr const char* TAG_CREDITS{"credits"};
constexpr const char* TAG_EXIT{"exit"};

// Title screen buttons.
constexpr std::array<const char*, 7> BUTTONS{
	TAG_START_GAME, TAG_GAMEMODE_DESIGNER, TAG_SCOREBOARDS, TAG_REPLAYS, TAG_SETTINGS, TAG_CREDITS, TAG_EXIT,
};
// Shortcuts of the title screen buttons.
constexpr std::array<std::initializer_list<tr::system::key_chord>, BUTTONS.size()> SHORTCUTS{{
	{{tr::system::keycode::ENTER}, {tr::system::keycode::TOP_ROW_1}},
	{{tr::system::keycode::G}, {tr::system::keycode::TOP_ROW_2}},
	{{tr::system::keycode::B}, {tr::system::keycode::TOP_ROW_3}},
	{{tr::system::keycode::R}, {tr::system::keycode::TOP_ROW_4}},
	{{tr::system::keycode::S}, {tr::system::keycode::TOP_ROW_5}},
	{{tr::system::keycode::C}, {tr::system::keycode::TOP_ROW_6}},
	{{tr::system::keycode::ESCAPE}, {tr::system::keycode::Q}, {tr::system::keycode::E}, {tr::system::keycode::TOP_ROW_7}},
}};

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

title_state::title_state()
	: m_substate{substate::ENTERING_GAME}
	, m_timer{0}
	, m_background_game{std::make_unique<game>(pick_menu_gamemode(), engine::rng.generate<std::uint64_t>())}
{
	set_up_ui();
	engine::play_song("menu", 1.0s);
}

title_state::title_state(std::unique_ptr<game>&& game)
	: m_substate{substate::IN_TITLE}, m_timer{0}, m_background_game{std::move(game)}
{
	set_up_ui();
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> title_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> title_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

	switch (m_substate) {
	case substate::ENTERING_GAME:
		if (m_timer >= 1.0_s) {
			m_timer = 0;
			m_substate = substate::IN_TITLE;
		}
		return nullptr;
	case substate::IN_TITLE:
		return nullptr;
	case substate::ENTERING_START_GAME:
		return m_timer >= 0.5_s ? std::make_unique<start_game_state>(std::move(m_background_game)) : nullptr;
	case substate::ENTERING_GAMEMODE_DESIGNER:
		return m_timer >= 0.5_s ? std::make_unique<gamemode_designer_state>(std::move(m_background_game),
																			gamemode{.author = engine::scorefile.name}, false)
								: nullptr;
	case substate::ENTERING_SCOREBOARDS:
		return m_timer >= 0.5_s ? std::make_unique<scoreboards_state>(std::move(m_background_game)) : nullptr;
	case substate::ENTERING_REPLAYS:
		return m_timer >= 0.5_s ? std::make_unique<replays_state>(std::move(m_background_game)) : nullptr;
	case substate::ENTERING_SETTINGS:
		return m_timer >= 0.5_s ? std::make_unique<settings_state>(std::move(m_background_game)) : nullptr;
	case substate::EXITING_GAME:
		return m_timer >= 0.5_s ? std::make_unique<tr::drop_state>() : nullptr;
	}
}

void title_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

float title_state::fade_overlay_opacity() const
{
	switch (m_substate) {
	case substate::ENTERING_GAME:
		return 1 - m_timer / 1_sf;
	case substate::IN_TITLE:
	case substate::ENTERING_START_GAME:
	case substate::ENTERING_GAMEMODE_DESIGNER:
	case substate::ENTERING_REPLAYS:
	case substate::ENTERING_SCOREBOARDS:
	case substate::ENTERING_SETTINGS:
		return 0;
	case substate::EXITING_GAME:
		return m_timer / 0.5_sf;
	}
}

void title_state::set_up_ui()
{
	widget& logo_text{m_ui.emplace<image_widget>(TAG_LOGO_TEXT, glm::vec2{500, 100}, tr::align::CENTER)};
	logo_text.pos.change(interp_mode::CUBE, {500, 160}, 2.5_s);
	logo_text.unhide(2.5_s);
	widget& logo_overlay{
		m_ui.emplace<image_widget>(TAG_LOGO_OVERLAY, glm::vec2{500, 100}, tr::align::CENTER, &engine::settings.primary_hue)};
	logo_overlay.pos.change(interp_mode::CUBE, {500, 160}, 2.5_s);
	logo_overlay.unhide(2.5_s);
	widget& logo_ball{m_ui.emplace<image_widget>(TAG_LOGO_BALL, glm::vec2{-180, 644}, tr::align::CENTER, &engine::settings.secondary_hue)};
	logo_ball.pos.change(interp_mode::CUBE, {327, 217}, 2.5_s);
	logo_ball.unhide(2.5_s);

	widget& copyright{m_ui.emplace<text_widget>(TAG_COPYRIGHT, glm::vec2{4, 1000}, tr::align::TOP_LEFT, font::DEFAULT,
												tr::system::ttf_style::NORMAL, 24)};
	copyright.pos.change(interp_mode::CUBE, {4, 998 - copyright.size().y}, 1_s);
	copyright.unhide(1_s);
	widget& version{m_ui.emplace<text_widget>(TAG_VERSION, glm::vec2{996, 1000}, tr::align::TOP_RIGHT, font::DEFAULT,
											  tr::system::ttf_style::NORMAL, 24)};
	version.pos.change(interp_mode::CUBE, {996, 998 - version.size().y}, 1_s);
	version.unhide(1_s);

	const status_callback status_cb{[this] { return m_substate == substate::IN_TITLE || m_substate == substate::ENTERING_GAME; }};
	const std::array<action_callback, BUTTONS.size()> action_cbs{
		[this] {
			m_substate = substate::ENTERING_START_GAME;
			m_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			m_substate = substate::ENTERING_GAMEMODE_DESIGNER;
			m_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			m_substate = substate::ENTERING_SCOREBOARDS;
			m_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			m_substate = substate::ENTERING_REPLAYS;
			m_timer = 0;
			set_up_exit_animation();
		},
		[this] {
			m_substate = substate::ENTERING_SETTINGS;
			m_timer = 0;
			set_up_exit_animation();
		},
		[] { engine::play_sound(sound::CONFIRM, 0.5f, 0.0f); },
		[this] {
			m_substate = substate::EXITING_GAME;
			m_timer = 0;
			set_up_exit_animation();
			engine::fade_song_out(0.5s);
		},
	};

	glm::vec2 end_pos{990, 965 - (BUTTONS.size() - 1) * 50};
	for (std::size_t i = 0; i < BUTTONS.size(); ++i) {
		const float offset{(i % 2 == 0 ? -1.0f : 1.0f) * engine::rng.generate(35.0f, 75.0f)};
		const glm::vec2 pos{end_pos.x + offset, end_pos.y};
		widget& widget{m_ui.emplace<clickable_text_widget>(BUTTONS[i], pos, tr::align::CENTER_RIGHT, font::LANGUAGE, 48,
														   DEFAULT_TEXT_CALLBACK, status_cb, action_cbs[i], NO_TOOLTIP, SHORTCUTS[i],
														   i != BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL)};
		widget.pos.change(interp_mode::CUBE, end_pos, 1_s);
		widget.unhide(1_s);
		end_pos += glm::vec2{-25, 50};
	}
}

void title_state::set_up_exit_animation()
{
	int i = 0;
	for (const char* tag : BUTTONS) {
		const float offset{(i++ % 2 != 0 ? -1.0f : 1.0f) * engine::rng.generate(35.0f, 75.0f)};
		widget& widget{m_ui.get(tag)};
		widget.pos.change(interp_mode::CUBE, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
	}
	m_ui.get(TAG_LOGO_TEXT).pos.change(interp_mode::CUBE, {500, 220}, 0.5_s);
	m_ui.get(TAG_LOGO_OVERLAY).pos.change(interp_mode::CUBE, {500, 220}, 0.5_s);
	m_ui.get(TAG_LOGO_BALL).pos.change(interp_mode::CUBE, {487, 57}, 0.5_s);
	m_ui.get(TAG_COPYRIGHT).pos.change(interp_mode::CUBE, {4, 1000}, 0.5_s);
	m_ui.get(TAG_VERSION).pos.change(interp_mode::CUBE, {996, 1000}, 0.5_s);
	m_ui.hide_all(0.5_s);
}