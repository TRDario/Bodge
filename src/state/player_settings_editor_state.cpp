#include "../../include/state/player_settings_editor_state.hpp"
#include "../../include/state/gamemode_designer_state.hpp"
#include "../../include/system.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

constexpr tag T_TITLE{"gamemode_designer"};
constexpr tag T_SUBTITLE{"player_settings"};
constexpr tag T_STARTING_LIVES{"starting_lives"};
constexpr tag T_STARTING_LIVES_DEC{"starting_lives_dec"};
constexpr tag T_CUR_STARTING_LIVES{"cur_starting_lives"};
constexpr tag T_STARTING_LIVES_INC{"starting_lives_inc"};
constexpr tag T_HITBOX_RADIUS{"hitbox_radius"};
constexpr tag T_HITBOX_RADIUS_DEC{"hitbox_radius_dec"};
constexpr tag T_CUR_HITBOX_RADIUS{"cur_hitbox_radius"};
constexpr tag T_HITBOX_RADIUS_INC{"hitbox_radius_inc"};
constexpr tag T_INERTIA_FACTOR{"inertia_factor"};
constexpr tag T_INERTIA_FACTOR_DEC{"inertia_factor_dec"};
constexpr tag T_CUR_INERTIA_FACTOR{"cur_inertia_factor"};
constexpr tag T_INERTIA_FACTOR_INC{"inertia_factor_inc"};
constexpr tag T_EXIT{"exit"};

// Left-aligned labels.
constexpr std::array<label, 3> LABELS{{
	{T_STARTING_LIVES, "starting_lives_tt"},
	{T_HITBOX_RADIUS, "hitbox_size_tt"},
	{T_INERTIA_FACTOR, "inertia_factor_tt"},
}};
// Right-aligned widgets.
constexpr std::array<tag, 9> RIGHT_WIDGETS{T_STARTING_LIVES_DEC, T_CUR_STARTING_LIVES, T_STARTING_LIVES_INC,
										   T_HITBOX_RADIUS_DEC,  T_CUR_HITBOX_RADIUS,  T_HITBOX_RADIUS_INC,
										   T_INERTIA_FACTOR_DEC, T_CUR_INERTIA_FACTOR, T_INERTIA_FACTOR_INC};

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

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

