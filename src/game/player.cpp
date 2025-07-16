#include "../../include/game/player.hpp"
#include "../../include/audio.hpp"
#include "../../include/engine.hpp"
#include "../../include/font_manager.hpp"
#include "../../include/game/ball.hpp"

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

tr::static_atlas create_timer_atlas()
{
	tr::string_hash_map<tr::bitmap> renders;
	for (char chr : std::string_view{"0123456789:"}) {
		renders.emplace(std::string{&chr, 1}, font_manager.render_gradient_text({&chr, 1}, font::DEFAULT, tr::ttf_style::NORMAL, 64, 5));
	}
	tr::static_atlas atlas{renders};
	if (tr::gfx_context::debug()) {
		atlas.set_label("(Bodge) Timer Atlas");
	}
	return atlas;
}

glm::vec2 player::timer_text_size(const std::string& text, float scale) const noexcept
{
	glm::vec2 size{};
	for (char chr : text) {
		const glm::vec2 char_size{_atlas[{&chr, 1}].size * glm::vec2{_atlas.size()} * scale};
		size = {size.x + char_size.x - 5, std::max<float>(size.y, char_size.y)};
	}
	return size;
}

void player::set_up_death_fragments() noexcept
{
	for (std::size_t i = 0; i < _fragments.size(); ++i) {
		const tr::fangle th{60_degf * i + 30_degf};
		const glm::vec2 vel{tr::magth(tr::rand(rng, 200.0f, 400.0f), tr::rand(rng, th - 30_degf, th + 30_degf))};
		const tr::fangle ang_vel{tr::rand(rng, 360_degf, 720_degf) * (tr::rand<bool>(rng) ? 1 : -1)};
		_fragments[i] = {_hitbox.c + tr::magth(_hitbox.r, th), vel, th + 90_degf, ang_vel};
	}
}

void player::death_fragment::update() noexcept
{
	pos += vel / 1_sf;
	rot += rotvel / 1_sf;
}

void player::add_fill_to_renderer(std::uint8_t opacity, tr::fangle rotation, float size) const
{
	const tr::simple_color_mesh_ref fill{tr::renderer_2d::new_color_fan(layer::PLAYER, 6)};
	tr::fill_poly_vtx(fill.positions, 6, {_hitbox.c, size}, rotation);
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, opacity});
}

void player::add_outline_to_renderer(tr::rgb8 tint, std::uint8_t opacity, tr::fangle rotation, float size) const
{
	const tr::simple_color_mesh_ref outline{tr::renderer_2d::new_color_outline(layer::PLAYER, 6)};
	tr::fill_poly_outline_vtx(outline.positions, 6, {_hitbox.c, size}, rotation, 4.0f);
	std::ranges::fill(outline.colors | std::views::take(6), tr::rgba8{tint, opacity});
	std::ranges::fill(outline.colors | std::views::drop(6), tr::rgba8{0, 0, 0, opacity});
}

void player::add_trail_to_renderer(tr::rgb8 tint, std::uint8_t opacity, tr::fangle rotation, float size) const
{
	constexpr std::size_t VERTICES{6 * (trail::SIZE + 1)};
	constexpr std::size_t INDICES{tr::poly_outline_idx(6) * trail::SIZE};

	tr::color_mesh_ref trail{tr::renderer_2d::new_color_mesh(layer::PLAYER_TRAIL, VERTICES, INDICES)};
	tr::fill_poly_vtx(trail.positions | std::views::take(6), 6, {_hitbox.c, size}, rotation);
	std::ranges::fill(trail.colors, tr::rgba8{tint, opacity});

	std::vector<std::uint16_t>::iterator indices_it{trail.indices.begin()};
	for (std::size_t i = 0; i < _trail.SIZE; ++i) {
		const float trail_fade{static_cast<float>(_trail.SIZE - i) / _trail.SIZE};
		const float trail_size{size * trail_fade};
		const std::uint8_t trail_opacity{static_cast<std::uint8_t>(opacity / 3.0f * trail_fade)};

		tr::fill_poly_vtx(trail.positions | std::views::drop(6 * (i + 1)), 6, {_trail[i], trail_size}, rotation);
		for (int j = 0; j < 6; ++j) {
			trail.colors[6 * (i + 1) + j].a = trail_opacity;
			*indices_it++ = static_cast<std::uint16_t>(trail.base_index + 6 * (i + 1) + j);
			*indices_it++ = static_cast<std::uint16_t>(trail.base_index + 6 * (i + 1) + ((j + 1) % 6));
			*indices_it++ = static_cast<std::uint16_t>(trail.base_index + 6 * i + ((j + 1) % 6));
			*indices_it++ = static_cast<std::uint16_t>(trail.base_index + 6 * (i + 1) + j);
			*indices_it++ = static_cast<std::uint16_t>(trail.base_index + 6 * i + ((j + 1) % 6));
			*indices_it++ = static_cast<std::uint16_t>(trail.base_index + 6 * i + j);
		}
	}
}

