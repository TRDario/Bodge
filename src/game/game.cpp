#include "../../include/game/game.hpp"
#include "../../include/audio.hpp"
#include "../../include/fonts.hpp"
#include "../../include/graphics.hpp"
#include "../../include/score.hpp"
#include "../../include/system.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////
// clang-format off

// The pitch of the collection sound for each life fragment.
constexpr std::array<float, 9> COLLECT_PITCHES{
	12 / 12.0f,
	14 / 12.0f,
	16 / 12.0f,
	17 / 12.0f,
	19 / 12.0f,
	21 / 12.0f,
	23 / 12.0f,
	24 / 12.0f,
	24 / 12.0f,
};

// Size of the central scoring region.
inline constexpr float CENTER_SIZE{1000 / 3.0f};
// Size of the edge scoring region.
inline constexpr ticks EDGE_SIZE{100};
// The amount of accumulated time needed for a score region to activate.
inline constexpr ticks MIN_SCORE_REGION_TIME{2_s};

// The size of a life in the UI if there are more than MAX_LARGE_LIVES remaining.
inline constexpr float SMALL_LIFE_SIZE{10};
// The size of a life in the UI if there are less than or equal to MAX_LARGE_LIVES remaining.
inline constexpr float LARGE_LIFE_SIZE{20};
// Maximum number of lives before the UI switches to a smaller life icon.
inline constexpr int MAX_LARGE_LIVES{5};
// Maximum number of small lives per line.
inline constexpr int LIVES_PER_LINE{10};

inline constexpr glm::vec2 TIMER_TEXT_POS{500, 50};
inline constexpr glm::vec2 SCORE_TEXT_POS{990, -4};

// clang-format on
///////////////////////////////////////////////////////////// PLAYERLESS GAME /////////////////////////////////////////////////////////////

playerless_game::playerless_game(const ::gamemode& gamemode, u64 rng_seed)
	: m_gamemode{gamemode}
	, m_rng{rng_seed}
	, m_start_timer{0}
	, m_last_ball_timer{0}
	, m_next_ball_size{gamemode.ball.initial_size}
	, m_next_ball_velocity{gamemode.ball.initial_velocity}
{
	for (int i = 0; i < gamemode.ball.starting_count; ++i) {
		add_new_ball();
	}
}

const gamemode& playerless_game::gamemode() const
{
	return m_gamemode;
}

void playerless_game::update()
{
	++m_start_timer;

	++m_last_ball_timer;
	if (m_last_ball_timer >= m_gamemode.ball.spawn_interval && m_balls.size() < m_gamemode.ball.max_count) {
		add_new_ball();
		engine::play_sound(sound::BALL_SPAWN, 0.25f, (m_balls.back().hitbox().c.x - 500) / 500);
	}

	for (u8 i = 0; i < m_balls.size(); ++i) {
		m_balls[i].update();
		if (m_balls[i].tangible()) {
			for (u8 j = i + 1; j < m_balls.size(); ++j) {
				if (m_balls[j].tangible() && colliding(m_balls[i], m_balls[j])) {
					handle_collision(m_balls[i], m_balls[j]);
				}
			}
		}
	}
}

void playerless_game::add_to_renderer() const
{
	std::ranges::for_each(m_balls, &ball::add_to_renderer);
	add_overlay_to_renderer();
	add_border_to_renderer();
}

void playerless_game::add_new_ball()
{
	m_last_ball_timer = 0;
	m_balls.emplace_back(m_rng, m_next_ball_size, m_next_ball_velocity);
	m_next_ball_size = std::min(m_next_ball_size + m_gamemode.ball.size_step, 100.0f);
	m_next_ball_velocity = std::min(m_next_ball_velocity + m_gamemode.ball.velocity_step, 5000.0f);
}

