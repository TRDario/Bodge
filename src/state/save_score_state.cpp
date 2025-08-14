#include "../../include/state/save_score_state.hpp"
#include "../../include/state/game_over_state.hpp"
#include "../../include/state/pause_state.hpp"
#include "../../include/state/save_replay_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"save_score"};
constexpr tag T_PREVIEW{"preview"};
constexpr tag T_SCORE{"score"};
constexpr tag T_DESCRIPTION{"description"};
constexpr tag T_INPUT{"input"};
constexpr tag T_SAVE{"save"};
constexpr tag T_CANCEL{"cancel"};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_INPUT},
	selection_tree_row{T_SAVE},
	selection_tree_row{T_CANCEL},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::ENTER}, T_SAVE},    {{tr::system::keycode::S}, T_SAVE},   {{tr::system::keycode::TOP_ROW_1}, T_SAVE},
	{{tr::system::keycode::ESCAPE}, T_CANCEL}, {{tr::system::keycode::C}, T_CANCEL}, {{tr::system::keycode::TOP_ROW_2}, T_CANCEL},
};

constexpr interpolator<glm::vec2> TITLE_MOVE_IN{interp::CUBIC, TOP_START_POS, TITLE_POS, 0.5_s};
constexpr interpolator<glm::vec2> PREVIEW_MOVE_IN{interp::CUBIC, {400, 200}, {500, 200}, 0.5_s};
constexpr interpolator<glm::vec2> SCORE_MOVE_IN{interp::CUBIC, {400, 235}, {500, 235}, 0.5_s};
constexpr interpolator<glm::vec2> DESCRIPTION_MOVE_IN{interp::CUBIC, {600, 440}, {500, 440}, 0.5_s};
constexpr interpolator<glm::vec2> DESCRIPTION_INPUT_MOVE_IN{interp::CUBIC, {600, 475}, {500, 475}, 0.5_s};
constexpr interpolator<glm::vec2> SAVE_MOVE_IN{interp::CUBIC, BOTTOM_START_POS, {500, 950}, 0.5_s};
constexpr interpolator<glm::vec2> CANCEL_MOVE_IN{interp::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

save_score_state::save_score_state(std::unique_ptr<active_game>&& game, glm::vec2 mouse_pos, save_screen_flags flags)
	: m_substate{substate_base::SAVING_SCORE | flags}
	, m_substate_data{.mouse_pos = mouse_pos}
	, m_timer{0}
	, m_ui{SELECTION_TREE, SHORTCUTS}
	, m_game{std::move(game)}
	, m_score{{}, unix_now(), m_game->result(), {!m_game->game_over(), engine::cli_settings.game_speed != 1.0f}}
{
	set_up_ui();
}

save_score_state::save_score_state(std::unique_ptr<active_game>&& game, ticks prev_pb, save_screen_flags flags)
	: m_substate{substate_base::SAVING_SCORE | (flags | save_screen_flags::GAME_OVER)}
	, m_substate_data{.prev_pb = prev_pb}
	, m_timer{0}
	, m_ui{SELECTION_TREE, SHORTCUTS}
	, m_game{std::move(game)}
	, m_score{{}, unix_now(), m_game->result(), {!m_game->game_over(), engine::cli_settings.game_speed != 1.0f}}
{
	set_up_ui();
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> save_score_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> save_score_state::update(tr::duration)
{
	++m_timer;
	m_ui.update();
	if (to_flags(m_substate) & save_screen_flags::GAME_OVER) {
		m_game->update();
	}
	m_score.description = m_ui.as<multiline_input_widget<255>>(T_INPUT).buffer;

	switch (to_base(m_substate)) {
	case substate_base::SAVING_SCORE:
		return nullptr;
	case substate_base::RETURNING:
		if (m_timer >= 0.5_s) {
			if (to_flags(m_substate) & save_screen_flags::GAME_OVER) {
				return std::make_unique<game_over_state>(std::move(m_game), false, m_substate_data.prev_pb);
			}
			else {
				return std::make_unique<pause_state>(std::move(m_game), game_type::REGULAR, m_substate_data.mouse_pos, false);
			}
		}
		else {
			return nullptr;
		}
	case substate_base::ENTERING_SAVE_REPLAY:
		return m_timer >= 0.5_s ? std::make_unique<save_replay_state>(std::move(m_game), to_flags(m_substate)) : nullptr;
	}
}

void save_score_state::draw()
{
	if (to_flags(m_substate) & save_screen_flags::GAME_OVER) {
		m_game->add_to_renderer();
		tr::gfx::renderer_2d::draw(engine::blur().input());
	}
	engine::blur().draw(0.35f, 10.0f);
	m_ui.add_to_renderer();
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

save_score_state::substate operator|(const save_score_state::substate_base& l, const save_screen_flags& r)
{
	return static_cast<save_score_state::substate>(static_cast<int>(l) | static_cast<int>(r));
}

save_score_state::substate_base to_base(save_score_state::substate state)
{
	return static_cast<save_score_state::substate_base>(static_cast<int>(state) & 0x3);
}

save_screen_flags to_flags(save_score_state::substate state)
{
	return static_cast<save_screen_flags>(static_cast<int>(state) & static_cast<int>(save_screen_flags::MASK));
}

void save_score_state::set_up_ui()
{
	// STATUS CALLBACKS

	const status_callback scb{[this] { return to_base(m_substate) == substate_base::SAVING_SCORE; }};

	// ACTION CALLBACKS

	const action_callback save_acb{[this] {
		m_substate = substate_base::ENTERING_SAVE_REPLAY | to_flags(m_substate);
		m_timer = 0;
		set_up_exit_animation();
		engine::scorefile.playtime += m_score.result;
		add_score(engine::scorefile, m_game->gamemode(), m_score);
		update_pb(engine::scorefile, m_game->gamemode(), m_game->result());
	}};
	const action_callback cancel_acb{[this] {
		m_substate = substate_base::RETURNING | to_flags(m_substate);
		m_timer = 0;
		set_up_exit_animation();
	}};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_PREVIEW, PREVIEW_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_PREVIEW},
							   tr::system::ttf_style::NORMAL, 48);
	m_ui.emplace<score_widget>(T_SCORE, SCORE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, score_widget::DONT_SHOW_RANK, &m_score);
	m_ui.emplace<label_widget>(T_DESCRIPTION, DESCRIPTION_MOVE_IN, tr::align::CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_DESCRIPTION},
							   tr::system::ttf_style::NORMAL, 48);
	m_ui.emplace<multiline_input_widget<255>>(T_INPUT, DESCRIPTION_INPUT_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, 800, 10, tr::halign::CENTER,
											  24, scb);
	m_ui.emplace<text_button_widget>(T_SAVE, SAVE_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SAVE},
									 font::LANGUAGE, 48, scb, save_acb, sound::CONFIRM);
	m_ui.emplace<text_button_widget>(T_CANCEL, CANCEL_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_CANCEL},
									 font::LANGUAGE, 48, scb, cancel_acb, sound::CANCEL);
}

void save_score_state::set_up_exit_animation()
{
	m_ui[T_TITLE].pos.change(interp::CUBIC, TOP_START_POS, 0.5_s);
	m_ui[T_PREVIEW].pos.change(interp::CUBIC, {600, 200}, 0.5_s);
	m_ui[T_SCORE].pos.change(interp::CUBIC, {600, 235}, 0.5_s);
	m_ui[T_DESCRIPTION].pos.change(interp::CUBIC, {400, 440}, 0.5_s);
	m_ui[T_INPUT].pos.change(interp::CUBIC, {400, 475}, 0.5_s);
	m_ui[T_SAVE].pos.change(interp::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui[T_CANCEL].pos.change(interp::CUBIC, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all(0.5_s);
}