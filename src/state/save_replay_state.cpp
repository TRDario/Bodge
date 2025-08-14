#include "../../include/state/save_replay_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"save_replay"};
constexpr tag T_NAME{"name"};
constexpr tag T_NAME_INPUT{"name_input"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_DESCRIPTION_INPUT{"description_input"};
constexpr tag T_SAVE{"save"};
constexpr tag T_DISCARD{"discard"};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_NAME_INPUT},
	selection_tree_row{T_DESCRIPTION_INPUT},
	selection_tree_row{T_SAVE},
	selection_tree_row{T_DISCARD},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::ENTER}, T_SAVE},     {{tr::system::keycode::S}, T_SAVE},    {{tr::system::keycode::TOP_ROW_1}, T_SAVE},
	{{tr::system::keycode::ESCAPE}, T_DISCARD}, {{tr::system::keycode::C}, T_DISCARD}, {{tr::system::keycode::TOP_ROW_2}, T_DISCARD},
};

constexpr interpolator<glm::vec2> TITLE_MOVE_IN{interp::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr interpolator<glm::vec2> NAME_MOVE_IN{interp::CUBIC, {400, 200}, {500, 200}, 0.5_s};
constexpr interpolator<glm::vec2> NAME_INPUT_MOVE_IN{interp::CUBIC, {400, 235}, {500, 235}, 0.5_s};
constexpr interpolator<glm::vec2> DESCRIPTION_MOVE_IN{interp::CUBIC, {600, 440}, {500, 440}, 0.5_s};
constexpr interpolator<glm::vec2> DESCRIPTION_INPUT_MOVE_IN{interp::CUBIC, {600, 475}, {500, 475}, 0.5_s};
constexpr interpolator<glm::vec2> SAVE_MOVE_IN{interp::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr interpolator<glm::vec2> DISCARD_MOVE_IN{interp::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

save_replay_state::save_replay_state(std::unique_ptr<active_game>&& game, save_screen_flags flags)
	: m_substate{substate_base::SAVING_REPLAY | flags}
	, m_timer{0}
	, m_ui{SELECTION_TREE, SHORTCUTS}
	, m_game{std::move(game)}
	, m_replay{m_game->replay().header()}
{
	// STATUS CALLBACKS

	const status_callback scb{[this] { return to_base(m_substate) == substate_base::SAVING_REPLAY; }};
	const status_callback save_scb{[this] {
		return to_base(m_substate) == substate_base::SAVING_REPLAY && !m_ui.as<line_input_widget<20>>(T_NAME_INPUT).buffer.empty();
	}};

	// ACTION CALLBACKS

	const action_callback name_acb{[this] { m_ui.select_next(); }};
	const action_callback save_acb{[this] {
		const score_flags flags{!m_game->game_over(), engine::cli_settings.game_speed != 1.0f};

		m_substate = substate_base::EXITING | to_flags(m_substate);
		m_timer = 0;
		set_up_exit_animation();
		m_game->replay().set_header({m_ui.as<multiline_input_widget<255>>(T_DESCRIPTION_INPUT).buffer, unix_now(), m_game->result(), flags},
									m_ui.as<line_input_widget<20>>("name_input").buffer);
		m_game->replay().save_to_file();
	}};
	const action_callback discard_acb{[this] {
		m_substate = substate_base::EXITING | to_flags(m_substate);
		m_timer = 0;
		set_up_exit_animation();
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_NAME, NAME_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_NAME},
							   tr::system::ttf_style::NORMAL, 48);
	m_ui.emplace<line_input_widget<20>>(T_NAME_INPUT, NAME_INPUT_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, tr::system::ttf_style::NORMAL, 64,
										scb, name_acb, std::string_view{});
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_DESCRIPTION},
							   tr::system::ttf_style::NORMAL, 48);
	m_ui.emplace<multiline_input_widget<255>>(T_DESCRIPTION_INPUT, DESCRIPTION_INPUT_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, 800, 10,
											  tr::halign::CENTER, 24, scb);
	m_ui.emplace<text_button_widget>(T_SAVE, SAVE_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SAVE},
									 font::LANGUAGE, 48, save_scb, save_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_DISCARD, DISCARD_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_DISCARD},
									 font::LANGUAGE, 48, scb, discard_acb, sound::CONFIRM);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> save_replay_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> save_replay_state::update(tr::duration)
{
	++m_timer;
	m_ui.update();
	if (to_flags(m_substate) & save_screen_flags::GAME_OVER) {
		m_game->update();
	}

	if (m_timer >= 0.5_s && to_base(m_substate) == substate_base::EXITING) {
		tr::gfx::renderer_2d::set_default_transform(TRANSFORM);
		if (to_flags(m_substate) & save_screen_flags::RESTARTING) {
			return std::make_unique<game_state>(std::make_unique<active_game>(m_game->gamemode()), game_type::REGULAR, true);
		}
		else {
			return std::make_unique<title_state>();
		}
	}
	else {
		return nullptr;
	}
}

void save_replay_state::draw()
{
	if (to_flags(m_substate) & save_screen_flags::GAME_OVER) {
		m_game->add_to_renderer();
		tr::gfx::renderer_2d::draw(engine::blur().input());
	}
	engine::blur().draw(0.35f, 10.0f);
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

save_replay_state::substate operator|(const save_replay_state::substate_base& l, const save_screen_flags& r)
{
	return static_cast<save_replay_state::substate>(static_cast<int>(l) | static_cast<int>(r));
}

save_replay_state::substate_base to_base(save_replay_state::substate state)
{
	return static_cast<save_replay_state::substate_base>(static_cast<int>(state) & 0x1);
}

save_screen_flags to_flags(save_replay_state::substate state)
{
	return static_cast<save_screen_flags>(static_cast<int>(state) & static_cast<int>(save_screen_flags::MASK));
}

float save_replay_state::fade_overlay_opacity() const
{
	if (to_base(m_substate) == substate_base::EXITING) {
		return m_timer / 0.5_sf;
	}
	else {
		return 0;
	}
}

void save_replay_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(interp::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_NAME].pos.change(interp::CUBIC, {600, 200}, 0.5_s);
	m_ui[T_NAME_INPUT].pos.change(interp::CUBIC, {600, 235}, 0.5_s);
	m_ui[T_DESCRIPTION].pos.change(interp::CUBIC, {400, 440}, 0.5_s);
	m_ui[T_DESCRIPTION_INPUT].pos.change(interp::CUBIC, {400, 475}, 0.5_s);
	m_ui[T_SAVE].pos.change(interp::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui[T_DISCARD].pos.change(interp::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all(0.5_s);
}