void player::add_lives_to_renderer() const
{
	const float life_size{_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};
	const std::uint8_t opacity{static_cast<std::uint8_t>(255 - 180 * std::min(_lives_hover_time, HOVER_TIME) / HOVER_TIME)};
	const tr::fangle rotation{tr::degs(120.0f * _timer / SECOND_TICKS)};

	for (int i = 0; i < _lives; ++i) {
		const glm::ivec2 grid_pos{i % LIVES_PER_LINE, i / LIVES_PER_LINE};
		const glm::vec2 pos{(static_cast<glm::vec2>(grid_pos) + 0.5f) * 2.5f * life_size + 8.0f};

		const tr::simple_color_mesh_ref outline{tr::renderer_2d::new_color_outline(layer::GAME_OVERLAY, 6)};
		tr::fill_poly_outline_vtx(outline.positions, 6, {pos, life_size}, rotation, 4.0f);
		std::ranges::fill(outline.colors | std::views::take(6), tr::rgba8{color, opacity});
		std::ranges::fill(outline.colors | std::views::drop(6), tr::rgba8{0, 0, 0, opacity});
	}
}

void player::add_timer_to_renderer() const
{
	const std::string text{timer_text(_timer)};
	tr::rgba8 tint;
	float scale;

	if (game_over()) {
		tint = (_timer > _pb) ? "00FF00"_rgba8 : "FF0000"_rgba8;
		scale = 1;
	}
	else {
		const float factor{std::min(_timer % 1_s, 0.2_s) / static_cast<float>(0.2_s)};
		const std::uint8_t tint_factor{tr::norm_cast<std::uint8_t>(1 - 0.25f * factor)};
		const ticks clamped_hover_time{std::min(_timer_hover_time, HOVER_TIME)};
		const std::uint8_t opacity{static_cast<std::uint8_t>((255 - clamped_hover_time * 180 / HOVER_TIME) * tint_factor / 255)};
		tint = {tint_factor, tint_factor, tint_factor, opacity};
		scale = (1.25f - 0.25f * factor) / engine::render_scale();
	}

	glm::vec2 tl{TIMER_TEXT_POS - timer_text_size(text, scale) / 2.0f};
	for (char chr : text) {
		tr::simple_textured_mesh_ref character{tr::renderer_2d::new_textured_fan(layer::GAME_OVERLAY, 4)};
		tr::fill_rect_vtx(character.positions, {tl, _atlas[{&chr, 1}].size * glm::vec2{_atlas.size()} * scale});
		tr::fill_rect_vtx(character.uvs, _atlas[{&chr, 1}]);
		std::ranges::fill(character.tints, tint);
		tl.x += _atlas[{&chr, 1}].size.x * _atlas.size().x * scale - 5;
	}
}

void player::set_screen_shake() const
{
	const int screen_shake_left{static_cast<int>(_iframes - PLAYER_INVULN_TIME + SCREEN_SHAKE_TIME)};
	if (screen_shake_left >= 0) {
		const glm::vec2 tl{tr::magth(40.0f * screen_shake_left / SCREEN_SHAKE_TIME, tr::rand<tr::fangle>(rng))};
		const glm::mat4 mat{tr::ortho(tr::frect2{tl, glm::vec2{1000}})};
		tr::renderer_2d::set_default_transform(mat);
	}
}

