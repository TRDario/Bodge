#include "../../include/game/player.hpp"
#include "../../include/audio.hpp"
#include "../../include/fonts.hpp"
#include "../../include/game/ball.hpp"
#include "../../include/graphics.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Damaged player color.
inline constexpr tr::rgb8 PLAYER_HIT_COLOR{255, 0, 0};
// Position of the timer text.
inline constexpr glm::vec2 TIMER_TEXT_POS{500, 50};
// The amount of time it takes for a full hover fade to occur.
inline constexpr ticks HOVER_TIME{0.25_s};
// The size of a small life in the UI.
inline constexpr float SMALL_LIFE_SIZE{10};
// The size of a large life in the UI.
inline constexpr float LARGE_LIFE_SIZE{20};
// The maximum allowed number of large lives in the UI.
inline constexpr int MAX_LARGE_LIVES{5};
// The number of (small) lives per line in the UI.
inline constexpr int LIVES_PER_LINE{10};
// Screen shake duration.
inline constexpr ticks SCREEN_SHAKE_TIME{2_s / 3};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

tr::gfx::dyn_atlas<char> create_timer_atlas()
{
	tr::gfx::dyn_atlas<char> atlas;
	for (char chr : std::string_view{"0123456789:"}) {
		atlas.add(chr, engine::render_gradient_glyph(chr, font::DEFAULT, tr::system::ttf_style::NORMAL, 64, 5));
	}
	if (tr::gfx::debug()) {
		atlas.set_label("(Bodge) Timer Atlas");
	}
	return atlas;
}

glm::vec2 player::timer_text_size(const std::string& text, float scale) const
{
	glm::vec2 size{};
	for (char chr : text) {
		const glm::vec2 char_size{glm::vec2{m_atlas.unnormalized(chr).size} / engine::render_scale() * scale};
		size = {size.x + char_size.x - 5, std::max<float>(size.y, char_size.y)};
	}
	return size;
}

void player::set_up_death_fragments()
{
	for (std::size_t i = 0; i < m_fragments.size(); ++i) {
		const tr::angle th{60_deg * i + 30_deg};
		const glm::vec2 vel{tr::magth(engine::rng.generate(200.0f, 400.0f), engine::rng.generate(th - 30_deg, th + 30_deg))};
		const tr::angle ang_vel{engine::rng.generate(360_deg, 720_deg) * (engine::rng.generate_bool() ? 1 : -1)};
		m_fragments[i] = {m_hitbox.c + tr::magth(m_hitbox.r, th), vel, th + 90_deg, ang_vel};
	}
}

void player::death_fragment::update()
{
	pos += vel / 1_sf;
	rot += rotvel / 1_sf;
}

void player::add_fill_to_renderer(std::uint8_t opacity, tr::angle rotation, float size) const
{
	const tr::gfx::simple_color_mesh_ref fill{tr::gfx::renderer_2d::new_color_fan(layer::PLAYER, 6)};
	tr::fill_poly_vtx(fill.positions, 6, {m_hitbox.c, size}, rotation);
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, opacity});
}

void player::add_outline_to_renderer(tr::rgb8 tint, std::uint8_t opacity, tr::angle rotation, float size) const
{
	const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::PLAYER, 6)};
	tr::fill_poly_outline_vtx(outline.positions, 6, {m_hitbox.c, size}, rotation, 4.0f);
	std::ranges::fill(outline.colors | std::views::take(6), tr::rgba8{tint, opacity});
	std::ranges::fill(outline.colors | std::views::drop(6), tr::rgba8{0, 0, 0, opacity});
}

