#include "../../include/state/save_replay_state.hpp"
#include "../../include/state/game_state.hpp"
#include "../../include/state/title_state.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag TAG_TITLE{"save_replay"};
constexpr tag TAG_NAME{"name"};
constexpr tag TAG_NAME_INPUT{"name_input"};
constexpr tag TAG_DESCRIPTION{"description"};
constexpr tag TAG_DESCRIPTION_INPUT{"description_input"};
constexpr tag TAG_SAVE{"save"};
constexpr tag TAG_DISCARD{"discard"};

// Shortcuts of the save button.
constexpr std::initializer_list<tr::system::key_chord> SAVE_SHORTCUTS{
	{tr::system::keycode::ENTER}, {tr::system::keycode::S}, {tr::system::keycode::TOP_ROW_1}};
// Shortcuts of the don't save button.
constexpr std::initializer_list<tr::system::key_chord> DONT_SAVE_SHORTCUTS{
	{tr::system::keycode::ESCAPE}, {tr::system::keycode::C}, {tr::system::keycode::TOP_ROW_2}};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

save_replay_state::save_replay_state(std::unique_ptr<active_game>&& game, save_screen_flags flags)
	: m_substate{substate_base::SAVING_REPLAY | flags}, m_timer{0}, m_game{std::move(game)}, m_replay{m_game->replay().header()}
{
	const status_callback status_cb{[this] { return to_base(m_substate) == substate_base::SAVING_REPLAY; }};

	widget& title{m_ui.emplace<text_widget>(TAG_TITLE, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL,
											64, loc_text_callback{TAG_TITLE})};
	title.pos.change(interp_mode::CUBE, {500, 0}, 0.5_s);
	title.unhide(0.5_s);

	widget& name_label{m_ui.emplace<text_widget>(TAG_NAME, glm::vec2{400, 200}, tr::align::CENTER, font::LANGUAGE,
												 tr::system::ttf_style::NORMAL, 48, loc_text_callback{TAG_NAME})};
	name_label.pos.change(interp_mode::CUBE, {500, 200}, 0.5_s);
	name_label.unhide(0.5_s);

	const action_callback name_action_cb{[this] { m_ui.move_input_focus_forward(); }};
	widget& name{m_ui.emplace<line_input_widget<20>>(TAG_NAME_INPUT, glm::vec2{400, 235}, tr::align::TOP_CENTER,
													 tr::system::ttf_style::NORMAL, 64, status_cb, name_action_cb)};
	name.pos.change(interp_mode::CUBE, {500, 235}, 0.5_s);
	name.unhide(0.5_s);

	widget& description_label{m_ui.emplace<text_widget>(TAG_DESCRIPTION, glm::vec2{600, 440}, tr::align::CENTER, font::LANGUAGE,
														tr::system::ttf_style::NORMAL, 48, loc_text_callback{TAG_DESCRIPTION})};
	description_label.pos.change(interp_mode::CUBE, {500, 440}, 0.5_s);
	description_label.unhide(0.5_s);

	widget& description{m_ui.emplace<multiline_input_widget<255>>(TAG_DESCRIPTION_INPUT, glm::vec2{600, 475}, tr::align::TOP_CENTER, 800,
																  10, tr::halign::CENTER, 24, status_cb)};
	description.pos.change(interp_mode::CUBE, {500, 475}, 0.5_s);
	description.unhide(0.5_s);

	const status_callback save_status_cb{[this] {
		return to_base(m_substate) == substate_base::SAVING_REPLAY && !m_ui.as<line_input_widget<20>>(TAG_NAME_INPUT).buffer.empty();
	}};
	const action_callback save_action_cb{[this] {
		const score_flags flags{!m_game->game_over(), engine::cli_settings.game_speed != 1.0f};

		m_substate = substate_base::EXITING | to_flags(m_substate);
		m_timer = 0;
		set_up_exit_animation();
		m_game->replay().set_header(
			{m_ui.as<multiline_input_widget<255>>(TAG_DESCRIPTION_INPUT).buffer, unix_now(), m_game->result(), flags},
			m_ui.as<line_input_widget<20>>("name_input").buffer);
		m_game->replay().save_to_file();
	}};
	widget& save{m_ui.emplace<clickable_text_widget>(TAG_SAVE, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													 loc_text_callback{TAG_SAVE}, save_status_cb, save_action_cb, NO_TOOLTIP,
													 SAVE_SHORTCUTS)};
	save.pos.change(interp_mode::CUBE, {500, 950}, 0.5_s);
	save.unhide(0.5_s);

	const action_callback dont_save_action_cb{[this] {
		m_substate = substate_base::EXITING | to_flags(m_substate);
		m_timer = 0;
		set_up_exit_animation();
	}};
	widget& discard{m_ui.emplace<clickable_text_widget>(TAG_DISCARD, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
														loc_text_callback{TAG_DISCARD}, status_cb, dont_save_action_cb, NO_TOOLTIP,
														DONT_SAVE_SHORTCUTS)};
	discard.pos.change(interp_mode::CUBE, {500, 1000}, 0.5_s);
	discard.unhide(0.5_s);
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
	m_ui[TAG_TITLE].pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	m_ui[TAG_NAME].pos.change(interp_mode::CUBE, {600, 200}, 0.5_s);
	m_ui[TAG_NAME_INPUT].pos.change(interp_mode::CUBE, {600, 235}, 0.5_s);
	m_ui[TAG_DESCRIPTION].pos.change(interp_mode::CUBE, {400, 440}, 0.5_s);
	m_ui[TAG_DESCRIPTION_INPUT].pos.change(interp_mode::CUBE, {400, 475}, 0.5_s);
	m_ui[TAG_SAVE].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	m_ui[TAG_DISCARD].pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	m_ui.hide_all(0.5_s);
}