void playerless_game::add_overlay_to_renderer() const
{
	const tr::gfx::simple_color_mesh_ref overlay{
		tr::gfx::renderer_2d::new_color_fan(layer::BALL_TRAILS, 4, TRANSFORM, tr::gfx::REVERSE_ALPHA_BLENDING)};
	std::ranges::copy(OVERLAY_POSITIONS, overlay.positions.begin());
	std::ranges::fill(overlay.colors, "00000000"_rgba8);
}

void playerless_game::add_border_to_renderer() const
{
	const tr::gfx::simple_color_mesh_ref border{tr::gfx::renderer_2d::new_color_outline(layer::BORDER, 4)};
	tr::fill_rect_outline_vtx(border.positions, {{2, 2}, {996, 996}}, 4);
	std::ranges::fill(border.colors, color_cast<tr::rgba8>(tr::hsv{float(engine::settings.secondary_hue), 1, 1}));
}

////////////////////////////////////////////////////////////////// GAME ///////////////////////////////////////////////////////////////////

tr::gfx::dyn_atlas<char> create_number_atlas()
{
	tr::gfx::dyn_atlas<char> atlas;
	atlas.set_filtering(tr::gfx::min_filter::LINEAR, tr::gfx::mag_filter::LINEAR);
	for (char chr : std::string_view{"0123456789:-"}) {
		atlas.add(chr, engine::render_gradient_glyph(chr, font::DEFAULT, tr::system::ttf_style::NORMAL, 64, 5));
	}
	if (tr::gfx::debug()) {
		atlas.set_label("(Bodge) Timer Atlas");
	}
	return atlas;
}

game::game(const ::gamemode& gamemode, u64 rng_seed)
	: playerless_game{gamemode, rng_seed}
	, m_number_atlas{create_number_atlas()}
	, m_player{gamemode.player}
	, m_lives_left{int(gamemode.player.starting_lives)}
	, m_collected_fragments{0}
	, m_score{0}
	, m_tock{false}
{
	tr::gfx::renderer_2d::set_default_layer_texture(layer::GAME_OVERLAY, m_number_atlas);
}

//

bool game::game_over() const
{
	return m_game_over_timer.active();
}

i64 game::final_score() const
{
	return m_score;
}

ticks game::final_time() const
{
	if (m_game_over_timer.active()) {
		return m_start_timer - m_game_over_timer.elapsed();
	}
	else {
		return m_start_timer;
	}
}

//

void game::update(const glm::vec2& input)
{
	play_tick_sound_if_needed();
	playerless_game::update();
	update_timers();
	update_life_fragments();
	if (!game_over()) {
		m_player.update(input);
		check_if_player_is_hovering_over_timer();
		check_if_player_is_hovering_over_lives();
		check_if_player_is_hovering_over_score();
		check_if_player_was_hit();
		check_if_player_collected_life_fragments();
		check_for_score_ticks();
		check_for_style_points();
	}
	else {
		m_player.update_fragments();
	}
	set_screen_shake();
}

void game::play_tick_sound_if_needed()
{
	if (game_over()) {
		return;
	}

	if (!m_last_life_fragments_timer.active() || m_last_life_fragments_timer.elapsed() >= LIFE_FRAGMENT_DURATION ||
		m_collected_fragments == 9) {
		if (m_start_timer % 1_s == 0) {
			engine::play_sound(sound::TICK, 0.33f, 0.0f, m_tock ? 0.75f : 1.0f);
			m_tock = !m_tock;
		}
	}
	else {
		const ticks time{m_last_life_fragments_timer.elapsed()};
		if ((time >= LIFE_FRAGMENT_FAST_FLASH_START && time % 0.1_s == 0) ||
			(time < LIFE_FRAGMENT_FAST_FLASH_START && time >= LIFE_FRAGMENT_SLOW_FLASH_START && time % 0.25_s == 0) ||
			(time < LIFE_FRAGMENT_SLOW_FLASH_START && time % 0.5_s == 0)) {
			engine::play_sound(sound::TICK_ALT, 0.75f, 0.0f, 1.0f);
		}
	}
}