void player::add_death_wave_to_renderer() const
{
	const float t{(_game_over_timer + 1) / static_cast<float>(0.5_s)};
	const float scale{std::sqrt(t) * 200};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};
	const std::uint8_t opacity{tr::norm_cast<std::uint8_t>(0.5f * std::sqrt(1 - t))};

	const std::size_t indices{tr::smooth_poly_vtx(scale, engine::render_scale()) + 2};
	const tr::simple_color_mesh_ref fan{tr::renderer_2d::new_color_fan(layer::PLAYER_TRAIL, indices)};
	fan.positions[0] = _hitbox.c;
	fan.colors[0] = {color, 0};
	tr::fill_poly_vtx(fan.positions | std::views::drop(1), fan.positions.size() - 2, {_hitbox.c, scale});
	fan.positions.back() = fan.positions[1];
	std::ranges::fill(fan.colors | std::views::drop(1), tr::rgba8{color, opacity});
}

void player::add_death_fragments_to_renderer() const
{
	const float t{(_game_over_timer + 1) / static_cast<float>(0.5_s)};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};
	const std::uint8_t opacity{tr::norm_cast<std::uint8_t>(std::sqrt(1 - t))};
	const float length{2 * _hitbox.r * tr::degs(30.0f).tan()};

	for (const death_fragment& fragment : _fragments) {
		const tr::simple_color_mesh_ref mesh{tr::renderer_2d::new_color_fan(layer::PLAYER, 4)};
		tr::fill_rotated_rect_vtx(mesh.positions, fragment.pos, {length / 2, 2}, {length, 4}, fragment.rot);
		std::ranges::fill(mesh.colors, tr::rgba8{color, opacity});
	}
}

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

player::player(const player_settings& settings, ticks pb) noexcept
	: _hitbox{{500, 500}, settings.hitbox_radius}
	, _trail{_hitbox.c}
	, _lives{static_cast<int>(settings.starting_lives)}
	, _inertia{settings.inertia_factor}
	, _timer{0}
	, _game_over_timer{0}
	, _iframes{0}
	, _lives_hover_time{0}
	, _timer_hover_time{0}
	, _atlas{create_timer_atlas()}
	, _pb{pb}
{
	tr::renderer_2d::set_default_layer_texture(layer::GAME_OVERLAY, _atlas);
}

///////////////////////////////////////////////////////////////// GETTERS /////////////////////////////////////////////////////////////////

bool player::game_over() const noexcept
{
	return _lives < 0;
}

ticks player::time_since_game_over() const noexcept
{
	return _game_over_timer;
}

bool colliding(const player& player, const tr::static_vector<ball, 255>& balls) noexcept
{
	return player._iframes == 0 &&
		   std::ranges::any_of(balls, [&](const ball& b) { return b.tangible() && tr::intersecting(b.hitbox(), player._hitbox); });
}

///////////////////////////////////////////////////////////////// SETTERS /////////////////////////////////////////////////////////////////

void player::hit() noexcept
{
	--_lives;
	_iframes = PLAYER_INVULN_TIME;

	if (game_over()) {
		set_up_death_fragments();
		// audio::play(sfx::EXPLOSION, 1, 0);
	}
	else {
		// audio::play(sfx::HIT, 1, 0);
	}
}