void player::add_trail_to_renderer(tr::rgb8 tint, std::uint8_t opacity, tr::angle rotation, float size) const
{
	constexpr std::size_t VERTICES{6 * (trail::SIZE + 1)};
	constexpr std::size_t INDICES{tr::poly_outline_idx(6) * trail::SIZE};

	tr::gfx::color_mesh_ref trail_mesh{tr::gfx::renderer_2d::new_color_mesh(layer::PLAYER_TRAIL, VERTICES, INDICES)};
	tr::fill_poly_vtx(trail_mesh.positions | std::views::take(6), 6, {m_hitbox.c, size}, rotation);
	std::ranges::fill(trail_mesh.colors, tr::rgba8{tint, opacity});

	std::vector<std::uint16_t>::iterator indices_it{trail_mesh.indices.begin()};
	for (std::size_t i = 0; i < m_trail.SIZE; ++i) {
		const float trail_fade{static_cast<float>(m_trail.SIZE - i) / m_trail.SIZE};
		const float trail_size{size * trail_fade};
		const std::uint8_t trail_opacity{static_cast<std::uint8_t>(opacity / 3.0f * trail_fade)};

		tr::fill_poly_vtx(trail_mesh.positions | std::views::drop(6 * (i + 1)), 6, {m_trail[i], trail_size}, rotation);
		for (int j = 0; j < 6; ++j) {
			trail_mesh.colors[6 * (i + 1) + j].a = trail_opacity;
			*indices_it++ = static_cast<std::uint16_t>(trail_mesh.base_index + 6 * (i + 1) + j);
			*indices_it++ = static_cast<std::uint16_t>(trail_mesh.base_index + 6 * (i + 1) + ((j + 1) % 6));
			*indices_it++ = static_cast<std::uint16_t>(trail_mesh.base_index + 6 * i + ((j + 1) % 6));
			*indices_it++ = static_cast<std::uint16_t>(trail_mesh.base_index + 6 * (i + 1) + j);
			*indices_it++ = static_cast<std::uint16_t>(trail_mesh.base_index + 6 * i + ((j + 1) % 6));
			*indices_it++ = static_cast<std::uint16_t>(trail_mesh.base_index + 6 * i + j);
		}
	}
}

void player::add_lives_to_renderer() const
{
	const float life_size{m_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 1, 1})};
	const std::uint8_t opacity{static_cast<std::uint8_t>(255 - 180 * std::min(m_lives_hover_time, HOVER_TIME) / HOVER_TIME)};
	const tr::angle rotation{tr::degs(120.0f * m_timer / SECOND_TICKS)};

	for (int i = 0; i < m_lives; ++i) {
		const glm::ivec2 grid_pos{i % LIVES_PER_LINE, i / LIVES_PER_LINE};
		const glm::vec2 pos{(static_cast<glm::vec2>(grid_pos) + 0.5f) * 2.5f * life_size + 8.0f};

		const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::GAME_OVERLAY, 6)};
		tr::fill_poly_outline_vtx(outline.positions, 6, {pos, life_size}, rotation, 4.0f);
		std::ranges::fill(outline.colors | std::views::take(6), tr::rgba8{color, opacity});
		std::ranges::fill(outline.colors | std::views::drop(6), tr::rgba8{0, 0, 0, opacity});
	}
}

void player::add_timer_to_renderer() const
{
	const std::string text{timer_text(m_timer)};
	tr::rgba8 tint;
	float scale;

	if (game_over()) {
		tint = (m_timer > m_pb) ? "00FF00"_rgba8 : "FF0000"_rgba8;
		scale = 1;
	}
	else {
		const float factor{std::min(m_timer % 1_s, 0.2_s) / static_cast<float>(0.2_s)};
		const std::uint8_t tint_factor{tr::norm_cast<std::uint8_t>(1 - 0.25f * factor)};
		const ticks clamped_hover_time{std::min(m_timer_hover_time, HOVER_TIME)};
		const std::uint8_t opacity{static_cast<std::uint8_t>((255 - clamped_hover_time * 180 / HOVER_TIME) * tint_factor / 255)};
		tint = {tint_factor, tint_factor, tint_factor, opacity};
		scale = (1.25f - 0.25f * factor);
	}

	glm::vec2 tl{TIMER_TEXT_POS - timer_text_size(text, scale) / 2.0f};
	for (char chr : text) {
		tr::gfx::simple_textured_mesh_ref character{tr::gfx::renderer_2d::new_textured_fan(layer::GAME_OVERLAY, 4)};
		tr::fill_rect_vtx(character.positions, {tl, glm::vec2{m_atlas.unnormalized(chr).size} / engine::render_scale() * scale});
		tr::fill_rect_vtx(character.uvs, m_atlas[chr]);
		std::ranges::fill(character.tints, tint);
		tl.x += m_atlas.unnormalized(chr).size.x / engine::render_scale() * scale - 5;
	}
}