void game::update_timers()
{
	m_last_life_fragments_timer.update();
	m_1up_animation_timer.update();
	if (m_hit_animation_timer.active()) {
		m_hit_animation_timer.update();
		for (fragment& fragment : m_shattered_life_fragments) {
			fragment.update();
		}
	}
	m_screen_shake_timer.update();
	m_game_over_timer.update();
	m_score_animation_timer.update();
}

void game::update_life_fragments()
{
	if (!m_gamemode.player.spawn_life_fragments) {
		return;
	}

	if (m_last_life_fragments_timer.active() && m_last_life_fragments_timer.elapsed() == LIFE_FRAGMENT_DURATION) {
		m_life_fragments.clear();
	}

	if ((!m_last_life_fragments_timer.active() && m_start_timer == m_gamemode.player.life_fragment_spawn_interval) ||
		(m_last_life_fragments_timer.active() && m_last_life_fragments_timer.elapsed() == m_gamemode.player.life_fragment_spawn_interval)) {
		for (int i = 0; i < 9; ++i) {
			constexpr float THIRD{1000.0f / 3};
			m_life_fragments.emplace_back(m_rng, tr::frect2{{(i % 3) * THIRD, int(i / 3) * THIRD}, {THIRD, THIRD}});
		}
		m_last_life_fragments_timer.start();
		m_collected_fragments = 0;
		engine::play_sound(sound::FRAGMENT_SPAWN, 1, 0);
	}

	for (auto it = m_life_fragments.begin(); it != m_life_fragments.end();) {
		it->update(m_last_life_fragments_timer.elapsed());
		if (it->can_despawn()) {
			it = m_life_fragments.erase(it);
		}
		else {
			++it;
		}
	}
}

void game::check_if_player_is_hovering_over_timer()
{
	const glm::vec2 size{text_size(format_time(m_start_timer), 1 / engine::render_scale()) * 0.95f};
	const tr::frect2 timer_text_bounds{TIMER_TEXT_POS - size / 2.0f - 8.0f, size + 16.0f};
	if (timer_text_bounds.contains(m_player.hitbox().c)) {
		m_timer_hover_timer.increment();
	}
	else {
		m_timer_hover_timer.decrement();
	}
}