void player::update() noexcept
{
	if (_iframes > 0) {
		--_iframes;
		set_screen_shake();
	}

	if (!game_over()) {
		if (_timer % 2_s == 0) {
			audio::play(sfx::TICK, 0.5f, 0.0f);
		}
		else if (_timer % 2_s == 1_s) {
			audio::play(sfx::TICK, 0.5f, 0.0f, 0.75f);
		}

		++_timer;
		const float life_size{_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
		const int lives_in_line{std::min(_lives, LIVES_PER_LINE)};
		const int lines{_lives / LIVES_PER_LINE + 1};
		tr::frect2 lives_ui_bounds{{}, {2.5f * life_size * (lives_in_line + 0.5f) + 16, 2.5f * life_size * lines + 16}};
		if (lives_ui_bounds.contains(_hitbox.c)) {
			_lives_hover_time = std::min(_lives_hover_time + 1, HOVER_TIME);
		}
		else if (_lives_hover_time > 0) {
			--_lives_hover_time;
		}

		const glm::vec2 text_size{timer_text_size(timer_text(_timer), 1 / engine::render_scale())};
		const tr::frect2 timer_text_bounds{TIMER_TEXT_POS - text_size / 2.0f - 8.0f, text_size + 16.0f};
		if (timer_text_bounds.contains(_hitbox.c)) {
			_timer_hover_time = std::min(_timer_hover_time + 1, HOVER_TIME);
		}
		else if (_timer_hover_time > 0) {
			--_timer_hover_time;
		}
	}
	else {
		++_game_over_timer;
		for (death_fragment& frag : _fragments) {
			frag.update();
		}
	}
}

void player::update(glm::vec2 target) noexcept
{
	if (_iframes > 0) {
		--_iframes;
		set_screen_shake();
	}

	if (!game_over()) {
		if (_timer % 2_s == 0) {
			audio::play(sfx::TICK, 0.5f, 0.0f);
		}
		else if (_timer % 2_s == 1_s) {
			audio::play(sfx::TICK, 0.5f, 0.0f, 0.75f);
		}

		++_timer;
		target = glm::clamp(target, glm::vec2{FIELD_MIN + _hitbox.r}, glm::vec2{FIELD_MAX - _hitbox.r});

		_trail.push(_hitbox.c);
		if (_inertia == 0) {
			_hitbox.c = target;
		}
		else {
			_hitbox.c = _hitbox.c + (target - _hitbox.c) / 1_sf / _inertia;
		}

		const float life_size{_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
		const int lives_in_line{std::min(_lives, LIVES_PER_LINE)};
		const int lines{_lives / LIVES_PER_LINE + 1};
		tr::frect2 lives_ui_bounds{{}, {2.5f * life_size * (lives_in_line + 0.5f) + 16, 2.5f * life_size * lines + 16}};
		if (lives_ui_bounds.contains(target)) {
			_lives_hover_time = std::min(_lives_hover_time + 1, HOVER_TIME);
		}
		else if (_lives_hover_time > 0) {
			--_lives_hover_time;
		}

		const glm::vec2 text_size{timer_text_size(timer_text(_timer), 1 / engine::render_scale())};
		const tr::frect2 timer_text_bounds{TIMER_TEXT_POS - text_size / 2.0f - 8.0f, text_size + 16.0f};
		if (timer_text_bounds.contains(target)) {
			_timer_hover_time = std::min(_timer_hover_time + 1, HOVER_TIME);
		}
		else if (_timer_hover_time > 0) {
			--_timer_hover_time;
		}
	}
	else {
		++_game_over_timer;
		for (death_fragment& frag : _fragments) {
			frag.update();
		}
	}
}

void player::add_to_renderer() const
{
	constexpr float PI{std::numbers::pi_v<float>};

	const tr::rgb8 tint{_iframes ? PLAYER_HIT_COLOR : color_cast<tr::rgb8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};
	const std::uint8_t opacity{tr::norm_cast<std::uint8_t>(std::abs(std::cos(_iframes * PI * 8 / PLAYER_INVULN_TIME)))};
	const tr::fangle rotation{tr::degs(270.0f * _timer / SECOND_TICKS)};
	const float size_offset{3.0f * std::sin(PI * _timer / SECOND_TICKS)};
	const float size{_hitbox.r + 6 + size_offset};

	if (game_over() && _game_over_timer < 0.5_s) {
		add_death_wave_to_renderer();
		add_death_fragments_to_renderer();
	}
	else if (!game_over()) {
		if (opacity != 0) {
			add_fill_to_renderer(opacity, rotation, size);
			add_outline_to_renderer(tint, opacity, rotation, size);
			add_trail_to_renderer(tint, opacity, rotation, size);
		}
		if (_lives > 0) {
			add_lives_to_renderer();
		}
	}
	add_timer_to_renderer();
}