void player::set_screen_shake() const
{
	const int screen_shake_left{static_cast<int>(m_iframes - PLAYER_INVULN_TIME + SCREEN_SHAKE_TIME)};
	if (screen_shake_left >= 0) {
		const glm::vec2 tl{tr::magth(40.0f * screen_shake_left / SCREEN_SHAKE_TIME, engine::rng.generate_angle())};
		const glm::mat4 mat{tr::ortho(tr::frect2{tl, glm::vec2{1000}})};
		tr::gfx::renderer_2d::set_default_transform(mat);
	}
}

void player::add_death_wave_to_renderer() const
{
	const float t{(m_game_over_timer + 1) / static_cast<float>(0.5_s)};
	const float scale{std::sqrt(t) * 200};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 1, 1})};
	const std::uint8_t opacity{tr::norm_cast<std::uint8_t>(0.5f * std::sqrt(1 - t))};

	const std::size_t indices{tr::smooth_poly_vtx(scale, engine::render_scale()) + 2};
	const tr::gfx::simple_color_mesh_ref fan{tr::gfx::renderer_2d::new_color_fan(layer::PLAYER_TRAIL, indices)};
	fan.positions[0] = m_hitbox.c;
	fan.colors[0] = {color, 0};
	tr::fill_poly_vtx(fan.positions | std::views::drop(1), fan.positions.size() - 2, {m_hitbox.c, scale});
	fan.positions.back() = fan.positions[1];
	std::ranges::fill(fan.colors | std::views::drop(1), tr::rgba8{color, opacity});
}

void player::add_death_fragments_to_renderer() const
{
	const float t{(m_game_over_timer + 1) / static_cast<float>(0.5_s)};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 1, 1})};
	const std::uint8_t opacity{tr::norm_cast<std::uint8_t>(std::sqrt(1 - t))};
	const float length{2 * m_hitbox.r * tr::degs(30.0f).tan()};

	for (const death_fragment& fragment : m_fragments) {
		const tr::gfx::simple_color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_fan(layer::PLAYER, 4)};
		tr::fill_rotated_rect_vtx(mesh.positions, fragment.pos, {length / 2, 2}, {length, 4}, fragment.rot);
		std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
	}
}

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

player::player(const player_settings& settings, ticks pb)
	: m_hitbox{{500, 500}, settings.hitbox_radius}
	, m_trail{m_hitbox.c}
	, m_lives{static_cast<int>(settings.starting_lives)}
	, m_inertia{settings.inertia_factor}
	, m_timer{0}
	, m_game_over_timer{0}
	, m_iframes{0}
	, m_lives_hover_time{0}
	, m_timer_hover_time{0}
	, m_atlas{create_timer_atlas()}
	, m_pb{pb}
{
	tr::gfx::renderer_2d::set_default_layer_texture(layer::GAME_OVERLAY, m_atlas);
}

///////////////////////////////////////////////////////////////// GETTERS /////////////////////////////////////////////////////////////////

bool player::game_over() const
{
	return m_lives < 0;
}

ticks player::time_since_game_over() const
{
	return m_game_over_timer;
}

bool colliding(const player& player, const tr::static_vector<ball, 255>& balls)
{
	return player.m_iframes == 0 &&
		   std::ranges::any_of(balls, [&](const ball& b) { return b.tangible() && tr::intersecting(b.hitbox(), player.m_hitbox); });
}

///////////////////////////////////////////////////////////////// SETTERS /////////////////////////////////////////////////////////////////

void player::hit()
{
	--m_lives;
	m_iframes = PLAYER_INVULN_TIME;

	if (game_over()) {
		set_up_death_fragments();
		engine::play_sound(sound::GAME_OVER, 1, 0);
	}
	else {
		engine::play_sound(sound::HIT, 1, 0);
	}
}

