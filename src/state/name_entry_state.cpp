#include "../../include/state/name_entry_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Shortcuts of the confirm button.
constexpr std::initializer_list<tr::system::key_chord> CONFIRM_SHORTCUTS{{tr::system::keycode::ENTER}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

name_entry_state::name_entry_state()
	: m_substate{substate::ENTERING_GAME}
	, m_timer{0}
	, m_background_game{std::make_unique<game>(pick_menu_gamemode(), engine::rng.generate<std::uint64_t>())}
{
	const status_callback input_status_cb{[this] { return m_substate != substate::ENTERING_TITLE; }};
	const status_callback confirm_status_cb{
		[this] { return m_substate != substate::ENTERING_TITLE && !m_ui.get<line_input_widget<20>>("input").buffer.empty(); }};

	const action_callback action_cb{[this] {
		line_input_widget<20>& input{m_ui.get<line_input_widget<20>>("input")};
		if (!input.buffer.empty()) {
			m_timer = 0;
			m_substate = substate::ENTERING_TITLE;
			m_ui.get("enter_your_name").pos.change(TOP_START_POS, 1.0_s);
			m_ui.get("confirm").pos.change(BOTTOM_START_POS, 1.0_s);
			m_ui.hide_all(1.0_s);
			engine::play_sound(sound::CONFIRM, 0.5f, 0.0f);
			engine::scorefile.name = input.buffer;
		}
	}};

	widget& title{m_ui.emplace<text_widget>("enter_your_name", TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE,
											tr::system::ttf_style::NORMAL, 64)};
	title.pos.change({500, 0}, 1.0_s);
	title.unhide(1.0_s);

	widget& input{m_ui.emplace<line_input_widget<20>>("input", glm::vec2{500, 500}, tr::align::CENTER, tr::system::ttf_style::NORMAL, 64,
													  input_status_cb, action_cb)};
	input.unhide(1.0_s);

	widget& confirm{m_ui.emplace<clickable_text_widget>("confirm", BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
														DEFAULT_TEXT_CALLBACK, confirm_status_cb, action_cb, NO_TOOLTIP,
														CONFIRM_SHORTCUTS)};
	confirm.pos.change({500, 1000}, 1.0_s);
	confirm.unhide(1.0_s);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> name_entry_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> name_entry_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

	switch (m_substate) {
	case substate::ENTERING_GAME:
		if (m_timer >= 1.0_s) {
			m_timer = 0;
			m_substate = substate::IN_NAME_ENTRY;
		}
		return nullptr;
	case substate::IN_NAME_ENTRY:
		return nullptr;
	case substate::ENTERING_TITLE:
		return m_timer >= 1.0_s ? std::make_unique<title_state>(std::move(m_background_game)) : nullptr;
	}
}

void name_entry_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	engine::add_fade_overlay_to_renderer(m_substate == substate::ENTERING_GAME ? 1 - m_timer / 1.0_sf : 0);
	tr::gfx::renderer_2d::draw(engine::screen());
}