void game::check_if_player_is_hovering_over_lives()
{
	const float life_size{m_lives_left > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
	const int lives_in_line{std::min(m_lives_left, LIVES_PER_LINE)};
	const int lines{m_lives_left / LIVES_PER_LINE + 1};
	tr::frect2 lives_ui_bounds{{}, {2.5f * life_size * (lives_in_line + 0.5f) + 16, 2.5f * life_size * lines + 16}};
	if (lives_ui_bounds.contains(m_player.hitbox().c)) {
		m_lives_hover_timer.increment();
	}
	else {
		m_lives_hover_timer.decrement();
	}
}

void game::check_if_player_is_hovering_over_score()
{
	const glm::vec2 size{text_size(std::to_string(m_score), 1 / engine::render_scale()) * 0.85f};
	const tr::frect2 timer_text_bounds{tl(SCORE_TEXT_POS, size, tr::align::TOP_RIGHT), size};
	if (timer_text_bounds.contains(m_player.hitbox().c)) {
		m_score_hover_timer.increment();
	}
	else {
		m_score_hover_timer.decrement();
	}
}

void game::check_if_player_was_hit()
{
	const auto hit_player{[&](const ball& b) { return b.tangible() && tr::intersecting(b.hitbox(), m_player.hitbox()); }};
	if (!m_player.invincible() && std::ranges::any_of(m_balls, hit_player)) {
		--m_lives_left;
		if (m_lives_left < 0) {
			m_game_over_timer.start();
			m_screen_shake_timer.start();
			m_player.kill();
			engine::play_sound(sound::GAME_OVER, 1, 0);
		}
		else {
			m_hit_animation_timer.start();
			m_screen_shake_timer.start();
			m_player.hit();
			set_up_shattered_life_fragments();
			engine::play_sound(sound::HIT, 1, 0);
		}
	}
}

void game::set_up_shattered_life_fragments()
{
	const float life_size{m_lives_left > (m_hit_animation_timer.active() ? MAX_LARGE_LIVES - 1 : MAX_LARGE_LIVES) ? SMALL_LIFE_SIZE
																												  : LARGE_LIFE_SIZE};
	const glm::ivec2 grid_pos{m_lives_left % LIVES_PER_LINE, m_lives_left / LIVES_PER_LINE};
	const glm::vec2 pos{(glm::vec2{grid_pos} + 0.5f) * 2.5f * life_size + 8.0f};
	for (usize i = 0; i < m_shattered_life_fragments.size(); ++i) {
		const tr::angle th{60_deg * i + 30_deg};
		const glm::vec2 vel{tr::magth(engine::rng.generate(200.0f, 400.0f), engine::rng.generate(th - 30_deg, th + 30_deg))};
		const tr::angle ang_vel{engine::rng.generate(420_deg, 780_deg) * (engine::rng.generate_bool() ? 1 : -1)};
		m_shattered_life_fragments[i] = {pos + tr::magth(life_size, th), vel, th + 90_deg, ang_vel};
	}
}

void game::check_if_player_collected_life_fragments()
{
	for (life_fragment& fragment : m_life_fragments) {
		if (!fragment.collected() && tr::intersecting(fragment.hitbox(), m_player.hitbox())) {
			fragment.set_collected();
			if (++m_collected_fragments < 9) {
				add_to_score(25);
			}
			else if (m_lives_left < 255) {
				++m_lives_left;
				m_1up_animation_timer.start();
				add_to_score(150);
				engine::play_sound(sound::ONE_UP, 1.25f, 0);
			}
			engine::play_sound(sound::COLLECT, 0.65f, 0, COLLECT_PITCHES[m_collected_fragments - 1]);
		}
	}
}

void game::check_for_score_ticks()
{
	if (m_player.hitbox().c.x >= FIELD_CENTER - CENTER_SIZE / 2 && m_player.hitbox().c.x <= FIELD_CENTER + CENTER_SIZE / 2 &&
		m_player.hitbox().c.y >= FIELD_CENTER - CENTER_SIZE / 2 && m_player.hitbox().c.y <= FIELD_CENTER + CENTER_SIZE / 2) {
		m_center_timer.increment();
	}
	else {
		m_center_timer.decrement();
	}

	if (m_player.hitbox().c.y < FIELD_MIN + EDGE_SIZE || m_player.hitbox().c.y > FIELD_MAX - EDGE_SIZE) {
		m_edge_timer.increment();
		if (m_player.hitbox().c.x < FIELD_MIN + EDGE_SIZE || m_player.hitbox().c.x > FIELD_MAX - EDGE_SIZE) {
			m_corner_timer.increment();
		}
		else {
			m_corner_timer.decrement();
		}
	}
	else if (m_player.hitbox().c.x < FIELD_MIN + EDGE_SIZE || m_player.hitbox().c.x > FIELD_MAX - EDGE_SIZE) {
		m_edge_timer.increment();
		if (m_player.hitbox().c.y < FIELD_MIN + EDGE_SIZE || m_player.hitbox().c.y > FIELD_MAX - EDGE_SIZE) {
			m_corner_timer.increment();
		}
		else {
			m_corner_timer.decrement();
		}
	}
	else {
		m_corner_timer.decrement();
		m_edge_timer.decrement();
	}

	if (m_start_timer % 1_s == 0) {
		if (m_center_timer.accumulated() >= MIN_SCORE_REGION_TIME) {
			add_to_score(25);
		}
		else if (m_corner_timer.accumulated() >= MIN_SCORE_REGION_TIME) {
			add_to_score(-10);
		}
		else if (m_edge_timer.accumulated() >= MIN_SCORE_REGION_TIME) {
			add_to_score(5);
		}
		else {
			add_to_score(10);
		}
	}
}

void game::check_for_style_points()
{
	m_style_cooldown_timer.update();
	if (!m_style_cooldown_timer.active()) {
		i64 max_points{0};
		for (const ball& ball : std::views::filter(m_balls, &ball::tangible)) {
			const float velocity{glm::length(ball.velocity())};
			const tr::angle rect_angle{tr::atan2(ball.velocity().y / velocity, ball.velocity().x / velocity)};
			const glm::vec2 rect_size{ball.hitbox().r + velocity / 3, ball.hitbox().r * 2 + 2 * m_player.hitbox().r};
			const glm::vec2 rect_center{ball.hitbox().c + ball.velocity() / 6.0f + tr::magth(ball.hitbox().r, rect_angle)};
			const tr::frect2 unrotated_rect{tr::frect2{rect_center - rect_size / 2.0f, rect_size}};
			const glm::mat4 inverse_rotation{tr::rotate_around(1.0f, rect_center, -rect_angle)};
			if (unrotated_rect.contains(inverse_rotation * m_player.hitbox().c)) {
				max_points =
					std::max({1_i64, tr::floor_cast<i64>(std::sqrt(ball.hitbox().r / 10) * std::pow(velocity / 250, 1.5f)), max_points});
			}
		}
		if (max_points != 0) {
			add_to_score(max_points);
			m_style_cooldown_timer.start();
			const float pan{(m_player.hitbox().c.x - 500) / 500};
			engine::play_sound(sound::STYLE, 0.25f, pan);
		}
	}
}

void game::set_screen_shake() const
{
	if (m_screen_shake_timer.active()) {
		const glm::vec2 tl{tr::magth(40 * (1 - m_screen_shake_timer.elapsed_ratio()), engine::rng.generate_angle())};
		const glm::mat4 mat{tr::ortho(tr::frect2{tl, glm::vec2{1000}})};
		tr::gfx::renderer_2d::set_default_transform(mat);
	}
}

//

void game::add_to_renderer() const
{
	playerless_game::add_to_renderer();
	for (const life_fragment& frag : m_life_fragments) {
		frag.add_to_renderer(m_last_life_fragments_timer.elapsed());
	}
	add_timer_to_renderer();
	if (game_over()) {
		m_player.add_to_renderer_dead(m_game_over_timer.elapsed());
	}
	else {
		m_player.add_to_renderer_alive(m_start_timer, m_style_cooldown_timer);
		add_lives_to_renderer();
	}
	add_score_to_renderer();
}

void game::add_timer_to_renderer() const
{
	ticks time;
	tr::rgba8 tint;
	float scale;
	if (game_over()) {
		time = final_time();
		tint = (time >= engine::scorefile.bests(gamemode()).time) ? "00FF00"_rgba8 : "FF0000"_rgba8;
		scale = 1;
	}
	else {
		time = m_start_timer;
		const float factor{std::min(m_start_timer % 1_s, 0.2_s) / 0.2_sf};
		const u8 tint_factor{tr::norm_cast<u8>(1 - 0.25f * factor)};
		const u8 opacity{u8((255 - m_timer_hover_timer.accumulated() * 180 / m_timer_hover_timer.max()) * tint_factor / 255)};
		tint = {tint_factor, tint_factor, tint_factor, opacity};
		scale = (1.25f - 0.25f * factor);
	}

	const std::string text{format_time(time)};
	glm::vec2 tl{TIMER_TEXT_POS - text_size(text, scale) / 2.0f};
	for (char chr : text) {
		tr::gfx::simple_textured_mesh_ref character{tr::gfx::renderer_2d::new_textured_fan(layer::GAME_OVERLAY, 4)};
		tr::fill_rect_vtx(character.positions, {tl, glm::vec2{m_number_atlas.unnormalized(chr).size} / engine::render_scale() * scale});
		tr::fill_rect_vtx(character.uvs, m_number_atlas[chr]);
		std::ranges::fill(character.tints, tint);
		tl.x += m_number_atlas.unnormalized(chr).size.x / engine::render_scale() * scale - 5;
	}
}

void game::add_lives_to_renderer() const
{
	const float life_size{m_lives_left > (m_hit_animation_timer.active() ? MAX_LARGE_LIVES - 1 : MAX_LARGE_LIVES) ? SMALL_LIFE_SIZE
																												  : LARGE_LIFE_SIZE};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{float(engine::settings.primary_hue), 1, 1})};
	const u8 opacity{u8(255 - 180 * m_lives_hover_timer.accumulated() / m_lives_hover_timer.max())};
	const tr::angle rotation{120_deg * m_start_timer / 1_s};

	int normal_lives{m_lives_left};
	if (m_1up_animation_timer.active() && !m_hit_animation_timer.active()) {
		--normal_lives;
	}
	for (int i = 0; i < normal_lives; ++i) {
		const glm::ivec2 grid_pos{i % LIVES_PER_LINE, i / LIVES_PER_LINE};
		const glm::vec2 pos{(glm::vec2{grid_pos} + 0.5f) * 2.5f * life_size + 8.0f};

		const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::GAME_OVERLAY, 6)};
		tr::fill_poly_outline_vtx(outline.positions, 6, {pos, life_size}, rotation, 2.0f);
		std::ranges::fill(outline.colors, tr::rgba8{color, opacity});
	}

	if (m_hit_animation_timer.active()) {
		add_shattering_life_to_renderer(color, opacity);
	}
	else if (m_1up_animation_timer.active()) {
		add_appearing_life_to_renderer(color, opacity);
	}
}