void player::update()
{
	if (m_iframes > 0) {
		--m_iframes;
		set_screen_shake();
	}

	if (!game_over()) {
		if (m_timer % 2_s == 0) {
			engine::play_sound(sound::TICK, 0.5f, 0.0f);
		}
		else if (m_timer % 2_s == 1_s) {
			engine::play_sound(sound::TICK, 0.5f, 0.0f, 0.75f);
		}

		++m_timer;
		const float life_size{m_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
		const int lives_in_line{std::min(m_lives, LIVES_PER_LINE)};
		const int lines{m_lives / LIVES_PER_LINE + 1};
		tr::frect2 lives_ui_bounds{{}, {2.5f * life_size * (lives_in_line + 0.5f) + 16, 2.5f * life_size * lines + 16}};
		if (lives_ui_bounds.contains(m_hitbox.c)) {
			m_lives_hover_time = std::min(m_lives_hover_time + 1, HOVER_TIME);
		}
		else if (m_lives_hover_time > 0) {
			--m_lives_hover_time;
		}

		const glm::vec2 text_size{timer_text_size(timer_text(m_timer), 1 / engine::render_scale())};
		const tr::frect2 timer_text_bounds{TIMER_TEXT_POS - text_size / 2.0f - 8.0f, text_size + 16.0f};
		if (timer_text_bounds.contains(m_hitbox.c)) {
			m_timer_hover_time = std::min(m_timer_hover_time + 1, HOVER_TIME);
		}
		else if (m_timer_hover_time > 0) {
			--m_timer_hover_time;
		}
	}
	else {
		++m_game_over_timer;
		for (death_fragment& frag : m_fragments) {
			frag.update();
		}
	}
}

void player::update(glm::vec2 target)
{
	if (m_iframes > 0) {
		--m_iframes;
		set_screen_shake();
	}

	if (!game_over()) {
		if (m_timer % 2_s == 0) {
			engine::play_sound(sound::TICK, 0.5f, 0.0f);
		}
		else if (m_timer % 2_s == 1_s) {
			engine::play_sound(sound::TICK, 0.5f, 0.0f, 0.75f);
		}

		++m_timer;
		target = glm::clamp(target, glm::vec2{FIELD_MIN + m_hitbox.r}, glm::vec2{FIELD_MAX - m_hitbox.r});

		m_trail.push(m_hitbox.c);
		if (m_inertia == 0) {
			m_hitbox.c = target;
		}
		else {
			m_hitbox.c = m_hitbox.c + (target - m_hitbox.c) / 1_sf / m_inertia;
		}

		const float life_size{m_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
		const int lives_in_line{std::min(m_lives, LIVES_PER_LINE)};
		const int lines{m_lives / LIVES_PER_LINE + 1};
		tr::frect2 lives_ui_bounds{{}, {2.5f * life_size * (lives_in_line + 0.5f) + 16, 2.5f * life_size * lines + 16}};
		if (lives_ui_bounds.contains(target)) {
			m_lives_hover_time = std::min(m_lives_hover_time + 1, HOVER_TIME);
		}
		else if (m_lives_hover_time > 0) {
			--m_lives_hover_time;
		}

		const glm::vec2 text_size{timer_text_size(timer_text(m_timer), 1 / engine::render_scale())};
		const tr::frect2 timer_text_bounds{TIMER_TEXT_POS - text_size / 2.0f - 8.0f, text_size + 16.0f};
		if (timer_text_bounds.contains(target)) {
			m_timer_hover_time = std::min(m_timer_hover_time + 1, HOVER_TIME);
		}
		else if (m_timer_hover_time > 0) {
			--m_timer_hover_time;
		}
	}
	else {
		++m_game_over_timer;
		for (death_fragment& frag : m_fragments) {
			frag.update();
		}
	}
}

void player::add_to_renderer() const
{
	constexpr float PI{std::numbers::pi_v<float>};

	const tr::rgb8 tint{m_iframes ? PLAYER_HIT_COLOR
								  : color_cast<tr::rgb8>(tr::hsv{static_cast<float>(engine::settings.primary_hue), 1, 1})};
	const std::uint8_t opacity{tr::norm_cast<std::uint8_t>(std::abs(std::cos(m_iframes * PI * 8 / PLAYER_INVULN_TIME)))};
	const tr::angle rotation{tr::degs(270.0f * m_timer / SECOND_TICKS)};
	const float size_offset{3.0f * std::sin(PI * m_timer / SECOND_TICKS)};
	const float size{m_hitbox.r + 6 + size_offset};

	if (game_over() && m_game_over_timer < 0.5_s) {
		add_death_wave_to_renderer();
		add_death_fragments_to_renderer();
	}
	else if (!game_over()) {
		if (opacity != 0) {
			add_fill_to_renderer(opacity, rotation, size);
			add_outline_to_renderer(tint, opacity, rotation, size);
			add_trail_to_renderer(tint, opacity, rotation, size);
		}
		if (m_lives > 0) {
			add_lives_to_renderer();
		}
	}
	add_timer_to_renderer();
}