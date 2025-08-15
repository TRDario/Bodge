#include "../../include/state/player_settings_editor_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/system.hpp"
#include "../../include/ui/widget.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"gamemode_designer"};
constexpr tag T_SUBTITLE{"player_settings"};
constexpr tag T_STARTING_LIVES{"starting_lives"};
constexpr tag T_STARTING_LIVES_D{"starting_lives_d"};
constexpr tag T_STARTING_LIVES_C{"starting_lives_c"};
constexpr tag T_STARTING_LIVES_I{"starting_lives_i"};
constexpr tag T_HITBOX_RADIUS{"hitbox_radius"};
constexpr tag T_HITBOX_RADIUS_D{"hitbox_radius_d"};
constexpr tag T_HITBOX_RADIUS_C{"hitbox_radius_c"};
constexpr tag T_HITBOX_RADIUS_I{"hitbox_radius_i"};
constexpr tag T_INERTIA_FACTOR{"inertia_factor"};
constexpr tag T_INERTIA_FACTOR_D{"inertia_factor_d"};
constexpr tag T_INERTIA_FACTOR_C{"inertia_factor_c"};
constexpr tag T_INERTIA_FACTOR_I{"inertia_factor_i"};
constexpr tag T_EXIT{"exit"};

// Left-aligned labels.
constexpr std::array<label, 3> LABELS{{
	{T_STARTING_LIVES, "starting_lives_tt"},
	{T_HITBOX_RADIUS, "hitbox_size_tt"},
	{T_INERTIA_FACTOR, "inertia_factor_tt"},
}};
// Right-aligned widgets.
constexpr std::array<tag, 9> RIGHT_WIDGETS{T_STARTING_LIVES_D, T_STARTING_LIVES_C, T_STARTING_LIVES_I, T_HITBOX_RADIUS_D, T_HITBOX_RADIUS_C,
										   T_HITBOX_RADIUS_I,  T_INERTIA_FACTOR_D, T_INERTIA_FACTOR_C, T_INERTIA_FACTOR_I};

constexpr selection_tree SELECTION_TREE{
	selection_tree_row{T_STARTING_LIVES_D, T_STARTING_LIVES_C, T_STARTING_LIVES_I},
	selection_tree_row{T_HITBOX_RADIUS_D, T_HITBOX_RADIUS_C, T_HITBOX_RADIUS_I},
	selection_tree_row{T_INERTIA_FACTOR_D, T_INERTIA_FACTOR_C, T_INERTIA_FACTOR_I},
	selection_tree_row{T_EXIT},
};

constexpr shortcut_table SHORTCUTS{
	{{tr::system::keycode::ESCAPE}, T_EXIT},
	{{tr::system::keycode::TOP_ROW_1}, T_EXIT},
};

// Starting position of the starting lives widgets.
constexpr glm::vec2 STARTING_LIVES_START_POS{1050, 450};
// Starting position of the hitbox radius widgets.
constexpr glm::vec2 HITBOX_RADIUS_START_POS{1050, 525};
// Starting position of the inertia factor widgets.
constexpr glm::vec2 INERTIA_FACTOR_START_POS{1050, 600};