void game::add_appearing_life_to_renderer(tr::rgb8 color, u8 base_opacity) const
{
	const float raw_age_factor{m_1up_animation_timer.elapsed_ratio()};
	const float eased_age_factor{raw_age_factor == 1.0f ? raw_age_factor : 1.0f - std::pow(2.0f, -10.0f * raw_age_factor)};
	const float size_factor{(5 - 4 * eased_age_factor)};
	const float life_size{m_lives_left > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
	const glm::ivec2 grid_pos{(m_lives_left - 1) % LIVES_PER_LINE, (m_lives_left - 1) / LIVES_PER_LINE};
	const glm::vec2 pos{(glm::vec2{grid_pos} + 0.5f) * 2.5f * life_size + 8.0f};
	const tr::angle rotation{120_deg * m_start_timer / 1_s};
	const u8 opacity{u8(base_opacity * std::pow(raw_age_factor, 1 / 3.0f))};

	const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::GAME_OVERLAY, 6)};
	tr::fill_poly_outline_vtx(outline.positions, 6, {pos, life_size * size_factor}, rotation, 2.0f * size_factor);
	std::ranges::fill(outline.colors, tr::rgba8{color, opacity});
}

void game::add_shattering_life_to_renderer(tr::rgb8 color, u8 base_opacity) const
{
	const float life_size{m_lives_left > MAX_LARGE_LIVES - 1 ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
	const float length{2 * life_size * (30_deg).tan()};
	const u8 opacity{u8(base_opacity - base_opacity * m_hit_animation_timer.elapsed_ratio())};
	for (const fragment& fragment : m_shattered_life_fragments) {
		const tr::gfx::simple_color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_fan(layer::GAME_OVERLAY, 4)};
		tr::fill_rotated_rect_vtx(mesh.positions, fragment.pos, {length / 2, 1}, {length, 2}, fragment.rot);
		std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
	}
}

