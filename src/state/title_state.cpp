#include "../../include/state/title_state.hpp"
#include "../../include/audio.hpp"
#include "../../include/state/credits_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/state/replays_state.hpp"
#include "../../include/state/scoreboards_state.hpp"
#include "../../include/state/settings_state.hpp"
#include "../../include/state/start_game_state.hpp"
#include "../../include/ui/widget.hpp"

// clang-format off

constexpr tag T_LOGO_TEXT{"logo_text"};
constexpr tag T_LOGO_OVERLAY{"logo_overlay"};
constexpr tag T_LOGO_BALL{"logo_ball"};
constexpr tag T_COPYRIGHT{"©2025 TRDario"};
constexpr tag T_VERSION{VERSION_STRING};
constexpr tag T_START_GAME{"start_game"};
constexpr tag T_GAMEMODE_DESIGNER{"gamemode_designer"};
constexpr tag T_SCOREBOARDS{"scoreboards"};
constexpr tag T_REPLAYS{"replays"};
constexpr tag T_SETTINGS{"settings"};
constexpr tag T_CREDITS{"credits"};
constexpr tag T_EXIT{"exit"};

constexpr std::array<tag, 7> BUTTONS{T_START_GAME, T_GAMEMODE_DESIGNER, T_SCOREBOARDS, T_REPLAYS, T_SETTINGS, T_CREDITS, T_EXIT};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_START_GAME},
	selection_tree_row{T_GAMEMODE_DESIGNER},
	selection_tree_row{T_SCOREBOARDS},
	selection_tree_row{T_REPLAYS},
	selection_tree_row{T_SETTINGS},
	selection_tree_row{T_CREDITS},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::ENTER}, T_START_GAME},
	{{tr::system::keycode::TOP_ROW_1}, T_START_GAME},
	{{tr::system::keycode::G}, T_GAMEMODE_DESIGNER},
	{{tr::system::keycode::TOP_ROW_2}, T_GAMEMODE_DESIGNER},
	{{tr::system::keycode::B}, T_SCOREBOARDS},
	 {{tr::system::keycode::TOP_ROW_3}, T_SCOREBOARDS},
	{{tr::system::keycode::R}, T_REPLAYS},
	{{tr::system::keycode::TOP_ROW_4}, T_REPLAYS},
	{{tr::system::keycode::S}, T_SETTINGS},
	{{tr::system::keycode::TOP_ROW_5}, T_SETTINGS},
	{{tr::system::keycode::ESCAPE}, T_EXIT},
	{{tr::system::keycode::TOP_ROW_6}, T_EXIT},
};

constexpr tweener<glm::vec2> LOGO_TEXT_MOVE_IN{tween::CUBIC, {500, 100}, {500, 160}, 2.5_s};
constexpr tweener<glm::vec2> LOGO_BALL_MOVE_IN{tween::CUBIC, {-180, 644}, {327, 217}, 2.5_s};

// clang-format on

title_state::title_state()
	: menu_state{SELECTION_TREE, SHORTCUTS}, m_substate{substate::ENTERING_GAME}
{
	set_up_ui();
	engine::play_song("menu", 1.0s);
}

title_state::title_state(std::unique_ptr<game>&& game)
	: menu_state{SELECTION_TREE, SHORTCUTS, std::move(game)}, m_substate{substate::IN_TITLE}
{
	set_up_ui();
}

//

std::unique_ptr<tr::state> title_state::update(tr::duration)
{
	menu_state::update({});
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
		return m_timer >= 0.5_s ? std::make_unique<start_game_state>(release_game()) : nullptr;
	case substate::ENTERING_GAMEMODE_DESIGNER:
		return m_timer >= 0.5_s ? std::make_unique<gamemode_designer_state>(
									  release_game(), gamemode{.author = engine::scorefile.name, .song = "classic"}, false)
								: nullptr;
	case substate::ENTERING_SCOREBOARDS:
		return m_timer >= 0.5_s ? std::make_unique<scoreboards_state>(release_game()) : nullptr;
	case substate::ENTERING_REPLAYS:
		return m_timer >= 0.5_s ? std::make_unique<replays_state>(release_game()) : nullptr;
	case substate::ENTERING_SETTINGS:
		return m_timer >= 0.5_s ? std::make_unique<settings_state>(release_game()) : nullptr;
	case substate::ENTERING_CREDITS:
		return m_timer >= 0.5_s ? std::make_unique<credits_state>(release_game()) : nullptr;
	case substate::EXITING_GAME:
		return m_timer >= 0.5_s ? std::make_unique<tr::drop_state>() : nullptr;
	}
}

