#include "../../include/state/save_replay_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Shortcuts of the save button.
constexpr std::initializer_list<tr::key_chord> SAVE_SHORTCUTS{{tr::keycode::ENTER}, {tr::keycode::S}, {tr::keycode::TOP_ROW_1}};
// Shortcuts of the don't save button.
constexpr std::initializer_list<tr::key_chord> DONT_SAVE_SHORTCUTS{{tr::keycode::ESCAPE}, {tr::keycode::C}, {tr::keycode::TOP_ROW_2}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

save_replay_state::save_replay_state(std::unique_ptr<active_game>&& game, save_screen_flags flags)
	: _substate{substate_base::SAVING_REPLAY | flags}, _timer{0}, _game{std::move(game)}, _replay{_game->replay().header()}
{
	const status_callback status_cb{[this] { return to_base(_substate) == substate_base::SAVING_REPLAY; }};

	widget& title{_ui.emplace<text_widget>("save_replay", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 0.5_s);
	title.unhide(0.5_s);

	widget& name_label{_ui.emplace<text_widget>("name", glm::vec2{400, 200}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
	name_label.pos.change({500, 200}, 0.5_s);
	name_label.unhide(0.5_s);

	const action_callback name_action_cb{[this] { _ui.move_input_focus_forward(); }};
	widget& name{_ui.emplace<line_input_widget<20>>("name_input", glm::vec2{400, 235}, tr::align::TOP_CENTER, tr::ttf_style::NORMAL, 64,
													status_cb, name_action_cb)};
	name.pos.change({500, 235}, 0.5_s);
	name.unhide(0.5_s);

	widget& description_label{
		_ui.emplace<text_widget>("description", glm::vec2{600, 440}, tr::align::CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 48)};
	description_label.pos.change({500, 440}, 0.5_s);
	description_label.unhide(0.5_s);

	widget& description{_ui.emplace<multiline_input_widget<255>>("description_input", glm::vec2{600, 475}, tr::align::TOP_CENTER, 800, 10,
																 tr::halign::CENTER, 24, status_cb)};
	description.pos.change({500, 475}, 0.5_s);
	description.unhide(0.5_s);

	const status_callback save_status_cb{[this] {
		return to_base(_substate) == substate_base::SAVING_REPLAY && !_ui.get<line_input_widget<20>>("name_input").buffer.empty();
	}};
	const action_callback save_action_cb{[this] {
		const score_flags flags{!_game->game_over(), cli_settings.game_speed != 1.0f};

		_substate = substate_base::EXITING | to_flags(_substate);
		_timer = 0;
		set_up_exit_animation();
		_game->replay().set_header({_ui.get<multiline_input_widget<255>>("description_input").buffer, unix_now(), _game->result(), flags},
								   _ui.get<line_input_widget<20>>("name_input").buffer);
		_game->replay().save_to_file();
	}};
	widget& save{_ui.emplace<clickable_text_widget>("save", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													DEFAULT_TEXT_CALLBACK, save_status_cb, save_action_cb, NO_TOOLTIP, SAVE_SHORTCUTS)};
	save.pos.change({500, 950}, 0.5_s);
	save.unhide(0.5_s);

	const action_callback dont_save_action_cb{[this] {
		_substate = substate_base::EXITING | to_flags(_substate);
		_timer = 0;
		set_up_exit_animation();
	}};
	widget& discard{_ui.emplace<clickable_text_widget>("discard", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													   DEFAULT_TEXT_CALLBACK, status_cb, dont_save_action_cb, NO_TOOLTIP,
													   DONT_SAVE_SHORTCUTS)};
	discard.pos.change({500, 1000}, 0.5_s);
	discard.unhide(0.5_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> save_replay_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> save_replay_state::update(tr::duration)
{
	++_timer;
	_ui.update();
	if (to_flags(_substate) & save_screen_flags::GAME_OVER) {
		_game->update();
	}

	if (_timer >= 0.5_s && to_base(_substate) == substate_base::EXITING) {
		if (to_flags(_substate) & save_screen_flags::RESTARTING) {
			return std::make_unique<game_state>(std::make_unique<active_game>(_game->gamemode()), game_type::REGULAR, true);
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
	if (to_flags(_substate) & save_screen_flags::GAME_OVER) {
		_game->add_to_renderer();
		tr::renderer_2d::draw(engine::blur_renderer().input());
	}
	engine::blur_renderer().draw(0.35f, 10.0f);
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(fade_overlay_opacity());
	tr::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

save_replay_state::substate operator|(const save_replay_state::substate_base& l, const save_screen_flags& r) noexcept
{
	return static_cast<save_replay_state::substate>(static_cast<int>(l) | static_cast<int>(r));
}

save_replay_state::substate_base to_base(save_replay_state::substate state) noexcept
{
	return static_cast<save_replay_state::substate_base>(static_cast<int>(state) & 0x1);
}

save_screen_flags to_flags(save_replay_state::substate state) noexcept
{
	return static_cast<save_screen_flags>(static_cast<int>(state) & static_cast<int>(save_screen_flags::MASK));
}

float save_replay_state::fade_overlay_opacity() const noexcept
{
	if (to_base(_substate) == substate_base::EXITING) {
		return _timer / 0.5_sf;
	}
	else {
		return 0;
	}
}

void save_replay_state::set_up_exit_animation() noexcept
{
	_ui.get("save_replay").pos.change(TOP_START_POS, 0.5_s);
	_ui.get("name").pos.change({600, 200}, 0.5_s);
	_ui.get("name_input").pos.change({600, 235}, 0.5_s);
	_ui.get("description").pos.change({400, 440}, 0.5_s);
	_ui.get("description_input").pos.change({400, 475}, 0.5_s);
	_ui.get("save").pos.change(BOTTOM_START_POS, 0.5_s);
	_ui.get("discard").pos.change(BOTTOM_START_POS, 0.5_s);
	_ui.hide_all(0.5_s);
}