void game::add_score_to_renderer() const
{
	const std::string text{format_score(m_score)};
	tr::rgba8 tint;
	float scale;

	if (game_over()) {
		tint = (m_score >= engine::scorefile.bests(gamemode()).score) ? "00FF00"_rgba8 : "FF0000"_rgba8;
		scale = 0.85f;
	}
	else {
		const float factor{m_score_animation_timer.elapsed_ratio()};
		const u8 tint_factor{tr::norm_cast<u8>(1 - 0.1f * factor)};
		const u8 opacity{u8((255 - m_score_hover_timer.accumulated() * 180 / m_score_hover_timer.max()) * tint_factor / 255)};
		tint = {tint_factor, tint_factor, tint_factor, opacity};
		scale = (0.85f - 0.1f * factor);

		constexpr float MAX{decltype(m_center_timer)::max() - MIN_SCORE_REGION_TIME + 0.5_sf};
		if (m_center_timer.accumulated() >= MIN_SCORE_REGION_TIME - 0.5_s) {
			tint.b -= 0.75f * tint.b * (m_center_timer.accumulated() - MIN_SCORE_REGION_TIME + 0.5_s) / MAX;
		}
		else if (m_edge_timer.accumulated() >= MIN_SCORE_REGION_TIME - 0.5_s) {
			tint.g -= 0.5f * tint.g * (m_edge_timer.accumulated() - MIN_SCORE_REGION_TIME + 0.5_s) / MAX;
			tint.b -= 0.5f * tint.b * (m_edge_timer.accumulated() - MIN_SCORE_REGION_TIME + 0.5_s) / MAX;
		}
	}

	glm::vec2 tl{tr::tl(SCORE_TEXT_POS, text_size(text, scale), tr::align::TOP_RIGHT)};
	for (char chr : text) {
		tr::gfx::simple_textured_mesh_ref character{tr::gfx::renderer_2d::new_textured_fan(layer::GAME_OVERLAY, 4)};
		tr::fill_rect_vtx(character.positions, {tl, glm::vec2{m_number_atlas.unnormalized(chr).size} / engine::render_scale() * scale});
		tr::fill_rect_vtx(character.uvs, m_number_atlas[chr]);
		std::ranges::fill(character.tints, tint);
		tl.x += m_number_atlas.unnormalized(chr).size.x / engine::render_scale() * scale - 5;
	}
}