player_settings_editor_state::player_settings_editor_state(std::unique_ptr<game>&& game, const gamemode& gamemode)
	: m_substate{substate::IN_EDITOR}, m_timer{0}, m_ui{SHORTCUTS}, m_background_game{std::move(game)}, m_pending{gamemode}
{
	// STATUS CALLBACKS

	const status_callback status_cb{[this] { return m_substate == substate::IN_EDITOR; }};
	const status_callback starting_lives_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.starting_lives > 0; }};
	const status_callback starting_lives_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.starting_lives < 255; }};
	const status_callback hitbox_radius_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.hitbox_radius > 0.0f; }};
	const status_callback hitbox_radius_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.hitbox_radius < 100.0f; }};
	const status_callback inertia_factor_dec_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.inertia_factor > 0.0f; }};
	const status_callback inertia_factor_inc_status_cb{
		[this] { return m_substate == substate::IN_EDITOR && m_pending.player.inertia_factor < 0.99f; }};

	// ACTION CALLBACKS

	const action_callback starting_lives_dec_action_cb{[&starting_lives = m_pending.player.starting_lives] {
		starting_lives = static_cast<std::uint32_t>(std::max(static_cast<int>(starting_lives - engine::keymods_choose(1, 5, 10)), 0));
	}};
	const action_callback starting_lives_inc_action_cb{[&starting_lives = m_pending.player.starting_lives] {
		starting_lives = std::min(starting_lives + engine::keymods_choose(1, 5, 10), std::uint32_t{255});
	}};
	const action_callback hitbox_radius_dec_action_cb{[&hitbox_radius = m_pending.player.hitbox_radius] {
		hitbox_radius = std::max(hitbox_radius - engine::keymods_choose(1, 5, 10), 0.0f);
	}};
	const action_callback hitbox_radius_inc_action_cb{[&hitbox_radius = m_pending.player.hitbox_radius] {
		hitbox_radius = std::min(hitbox_radius + engine::keymods_choose(1, 5, 10), 100.0f);
	}};
	const action_callback inertia_factor_dec_action_cb{[&inertia_factor = m_pending.player.inertia_factor] {
		inertia_factor = std::max(inertia_factor - engine::keymods_choose(0.01f, 0.05f, 0.1f), 0.0f);
	}};
	const action_callback inertia_factor_inc_action_cb{[&inertia_factor = m_pending.player.inertia_factor] {
		inertia_factor = std::min(inertia_factor + engine::keymods_choose(0.01f, 0.05f, 0.1f), 0.99f);
	}};
	const action_callback exit_action_cb{[this] {
		m_substate = substate::EXITING;
		m_timer = 0;
		set_up_exit_animation();
	}};

	// TEXT CALLBACKS

	const text_callback cur_starting_lives_text_cb{[this] { return std::format("{}", m_pending.player.starting_lives); }};
	const text_callback cur_hitbox_radius_text_cb{[this] { return std::format("{:.0f}", m_pending.player.hitbox_radius); }};
	const text_callback cur_inertia_factor_text_cb{[this] { return std::format("{:.2f}", m_pending.player.inertia_factor); }};

	//

	widget& title{m_ui.emplace<text_widget>(T_TITLE, TITLE_POS, tr::align::TOP_CENTER, font::LANGUAGE, tr::system::ttf_style::NORMAL, 64,
											loc_text_callback{T_TITLE})};
	title.unhide();

	widget& subtitle{m_ui.emplace<text_widget>(T_SUBTITLE, TOP_START_POS, tr::align::TOP_CENTER, font::LANGUAGE,
											   tr::system::ttf_style::NORMAL, 32, loc_text_callback{T_SUBTITLE})};
	subtitle.pos.change(interp_mode::CUBE, {500, TITLE_POS.y + 64}, 0.5_s);
	subtitle.unhide(0.5_s);

	widget& starting_lives_dec{m_ui.emplace<arrow_widget>(T_STARTING_LIVES_DEC, STARTING_LIVES_START_POS, tr::align::CENTER_LEFT, false,
														  starting_lives_dec_status_cb, starting_lives_dec_action_cb)};
	widget& starting_lives_inc{m_ui.emplace<arrow_widget>(T_STARTING_LIVES_INC, STARTING_LIVES_START_POS, tr::align::CENTER_RIGHT, true,
														  starting_lives_inc_status_cb, starting_lives_inc_action_cb)};
	widget& cur_starting_lives{m_ui.emplace<text_widget>(T_CUR_STARTING_LIVES, STARTING_LIVES_START_POS, tr::align::CENTER, font::LANGUAGE,
														 tr::system::ttf_style::NORMAL, 48, cur_starting_lives_text_cb)};
	starting_lives_dec.pos.change(interp_mode::CUBE, {790, STARTING_LIVES_START_POS.y}, 0.5_s);
	starting_lives_inc.pos.change(interp_mode::CUBE, {985, STARTING_LIVES_START_POS.y}, 0.5_s);
	cur_starting_lives.pos.change(interp_mode::CUBE, {887.5, STARTING_LIVES_START_POS.y}, 0.5_s);
	starting_lives_dec.unhide(0.5_s);
	starting_lives_inc.unhide(0.5_s);
	cur_starting_lives.unhide(0.5_s);

	widget& hitbox_radius_dec{m_ui.emplace<arrow_widget>(T_HITBOX_RADIUS_DEC, HITBOX_RADIUS_START_POS, tr::align::CENTER_LEFT, false,
														 hitbox_radius_dec_status_cb, hitbox_radius_dec_action_cb)};
	widget& hitbox_radius_inc{m_ui.emplace<arrow_widget>(T_HITBOX_RADIUS_INC, HITBOX_RADIUS_START_POS, tr::align::CENTER_RIGHT, true,
														 hitbox_radius_inc_status_cb, hitbox_radius_inc_action_cb)};
	widget& cur_hitbox_radius{m_ui.emplace<text_widget>(T_CUR_HITBOX_RADIUS, HITBOX_RADIUS_START_POS, tr::align::CENTER, font::LANGUAGE,
														tr::system::ttf_style::NORMAL, 48, cur_hitbox_radius_text_cb)};
	hitbox_radius_dec.pos.change(interp_mode::CUBE, {790, HITBOX_RADIUS_START_POS.y}, 0.5_s);
	hitbox_radius_inc.pos.change(interp_mode::CUBE, {985, HITBOX_RADIUS_START_POS.y}, 0.5_s);
	cur_hitbox_radius.pos.change(interp_mode::CUBE, {887.5, HITBOX_RADIUS_START_POS.y}, 0.5_s);
	hitbox_radius_dec.unhide(0.5_s);
	hitbox_radius_inc.unhide(0.5_s);
	cur_hitbox_radius.unhide(0.5_s);

	widget& inertia_factor_dec{m_ui.emplace<arrow_widget>(T_INERTIA_FACTOR_DEC, INERTIA_FACTOR_START_POS, tr::align::CENTER_LEFT, false,
														  inertia_factor_dec_status_cb, inertia_factor_dec_action_cb)};
	widget& inertia_factor_inc{m_ui.emplace<arrow_widget>(T_INERTIA_FACTOR_INC, INERTIA_FACTOR_START_POS, tr::align::CENTER_RIGHT, true,
														  inertia_factor_inc_status_cb, inertia_factor_inc_action_cb)};
	widget& cur_inertia_factor{m_ui.emplace<text_widget>(T_CUR_INERTIA_FACTOR, INERTIA_FACTOR_START_POS, tr::align::CENTER, font::LANGUAGE,
														 tr::system::ttf_style::NORMAL, 48, cur_inertia_factor_text_cb)};
	inertia_factor_dec.pos.change(interp_mode::CUBE, {790, INERTIA_FACTOR_START_POS.y}, 0.5_s);
	inertia_factor_inc.pos.change(interp_mode::CUBE, {985, INERTIA_FACTOR_START_POS.y}, 0.5_s);
	cur_inertia_factor.pos.change(interp_mode::CUBE, {887.5, INERTIA_FACTOR_START_POS.y}, 0.5_s);
	inertia_factor_dec.unhide(0.5_s);
	inertia_factor_inc.unhide(0.5_s);
	cur_inertia_factor.unhide(0.5_s);

	for (std::size_t i = 0; i < LABELS.size(); ++i) {
		const label& label{LABELS[i]};
		const glm::vec2 pos{-50, 450 + i * 75};
		widget& widget{m_ui.emplace<text_widget>(label.tag, pos, tr::align::CENTER_LEFT, LABELS[i].tooltip, font::LANGUAGE,
												 tr::system::ttf_style::NORMAL, 48, loc_text_callback{label.tag})};
		widget.pos.change(interp_mode::CUBE, {15, 450 + i * 75}, 0.5_s);
		widget.unhide(0.5_s);
	}

	widget& exit{m_ui.emplace<clickable_text_widget>(T_EXIT, BOTTOM_START_POS, tr::align::BOTTOM_CENTER, font::LANGUAGE, 48,
													 loc_text_callback{T_EXIT}, status_cb, exit_action_cb, NO_TOOLTIP, sound::CANCEL)};
	exit.pos.change(interp_mode::CUBE, {500, 1000}, 0.5_s);
	exit.unhide(0.5_s);
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
	subtitle.pos.change(interp_mode::CUBE, TOP_START_POS, 0.5_s);
	subtitle.hide(0.5_s);
	for (tag tag : tr::project(LABELS, &label::tag)) {
		widget& widget{m_ui[tag]};
		widget.pos.change(interp_mode::CUBE, {-50, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	for (tag tag : RIGHT_WIDGETS) {
		widget& widget{m_ui[tag]};
		widget.pos.change(interp_mode::CUBE, {1050, glm::vec2{widget.pos}.y}, 0.5_s);
		widget.hide(0.5_s);
	}
	exit.pos.change(interp_mode::CUBE, BOTTOM_START_POS, 0.5_s);
	exit.hide(0.5_s);
}