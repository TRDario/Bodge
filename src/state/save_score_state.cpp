#include "../../include/state/save_score_state.hpp"
#include "../../include/state/game_over_state.hpp"
#include "../../include/state/pause_state.hpp"
#include "../../include/state/save_replay_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag TAG_TITLE{"save_score"};
constexpr tag TAG_PREVIEW{"preview"};
constexpr tag TAG_SCORE{"score"};
constexpr tag TAG_DESCRIPTION{"description"};
constexpr tag TAG_INPUT{"input"};
constexpr tag TAG_SAVE{"save"};
constexpr tag TAG_CANCEL{"cancel"};

// Shortcuts of the save button.
constexpr std::initializer_list<tr::system::key_chord> SAVE_SHORTCUTS{
	{tr::system::keycode::ENTER}, {tr::system::keycode::S}, {tr::system::keycode::TOP_ROW_1}};
// Shortcuts of the cancel button.
constexpr std::initializer_list<tr::system::key_chord> CANCEL_SHORTCUTS{
	{tr::system::keycode::ESCAPE}, {tr::system::keycode::C}, {tr::system::keycode::TOP_ROW_2}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

save_score_state::save_score_state(std::unique_ptr<active_game>&& game, glm::vec2 mouse_pos, save_screen_flags flags)
	: m_substate{substate_base::SAVING_SCORE | flags}
	, m_substate_data{.mouse_pos = mouse_pos}
	, m_timer{0}
	, m_game{std::move(game)}
	, m_score{{}, unix_now(), m_game->result(), {!m_game->game_over(), engine::cli_settings.game_speed != 1.0f}}
{
	set_up_ui();
}

save_score_state::save_score_state(std::unique_ptr<active_game>&& game, ticks prev_pb, save_screen_flags flags)
	: m_substate{substate_base::SAVING_SCORE | (flags | save_screen_flags::GAME_OVER)}
	, m_substate_data{.prev_pb = prev_pb}
	, m_timer{0}
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
	m_score.description = m_ui.as<multiline_input_widget<255>>(TAG_INPUT).buffer;

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
	const status_callback status_cb{[this] { return to_base(m_substate) == substate_base::SAVING_SCORE; }};

	const action_callback save_action_cb{[this] {
		m_substate = substate_base::ENTERING_SAVE_REPLAY | to_flags(m_substate);
		m_timer = 0;
		set_up_exit_animation();
		engine::scorefile.playtime += m_score.result;
		add_score(engine::scorefile, m_game->gamemode(), m_score);
		update_pb(engine::scorefile, m_game->gamemode(), m_game->result());
	}};
	const action_callback cancel_action_cb{[this] {
		m_substate = substate_base::RETURNING | to_flags(m_substate);
		m_timer = 0;
		set_up_exit_animation();
	}};

	widget& title{m_ui.emplace<text_widget>(TAG_TITLE, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL,
											64, loc_text_callback{TAG_TITLE})};
	title.pos.change(interp_mode::CUBE, {500, 0}, 0.5_s);
	title.unhide(0.5_s);

	widget& preview_label{m_ui.emplace<text_widget>(TAG_PREVIEW, glm::vec2{400, 200}, tr::align::CENTER, font::LANGUAGE,
													tr::system::ttf_style::NORMAL, 48, loc_text_callback{TAG_PREVIEW})};
	preview_label.pos.change(interp_mode::CUBE, {500, 200}, 0.5_s);
	preview_label.unhide(0.5_s);

	widget& preview{
		m_ui.emplace<score_widget>(TAG_SCORE, glm::vec2{400, 235}, tr::align::TOP_CENTER, score_widget::DONT_SHOW_RANK, &m_score)};
	preview.pos.change(interp_mode::CUBE, {500, 235}, 0.5_s);
	preview.unhide(0.5_s);

	widget& description_label{m_ui.emplace<text_widget>(TAG_DESCRIPTION, glm::vec2{600, 440}, tr::align::CENTER, font::LANGUAGE,
														tr::system::ttf_style::NORMAL, 48, loc_text_callback{TAG_DESCRIPTION})};
	description_label.pos.change(interp_mode::CUBE, {500, 440}, 0.5_s);
	description_label.unhide(0.5_s);

	widget& description{m_ui.emplace<multiline_input_widget<255>>(TAG_INPUT, glm::vec2{600, 475}, tr::align::TOP_CENTER, 800, 10,
																  tr::halign::CENTER, 24, status_cb)};
	description.pos.change(interp_mode::CUBE, {500, 475}, 0.5_s);
	description.unhide(0.5_s);

	widget& save{m_ui.emplace<clickable_text_widget>(TAG_SAVE, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													 loc_text_callback{TAG_SAVE}, status_cb, save_action_cb, NO_TOOLTIP, SAVE_SHORTCUTS)};
	save.pos.change(interp_mode::CUBE, {500, 950}, 0.5_s);
	save.unhide(0.5_s);

	widget& cancel{m_ui.emplace<clickable_text_widget>(TAG_CANCEL, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													   loc_text_callback{TAG_CANCEL}, status_cb, cancel_action_cb, NO_TOOLTIP,
													   CANCEL_SHORTCUTS, sound::CANCEL)};
	cancel.pos.change(interp_mode::CUBE, {500, 1000}, 0.5_s);
	cancel.unhide(0.5_s);
}

void save_score_state::set_up_exit_animation()
{
	m_ui[TAG_TITLE].pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	m_ui[TAG_PREVIEW].pos.change(interp_mode::CUBE, {600, 200}, 0.5_s);
	m_ui[TAG_SCORE].pos.change(interp_mode::CUBE, {600, 235}, 0.5_s);
	m_ui[TAG_DESCRIPTION].pos.change(interp_mode::CUBE, {400, 440}, 0.5_s);
	m_ui[TAG_INPUT].pos.change(interp_mode::CUBE, {400, 475}, 0.5_s);
	m_ui[TAG_SAVE].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	m_ui[TAG_CANCEL].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all(0.5_s);
}