//

void game::add_to_score(i64 change)
{
	m_score += change;
	m_score_animation_timer.start();
}

glm::vec2 game::text_size(const std::string& text, float scale) const
{
	glm::vec2 size{};
	for (char chr : text) {
		const glm::vec2 char_size{glm::vec2{m_number_atlas.unnormalized(chr).size} / engine::render_scale() * scale};
		size = {size.x + char_size.x - 5, std::max<float>(size.y, char_size.y)};
	}
	return size;
}

/////////////////////////////////////////////////////////////// ACTIVE GAME ///////////////////////////////////////////////////////////////

active_game::active_game(const ::gamemode& gamemode, u64 seed)
	: game{gamemode, seed}, replay{gamemode, seed}
{
}

//

void active_game::update()
{
	const bool is_game_over{game_over()};
	const glm::vec2 input{engine::mouse_pos()};
	game::update(input);
	if (!is_game_over) {
		replay.append(input);
	}
}

////////////////////////////////////////////////////////////// REPLAY GAME ////////////////////////////////////////////////////////////////

replay_game::replay_game(const ::gamemode& gamemode, replay&& replay)
	: game{gamemode, replay.header().seed}, m_replay{std::move(replay)}
{
}

replay_game::replay_game(const replay_game& r)
	: replay_game{r.gamemode(), replay{r.m_replay}}
{
}

//

bool replay_game::done() const
{
	return m_replay.done();
}

glm::vec2 replay_game::cursor_pos() const
{
	return m_replay.prev_input();
}

//

void replay_game::update()
{
	game::update(done() ? m_replay.prev_input() : m_replay.next_input());
}