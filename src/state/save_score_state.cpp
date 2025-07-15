#include "../../include/state/save_score_state.hpp"
#include "../../include/state/game_over_state.hpp"
#include "../../include/state/pause_state.hpp"
#include "../../include/state/save_replay_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Shortcuts of the save button.
constexpr std::initializer_list<tr::key_chord> SAVE_SHORTCUTS{{tr::keycode::ENTER}, {tr::keycode::S}, {tr::keycode::TOP_ROW_1}};
// Shortcuts of the cancel button.
constexpr std::initializer_list<tr::key_chord> CANCEL_SHORTCUTS{{tr::keycode::ESCAPE}, {tr::keycode::C}, {tr::keycode::TOP_ROW_2}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

save_score_state::save_score_state(std::unique_ptr<active_game>&& game, glm::vec2 mouse_pos, save_screen_flags flags)
	: _substate{substate_base::SAVING_SCORE | flags}
	, _substate_data{.mouse_pos = mouse_pos}
	, _timer{0}
	, _game{std::move(game)}
	, _score{{}, unix_now(), _game->result(), {!_game->game_over(), cli_settings.game_speed != 1.0f}}
{
	set_up_ui();
}

save_score_state::save_score_state(std::unique_ptr<active_game>&& game, ticks prev_pb, save_screen_flags flags)
	: _substate{substate_base::SAVING_SCORE | flags}
	, _substate_data{.prev_pb = prev_pb}
	, _timer{0}
	, _game{std::move(game)}
	, _score{{}, unix_now(), _game->result(), {!_game->game_over(), cli_settings.game_speed != 1.0f}}
{
	set_up_ui();
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> save_score_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> save_score_state::update(tr::duration)
{
	++_timer;
	_ui.update();
	if (to_flags(_substate) & save_screen_flags::GAME_OVER) {
		_game->update();
	}
	_score.description = _ui.get<multiline_input_widget<255>>("input").buffer;

	switch (to_base(_substate)) {
	case substate_base::SAVING_SCORE:
		return nullptr;
	case substate_base::RETURNING:
		if (_timer >= 0.5_s) {
			if (to_flags(_substate) & save_screen_flags::GAME_OVER) {
				return std::make_unique<game_over_state>(std::move(_game), false, _substate_data.prev_pb);
			}
			else {
				return std::make_unique<pause_state>(std::move(_game), game_type::REGULAR, _substate_data.mouse_pos, false);
			}
		}
		else {
			return nullptr;
		}
	case substate_base::ENTERING_SAVE_REPLAY:
		return _timer >= 0.5_s ? std::make_unique<save_replay_state>(std::move(_game), to_flags(_substate)) : nullptr;
	}
}

void save_score_state::draw()
{
	if (to_flags(_substate) & save_screen_flags::GAME_OVER) {
		_game->add_to_renderer();
		tr::renderer_2d::draw(engine::blur_renderer().input());
	}
	engine::blur_renderer().draw(0.35f, 10.0f);
	_ui.add_to_renderer();
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

save_score_state::substate operator|(const save_score_state::substate_base& l, const save_screen_flags& r) noexcept
{
	return static_cast<save_score_state::substate>(static_cast<int>(l) | static_cast<int>(r));
}

save_score_state::substate_base to_base(save_score_state::substate state) noexcept
{
	return static_cast<save_score_state::substate_base>(static_cast<int>(state) & 0x3);
}

save_screen_flags to_flags(save_score_state::substate state) noexcept
{
	return static_cast<save_screen_flags>(static_cast<int>(state) & static_cast<int>(save_screen_flags::MASK));
}

void save_score_state::set_up_ui()
{
	const status_callback status_cb{[this] { return to_base(_substate) == substate_base::SAVING_SCORE; }};

	const action_callback save_action_cb{[this] {
		_substate = substate_base::ENTERING_SAVE_REPLAY | to_flags(_substate);
		_timer = 0;
		set_up_exit_animation();
		scorefile.playtime += _score.result;
		scorefile.add_score(_game->gamemode(), _score);
		scorefile.update_category(_game->gamemode(), _game->result());
	}};
	const action_callback cancel_action_cb{[this] {
		_substate = substate_base::RETURNING | to_flags(_substate);
		_timer = 0;
		set_up_exit_animation();
	}};

	widget& title{_ui.emplace<text_widget>("save_score", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	widget& preview_label{
		_ui.emplace<text_widget>("preview", glm::vec2{400, 200}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
	preview_label.pos.change({500, 200}, 0.5_s);
	preview_label.unhide(0.5_s);

	widget& preview{_ui.emplace<score_widget>("score", glm::vec2{400, 235}, tr::align::TOP_CENTER, score_widget::DONT_SHOW_RANK, &_score)};
	preview.pos.change({500, 235}, 0.5_s);
	preview.unhide(0.5_s);

	widget& description_label{
		_ui.emplace<text_widget>("description", glm::vec2{600, 440}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
	description_label.pos.change({500, 440}, 0.5_s);
	description_label.unhide(0.5_s);

	widget& description{_ui.emplace<multiline_input_widget<255>>("input", glm::vec2{600, 475}, tr::align::TOP_CENTER, 800, 10,
																 tr::halign::CENTER, 24, status_cb)};
	description.pos.change({500, 475}, 0.5_s);
	description.unhide(0.5_s);

	widget& save{_ui.emplace<clickable_text_widget>("save", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													DEFAULT_TEXT_CALLBACK, status_cb, save_action_cb, NO_TOOLTIP, SAVE_SHORTCUTS)};
	save.pos.change({500, 950}, 0.5_s);
	save.unhide(0.5_s);

	widget& cancel{_ui.emplace<clickable_text_widget>("cancel", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													  DEFAULT_TEXT_CALLBACK, status_cb, cancel_action_cb, NO_TOOLTIP, CANCEL_SHORTCUTS,
													  sfx::CANCEL)};
	cancel.pos.change({500, 1000}, 0.5_s);
	cancel.unhide(0.5_s);
}

void save_score_state::set_up_exit_animation() noexcept
{
	_ui.get("save_score").pos.change(TOP_START_POS, 0.5_s);
	_ui.get("preview").pos.change({600, 200}, 0.5_s);
	_ui.get("score").pos.change({600, 235}, 0.5_s);
	_ui.get("description").pos.change({400, 440}, 0.5_s);
	_ui.get("input").pos.change({400, 475}, 0.5_s);
	_ui.get("save").pos.change(BOTTOM_START_POS, 0.5_s);
	_ui.get("cancel").pos.change(BOTTOM_START_POS, 0.5_s);
	_ui.hide_all(0.5_s);
}