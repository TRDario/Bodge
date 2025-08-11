#include "../../include/state/name_entry_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"enter_your_name"};
constexpr tag T_INPUT{"input"};
constexpr tag T_CONFIRM{"confirm"};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::ENTER}, T_CONFIRM},
};

constexpr interpolator<glm::vec2> TITLE_MOVE_IN{interp_mode::CUBE, TOP_START_POS, TITLE_POS, 1.0_s};
constexpr interpolator<glm::vec2> CONFIRM_MOVE_IN{interp_mode::CUBE, BOTTOM_START_POS, {500, 1000}, 1.0_s};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

name_entry_state::name_entry_state()
	: m_substate{substate::ENTERING_GAME}
	, m_timer{0}
	, m_ui{SHORTCUTS}
	, m_background_game{std::make_unique<game>(pick_menu_gamemode(), engine::rng.generate<std::uint64_t>())}
{
	engine::play_song("menu", 1.0s);

	const status_callback input_status_cb{[this] { return m_substate != substate::ENTERING_TITLE; }};
	const status_callback confirm_status_cb{
		[this] { return m_substate != substate::ENTERING_TITLE && !m_ui.as<line_input_widget<20>>(T_INPUT).buffer.empty(); }};

	const action_callback action_cb{[this] {
		line_input_widget<20>& input{m_ui.as<line_input_widget<20>>(T_INPUT)};
		if (!input.buffer.empty()) {
			m_timer = 0;
			m_substate = substate::ENTERING_TITLE;
			m_ui[T_TITLE].pos.change(interp_mode::CUBE, TOP_START_POS, 1.0_s);
			m_ui[T_CONFIRM].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 1.0_s);
			m_ui.hide_all(1.0_s);
			engine::play_sound(sound::CONFIRM, 0.5f, 0.0f);
			engine::scorefile.name = input.buffer;
		}
	}};

	m_ui.emplace<text_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 1.0_s, font::LANGUAGE, tr::system::ttf_style::NORMAL, 64,
							  loc_text_callback{T_TITLE});
	m_ui.emplace<line_input_widget<20>>(T_INPUT, glm::vec2{500, 500}, tr::align::CENTER, 1.0_s, tr::system::ttf_style::NORMAL, 64,
										input_status_cb, action_cb, std::string_view{});
	m_ui.emplace<clickable_text_widget>(T_CONFIRM, CONFIRM_MOVE_IN, tr::align::BOTTOM_CENTER, 1.0_s, font::LANGUAGE, 48,
										loc_text_callback{T_CONFIRM}, confirm_status_cb, action_cb);
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