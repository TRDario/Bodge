#include "../../include/state/name_entry_state.hpp"
#include "../../include/engine.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Shortcuts of the confirm button.
constexpr std::initializer_list<tr::key_chord> CONFIRM_SHORTCUTS{{tr::keycode::ENTER}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

name_entry_state::name_entry_state()
	: _substate{substate::ENTERING_GAME}, _timer{0}, _game{std::make_unique<game>(pick_menu_gamemode(), tr::rand<std::uint64_t>(rng))}
{
	const status_callback input_status_cb{[this] { return _substate != substate::ENTERING_TITLE; }};
	const status_callback confirm_status_cb{
		[this] { return _substate != substate::ENTERING_TITLE && !_ui.get<line_input_widget<20>>("input").buffer.empty(); }};

	const action_callback action_cb{[this] {
		line_input_widget<20>& input{_ui.get<line_input_widget<20>>("input")};
		if (!input.buffer.empty()) {
			_timer = 0;
			_substate = substate::ENTERING_TITLE;
			_ui.get("enter_your_name").pos.change(TOP_START_POS, 1.0_s);
			_ui.get("confirm").pos.change(BOTTOM_START_POS, 1.0_s);
			_ui.hide_all(1.0_s);
			audio::play_sound(sound::CONFIRM, 0.5f, 0.0f);
			scorefile.name = input.buffer;
		}
	}};

	widget& title{
		_ui.emplace<text_widget>("enter_your_name", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 1.0_s);
	title.unhide(1.0_s);

	widget& input{_ui.emplace<line_input_widget<20>>("input", glm::vec2{500, 500}, tr::align::CENTER, tr::ttf_style::NORMAL, 64,
													 input_status_cb, action_cb)};
	input.unhide(1.0_s);

	widget& confirm{_ui.emplace<clickable_text_widget>("confirm", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													   DEFAULT_TEXT_CALLBACK, confirm_status_cb, action_cb, NO_TOOLTIP, CONFIRM_SHORTCUTS)};
	confirm.pos.change({500, 1000}, 1.0_s);
	confirm.unhide(1.0_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> name_entry_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> name_entry_state::update(tr::duration)
{
	++_timer;
	_game->update({});
	_ui.update();

	switch (_substate) {
	case substate::ENTERING_GAME:
		if (_timer >= 1.0_s) {
			_timer = 0;
			_substate = substate::IN_NAME_ENTRY;
		}
		return nullptr;
	case substate::IN_NAME_ENTRY:
		return nullptr;
	case substate::ENTERING_TITLE:
		return _timer >= 1.0_s ? std::make_unique<title_state>(std::move(_game)) : nullptr;
	}
}

void name_entry_state::draw()
{
	_game->add_to_renderer();
	add_menu_game_overlay_to_renderer();
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(_substate == substate::ENTERING_GAME ? 1 - _timer / 1.0_sf : 0);
	tr::renderer_2d::draw(engine::screen());
}