//

float title_state::fade_overlay_opacity()
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
	case substate::ENTERING_CREDITS:
		return 0;
	case substate::EXITING_GAME:
		return m_timer / 0.5_sf;
	}
}

void title_state::set_up_ui()
{
	m_ui.emplace<image_widget>(T_LOGO_TEXT, LOGO_TEXT_MOVE_IN, tr::align::CENTER, 2.5_s, 0, "logo_text");
	m_ui.emplace<image_widget>(T_LOGO_OVERLAY, LOGO_TEXT_MOVE_IN, tr::align::CENTER, 2.5_s, 1, "logo_overlay",
							   &engine::settings.primary_hue);
	m_ui.emplace<image_widget>(T_LOGO_BALL, LOGO_BALL_MOVE_IN, tr::align::CENTER, 2.5_s, 2, "logo_ball", &engine::settings.secondary_hue);

	widget& copyright{m_ui.emplace<label_widget>(T_COPYRIGHT, glm::vec2{4, 1000}, tr::align::TOP_LEFT, 1_s, NO_TOOLTIP,
												 string_text_callback{T_COPYRIGHT}, tr::system::ttf_style::NORMAL, 24)};
	copyright.pos.change(tween::CUBIC, {4, 998 - copyright.size().y}, 1_s);
	widget& version{m_ui.emplace<label_widget>(T_VERSION, glm::vec2{996, 1000}, tr::align::TOP_RIGHT, 1_s, NO_TOOLTIP,
											   string_text_callback{T_VERSION}, tr::system::ttf_style::NORMAL, 24)};
	version.pos.change(tween::CUBIC, {996, 998 - version.size().y}, 1_s);

	const status_callback scb{
		[this] { return m_substate == substate::IN_TITLE || m_substate == substate::ENTERING_GAME; },
	};
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
		[this] {
			m_substate = substate::ENTERING_CREDITS;
			m_timer = 0;
			set_up_exit_animation();
		},
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
		const tweener<glm::vec2> move_in{tween::CUBIC, {end_pos.x + offset, end_pos.y}, end_pos, 1_s};
		m_ui.emplace<text_button_widget>(BUTTONS[i], move_in, tr::align::CENTER_RIGHT, 1_s, NO_TOOLTIP, loc_text_callback{BUTTONS[i]},
										 font::LANGUAGE, 48, scb, action_cbs[i], i != BUTTONS.size() - 1 ? sound::CONFIRM : sound::CANCEL);
		end_pos += glm::vec2{-25, 50};
	}
}

void title_state::set_up_exit_animation()
{
	int i = 0;
	for (tag tag : BUTTONS) {
		const float offset{(i++ % 2 != 0 ? -1.0f : 1.0f) * engine::rng.generate(35.0f, 75.0f)};
		widget& widget{m_ui[tag]};
		widget.pos.change(tween::CUBIC, glm::vec2{widget.pos} + glm::vec2{offset, 0}, 0.5_s);
	}
	m_ui[T_LOGO_TEXT].pos.change(tween::CUBIC, {500, 220}, 0.5_s);
	m_ui[T_LOGO_OVERLAY].pos.change(tween::CUBIC, {500, 220}, 0.5_s);
	m_ui[T_LOGO_BALL].pos.change(tween::CUBIC, {487, 57}, 0.5_s);
	m_ui[T_COPYRIGHT].pos.change(tween::CUBIC, {4, 1000}, 0.5_s);
	m_ui[T_VERSION].pos.change(tween::CUBIC, {996, 1000}, 0.5_s);
	m_ui.hide_all(0.5_s);
}