constexpr interpolator<glm::vec2> TITLE_MOVE_IN{TITLE_POS};
constexpr interpolator<glm::vec2> SUBTITLE_MOVE_IN{interp::CUBIC, TOP_START_POS, {500, TITLE_POS.y + 64}, 0.5_s};
constexpr interpolator<glm::vec2> STARTING_LIVES_D_MOVE_IN{
	interp::CUBIC, STARTING_LIVES_START_POS, {790, STARTING_LIVES_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> STARTING_LIVES_C_MOVE_IN{
	interp::CUBIC, STARTING_LIVES_START_POS, {887.5f, STARTING_LIVES_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> STARTING_LIVES_I_MOVE_IN{
	interp::CUBIC, STARTING_LIVES_START_POS, {985, STARTING_LIVES_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> HITBOX_RADIUS_D_MOVE_IN{interp::CUBIC, HITBOX_RADIUS_START_POS, {790, HITBOX_RADIUS_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> HITBOX_RADIUS_C_MOVE_IN{
	interp::CUBIC, HITBOX_RADIUS_START_POS, {887.5f, HITBOX_RADIUS_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> HITBOX_RADIUS_I_MOVE_IN{interp::CUBIC, HITBOX_RADIUS_START_POS, {985, HITBOX_RADIUS_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INERTIA_FACTOR_D_MOVE_IN{
	interp::CUBIC, INERTIA_FACTOR_START_POS, {790, INERTIA_FACTOR_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INERTIA_FACTOR_C_MOVE_IN{
	interp::CUBIC, INERTIA_FACTOR_START_POS, {887.5f, INERTIA_FACTOR_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> INERTIA_FACTOR_I_MOVE_IN{
	interp::CUBIC, INERTIA_FACTOR_START_POS, {985, INERTIA_FACTOR_START_POS.y}, 0.5_s};
constexpr interpolator<glm::vec2> EXIT_MOVE_IN{interp::CUBIC, BOTTOM_START_POS, {500, 1000}, 0.5_s};

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

player_settings_editor_state::player_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode)
	: m_substate{substate::IN_EDITOR}, m_timer{0}, m_ui{SELECTION_TREE, SHORTCUTS}, m_background_game{std::move(game)}, m_pending{gamemode}
{
	// STATUS CALLBACKS

	const status_callback scb{[this] { return m_substate == substate::IN_EDITOR; }};
	const status_callback starting_lives_d_scb{[this] { return m_substate == substate::IN_EDITOR && m_pending.player.starting_lives > 0; }};
	const status_callback starting_lives_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.starting_lives < 255; }};
	const status_callback hitbox_radius_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.hitbox_radius > 0.0f; }};
	const status_callback hitbox_radius_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.hitbox_radius < 100.0f; }};
	const status_callback inertia_factor_d_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.inertia_factor > 0.0f; }};
	const status_callback inertia_factor_i_scb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.inertia_factor < 0.99f; }};

	// ACTION CALLBACKS

	const action_callback starting_lives_d_acb{[&starting_lives = m_pending.player.starting_lives] {
		starting_lives = static_cast<std::uint32_t>(std::max(static_cast<int>(starting_lives - engine::keymods_choose(1, 5, 10)), 0));
	}};
	const action_callback starting_lives_i_acb{[&starting_lives = m_pending.player.starting_lives] {
		starting_lives = std::min(starting_lives + engine::keymods_choose(1, 5, 10), std::uint32_t{255});
	}};
	const action_callback hitbox_radius_d_acb{[&hitbox_radius = m_pending.player.hitbox_radius] {
		hitbox_radius = std::max(hitbox_radius - engine::keymods_choose(1, 5, 10), 0.0f);
	}};
	const action_callback hitbox_radius_i_acb{[&hitbox_radius = m_pending.player.hitbox_radius] {
		hitbox_radius = std::min(hitbox_radius + engine::keymods_choose(1, 5, 10), 100.0f);
	}};
	const action_callback inertia_factor_d_acb{[&inertia_factor = m_pending.player.inertia_factor] {
		inertia_factor = std::max(inertia_factor - engine::keymods_choose(0.01f, 0.05f, 0.1f), 0.0f);
	}};
	const action_callback inertia_factor_i_acb{[&inertia_factor = m_pending.player.inertia_factor] {
		inertia_factor = std::min(inertia_factor + engine::keymods_choose(0.01f, 0.05f, 0.1f), 0.99f);
	}};
	const action_callback exit_acb{[this] {
		m_substate = substate::EXITING;
		m_timer = 0;
		set_up_exit_animation();
	}};

	// TEXT CALLBACKS

	const text_callback cur_starting_lives_tcb{[this] { return std::format("{}", m_pending.player.starting_lives); }};
	const text_callback cur_hitbox_radius_tcb{[this] { return std::format("{:.0f}", m_pending.player.hitbox_radius); }};
	const text_callback cur_inertia_factor_tcb{[this] { return std::format("{:.2f}", m_pending.player.inertia_factor); }};

	//

	m_ui.emplace<label_widget>(T_TITLE, TITLE_MOVE_IN, tr::align::TOP_CENTER, 0, NO_TOOLTIP, loc_text_callback{T_TITLE},
							   tr::system::ttf_style::NORMAL, 64);
	m_ui.emplace<label_widget>(T_SUBTITLE, SUBTITLE_MOVE_IN, tr::align::TOP_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_SUBTITLE},
							   tr::system::ttf_style::NORMAL, 32);
	m_ui.emplace<arrow_widget>(T_STARTING_LIVES_D, STARTING_LIVES_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, starting_lives_d_scb,
							   starting_lives_d_acb);
	m_ui.emplace<text_widget>(T_STARTING_LIVES_C, STARTING_LIVES_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE,
							  tr::system::ttf_style::NORMAL, 48, cur_starting_lives_tcb);
	m_ui.emplace<arrow_widget>(T_STARTING_LIVES_I, STARTING_LIVES_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, starting_lives_i_scb,
							   starting_lives_i_acb);
	m_ui.emplace<text_widget>(T_HITBOX_RADIUS_C, HITBOX_RADIUS_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE,
							  tr::system::ttf_style::NORMAL, 48, cur_hitbox_radius_tcb);
	m_ui.emplace<arrow_widget>(T_HITBOX_RADIUS_D, HITBOX_RADIUS_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, hitbox_radius_d_scb,
							   hitbox_radius_d_acb);
	m_ui.emplace<arrow_widget>(T_HITBOX_RADIUS_I, HITBOX_RADIUS_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, hitbox_radius_i_scb,
							   hitbox_radius_i_acb);
	m_ui.emplace<arrow_widget>(T_INERTIA_FACTOR_D, INERTIA_FACTOR_D_MOVE_IN, tr::align::CENTER_LEFT, 0.5_s, false, inertia_factor_d_scb,
							   inertia_factor_d_acb);
	m_ui.emplace<text_widget>(T_INERTIA_FACTOR_C, INERTIA_FACTOR_C_MOVE_IN, tr::align::CENTER, 0.5_s, font::LANGUAGE,
							  tr::system::ttf_style::NORMAL, 48, cur_inertia_factor_tcb);
	m_ui.emplace<arrow_widget>(T_INERTIA_FACTOR_I, INERTIA_FACTOR_I_MOVE_IN, tr::align::CENTER_RIGHT, 0.5_s, true, inertia_factor_i_scb,
							   inertia_factor_i_acb);
	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const label& label{LABELS[i]};
		const interpolator<glm::vec2> move_in{interp::CUBIC, {-50, 450 + i * 75}, {15, 450 + i * 75}, 0.5_s};
		m_ui.emplace<label_widget>(label.tag, move_in, tr::align::CENTER_LEFT, 0.5_s, tooltip_loc_text_callback{LABELS[i].tooltip},
								   loc_text_callback{label.tag}, tr::system::ttf_style::NORMAL, 48);
	}
	m_ui.emplace<text_button_widget>(T_EXIT, EXIT_MOVE_IN, tr::align::BOTTOM_CENTER, 0.5_s, NO_TOOLTIP, loc_text_callback{T_EXIT},
									 font::LANGUAGE, 48, scb, exit_acb, sound::CANCEL);
}

///////////////////////////////////////////////////////////// VIRTUAL METHODS /////////////////////////////////////////////////////////////

std::unique_ptr<tr::state> player_settings_editor_state::handle_event(const tr::system::event& event)
{
	m_ui.handle_event(event);
	return nullptr;
}

std::unique_ptr<tr::state> player_settings_editor_state::update(tr::duration)
{
	++m_timer;
	m_background_game->update({});
	m_ui.update();

	switch (m_substate) {
	case substate::IN_EDITOR:
		return nullptr;
	case substate::EXITING:
		return m_timer >= 0.5_s ? std::make_unique<gamemode_designer_state>(std::move(m_background_game), m_pending, true) : nullptr;
	}
}

void player_settings_editor_state::draw()
{
	m_background_game->add_to_renderer();
	engine::add_menu_game_overlay_to_renderer();
	m_ui.add_to_renderer();
	tr::gfx::renderer_2d::draw(engine::screen());
}

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void player_settings_editor_state::set_up_exit_animation()
{
	widget& subtitle{m_ui[T_SUBTITLE]};
	widget& exit{m_ui[T_EXIT]};
	subtitle.pos.change(interp::CUBIC, TOP_START_POS, 0.5_s);
	subtitle.hide(0.5_s);
	for (tag tag : tr::project(LABELS, &label::tag)) {
		widget& widget{m_ui[tag]};
		widget.pos.change(interp::CUBIC, {-50, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		widget& widget{m_ui[tag]};
		widget.pos.change(interp::CUBIC, {1050, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	exit.pos.change(interp::CUBIC, BOTTOM_START_POS, 0.5_s);
	exit.hide(0.5_s);
}