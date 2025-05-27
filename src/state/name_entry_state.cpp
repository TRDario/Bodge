#include "../../include/engine.hpp"
#include "../../include/state/name_entry_state.hpp"
#include "../../include/state/title_state.hpp"

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

name_entry_state::name_entry_state()
	: _substate{substate::ENTERING_GAME}, _timer{0}, _game{MENU_GAMEMODES[rand(rng, MENU_GAMEMODES.size())], rand<std::uint64_t>(rng)}
{
	basic_text_widget& title{_ui.emplace<basic_text_widget>("enter_your_name", vec2{500, -50}, TOP_CENTER, font::LANGUAGE, 64)};
	title.pos.change({500, 0}, 1.0_s);
	title.unhide(1.0_s);

	const status_callback input_status_cb{[this] { return _substate != substate::ENTERING_TITLE; }};
	const action_callback action_cb{[this] {
		text_line_input_widget& input{_ui.get<text_line_input_widget>("input")};
		if (!input.buffer.empty()) {
			_timer = 0;
			_substate = substate::ENTERING_TITLE;
			_ui.get("enter_your_name").pos.change({500, -50}, 1.0_s);
			_ui.get("confirm").pos.change({500, 1050}, 1.0_s);
			_ui.hide_all(1.0_s);
			scorefile.name = input.buffer;
		}
	}};
	text_line_input_widget& input{_ui.emplace<text_line_input_widget>("input", vec2{500, 500}, CENTER, 64, input_status_cb, action_cb, 20)};
	input.unhide(1.0_s);

	const status_callback confirm_status_cb{
		[this] { return _substate != substate::ENTERING_TITLE && !_ui.get<text_line_input_widget>("input").buffer.empty(); }};
	clickable_text_widget& confirm{
		_ui.emplace<clickable_text_widget>("confirm", vec2{500, 1050}, BOTTOM_CENTER, 48, confirm_status_cb, action_cb)};
	confirm.pos.change({500, 1000}, 1.0_s);
	confirm.unhide(1.0_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

u32 name_entry_state::type() const noexcept
{
	return ID;
}

unique_ptr<state> name_entry_state::handle_event(const tr::event& event)
{
	_ui.handle_event(event);
	return nullptr;
}

unique_ptr<state> name_entry_state::update(tr::duration)
{
	++_timer;
	_game.update({});
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
	_game.add_to_renderer();
	engine::layered_renderer().add_color_quad(layer::GAME_OVERLAY, MENU_GAME_OVERLAY_QUAD);
	_ui.add_to_renderer();
	add_fade_overlay_to_renderer(_substate == substate::ENTERING_GAME ? 1 - _timer / 1.0_sf : 0);

	engine::layered_renderer().draw_up_to_layer(layer::UI, engine::screen());
	engine::batched_renderer().draw(engine::screen());
	engine::layered_renderer().draw(engine::screen());
}