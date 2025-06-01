#include "../../include/audio.hpp"
#include "../../include/engine.hpp"
#include "../../include/font_manager.hpp"
#include "../../include/game/ball.hpp"
#include "../../include/game/player.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// Damaged player color.
inline constexpr tr::rgb8 PLAYER_HIT_COLOR{255, 0, 0};
// Position of the timer text.
inline constexpr vec2 TIMER_TEXT_POS{500, 50};
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
	tr::string_hash_map<bitmap> renders;
	for (char chr : string_view{"0123456789:"}) {
		renders.emplace(string{&chr, 1}, font_manager.render_gradient_text({&chr, 1}, font::DEFAULT, NORMAL, 64, 5));
	}
	tr::static_atlas atlas{renders};
	if (cli_settings.debug_mode) {
		atlas.set_label("(Bodge) Timer Atlas");
	}
	return atlas;
}

// Gets the formatted timer text string.
string timer_text(ticks time)
{
	return {time >= 60_s ? format("{}:{:02}:{:02}", time / 60_s, (time % 60_s) / 1_s, (time % 1_s) * 100 / 1_s)
						 : format("{:02}:{:02}", time / 1_s, (time % 1_s) * 100 / 1_s)};
}

vec2 player::timer_text_size(const string& text, float scale) const noexcept
{
	vec2 size{};
	for (char chr : text) {
		const vec2 char_size{_atlas[{&chr, 1}].size * vec2{_atlas.size()} * scale};
		size = {size.x + char_size.x - 5, max<float>(size.y, char_size.y)};
	}
	return size;
}

void player::set_up_death_fragments() noexcept
{
	for (size_t i = 0; i < _fragments.size(); ++i) {
		const fangle th{60_degf * i + 30_degf};
		_fragments[i] = {_hitbox.c + magth(_hitbox.r, th), magth(rand(rng, 200.0f, 400.0f), rand(rng, th - 30_degf, th + 30_degf)),
						 th + 90_degf, rand(rng, 360_degf, 720_degf) * (rand<bool>(rng) ? 1 : -1)};
	}
}

void player::death_fragment::update() noexcept
{
	pos += vel / 1_sf;
	rot += rotvel / 1_sf;
}

void player::add_fill_to_renderer(u8 opacity, fangle rotation, float size) const
{
	engine::vertex_buffer().resize(6);

	tr::fill_poly_vtx(positions(engine::vertex_buffer()), 6, {_hitbox.c, size}, rotation);
	rs::fill(colors(engine::vertex_buffer()), rgba8{0, 0, 0, opacity});
	engine::layered_renderer().add_color_fan(layer::PLAYER, engine::vertex_buffer());
}

void player::add_outline_to_renderer(tr::rgb8 tint, u8 opacity, fangle rotation, float size) const
{
	engine::vertex_buffer().resize(12);
	vector<u16> indices(poly_outline_idx(6));

	fill_poly_outline_idx(indices.begin(), 6, 0);
	tr::fill_poly_outline_vtx(positions(engine::vertex_buffer()), 6, {_hitbox.c, size}, rotation, 4.0f);
	rs::fill(colors(engine::vertex_buffer()) | vs::take(6), rgba8{tint, opacity});
	rs::fill(colors(engine::vertex_buffer()) | vs::drop(6), rgba8{0, 0, 0, opacity});
	engine::layered_renderer().add_color_mesh(layer::PLAYER, engine::vertex_buffer(), std::move(indices));
}

void player::add_trail_to_renderer(tr::rgb8 tint, u8 opacity, fangle rotation, float size) const
{
	engine::vertex_buffer().resize(6 * (trail::SIZE + 1));
	vector<u16> indices(poly_outline_idx(6) * _trail.SIZE);

	tr::fill_poly_vtx(positions(engine::vertex_buffer()), 6, {_hitbox.c, size}, rotation);
	rs::fill(colors(engine::vertex_buffer()), rgba8{tint, opacity});
	for (size_t i = 0; i < _trail.SIZE; ++i) {
		const float trail_fade{static_cast<float>(_trail.SIZE - i) / _trail.SIZE};
		const float trail_size{size * trail_fade};
		const u8 trail_opacity{static_cast<u8>(opacity / 3.0f * trail_fade)};

		tr::fill_poly_vtx(positions(engine::vertex_buffer()).begin() + 6 * (i + 1), 6, {_trail[i], trail_size}, rotation);
		for (int j = 0; j < 6; ++j) {
			engine::vertex_buffer()[6 * (i + 1) + j].color.a = trail_opacity;
			indices.push_back(6 * (i + 1) + j);
			indices.push_back(6 * (i + 1) + ((j + 1) % 6));
			indices.push_back(6 * i + ((j + 1) % 6));
			indices.push_back(6 * (i + 1) + j);
			indices.push_back(6 * i + ((j + 1) % 6));
			indices.push_back(6 * i + j);
		}
	}
	engine::layered_renderer().add_color_mesh(layer::PLAYER_TRAIL, engine::vertex_buffer(), std::move(indices));
}

void player::add_lives_to_renderer() const
{
	const float life_size{_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};
	const u8 opacity{static_cast<u8>(255 - 180 * min(_lives_hover_time, HOVER_TIME) / HOVER_TIME)};
	const fangle rotation{tr::degs(120.0f * _timer / SECOND_TICKS)};

	vector<u16> indices(_lives * poly_outline_idx(6));
	engine::vertex_buffer().resize(_lives * 12);
	for (int i = 0; i < _lives; ++i) {
		const glm::ivec2 grid_pos{i % LIVES_PER_LINE, i / LIVES_PER_LINE};
		const vec2 pos{(static_cast<vec2>(grid_pos) + 0.5f) * 2.5f * life_size + 8.0f};

		fill_poly_outline_idx(indices.begin() + 36 * i, 6, 12 * i);
		tr::fill_poly_outline_vtx(positions(engine::vertex_buffer()).begin() + 12 * i, 6, {pos, life_size}, rotation, 4.0f);
		rs::fill(colors(engine::vertex_buffer()) | vs::drop(i * 12) | vs::take(6), rgba8{color, opacity});
		rs::fill(colors(engine::vertex_buffer()) | vs::drop(i * 12 + 6) | vs::take(6), rgba8{0, 0, 0, opacity});
	}
	engine::layered_renderer().add_color_mesh(layer::GAME_OVERLAY, engine::vertex_buffer(), std::move(indices));
}

void player::add_timer_to_renderer() const
{
	const string text{timer_text(_timer)};
	rgba8 tint;
	float scale;

	if (game_over()) {
		tint = (_timer > _pb) ? rgba8{0, 255, 0, 255} : rgba8{255, 0, 0, 255};
		scale = 1;
	}
	else {
		const float factor{min(_timer % 1_s, 0.2_s) / static_cast<float>(0.2_s)};
		const u8 tint_factor{norm_cast<u8>(1 - 0.25f * factor)};
		const u8 opacity{static_cast<u8>((255 - min(_timer_hover_time, HOVER_TIME) * 180 / HOVER_TIME) * tint_factor / 255)};
		tint = {tint_factor, tint_factor, tint_factor, opacity};
		scale = (1.25f - 0.25f * factor) / engine::render_scale();
	}

	vec2 tl{TIMER_TEXT_POS - timer_text_size(text, scale) / 2.0f};
	for (char chr : text) {
		array<tintvtx, 4> quad;
		fill_rect_vtx(positions(quad), {tl, _atlas[{&chr, 1}].size * vec2{_atlas.size()} * scale});
		fill_rect_vtx(uvs(quad), _atlas[{&chr, 1}]);
		rs::fill(colors(quad), tint);
		engine::layered_renderer().add_tex_quad(layer::GAME_OVERLAY, quad);
		tl.x += _atlas[{&chr, 1}].size.x * _atlas.size().x * scale - 5;
	}
}

void player::set_screen_shake() const
{
	const int screen_shake_left{static_cast<int>(_iframes - PLAYER_INVULN_TIME + SCREEN_SHAKE_TIME)};
	if (screen_shake_left >= 0) {
		const glm::mat4 mat{ortho(frect2{magth(40.0f * screen_shake_left / SCREEN_SHAKE_TIME, rand<fangle>(rng)), vec2{1000}})};
		for (int i = layer::BALL_TRAILS; i <= layer::BORDER; ++i) {
			engine::layered_renderer().set_layer_transform(i, mat);
		}
	}
}

void player::add_death_wave_to_renderer() const
{
	const float t{(_game_over_timer + 1) / static_cast<float>(0.5_s)};
	const float scale{std::sqrt(t) * 200};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};
	const u8 opacity{norm_cast<u8>(0.5f * std::sqrt(1 - t))};

	vector<clrvtx> fan{tr::smooth_poly_vtx(scale, engine::render_scale()) + 2};
	fan[0] = {_hitbox.c, {color, 0}};
	tr::fill_poly_vtx(positions(vs::drop(fan, 1)), fan.size() - 2, {_hitbox.c, scale});
	fan.back().pos = fan[1].pos;
	rs::fill(colors(vs::drop(fan, 1)), rgba8{color, opacity});
	engine::layered_renderer().add_color_fan(layer::PLAYER_TRAIL, fan);
}

void player::add_death_fragments_to_renderer() const
{
	const float t{(_game_over_timer + 1) / static_cast<float>(0.5_s)};
	const tr::rgb8 color{color_cast<tr::rgb8>(tr::hsv{static_cast<float>(settings.primary_hue), 1, 1})};
	const u8 opacity{norm_cast<u8>(std::sqrt(1 - t))};
	const float length{2 * _hitbox.r * tr::degs(30.0f).tan()};

	array<clrvtx, 4> quad;
	rs::fill(colors(quad), rgba8{color, opacity});
	for (const death_fragment& fragment : _fragments) {
		tr::fill_rotated_rect_vtx(positions(quad), fragment.pos, {length / 2, 2}, {length, 4}, fragment.rot);
		engine::layered_renderer().add_color_quad(layer::PLAYER, quad);
	}
}

/////////////////////////////////////////////////////////////// CONSTRUCTORS //////////////////////////////////////////////////////////////

player::player(const player_settings& settings, ticks pb) noexcept
	: _hitbox{{500, 500}, settings.size}
	, _trail{_hitbox.c}
	, _lives{static_cast<int>(settings.starting_lives)}
	, _inertia{settings.inertia}
	, _timer{0}
	, _game_over_timer{0}
	, _iframes{0}
	, _lives_hover_time{0}
	, _timer_hover_time{0}
	, _atlas{create_timer_atlas()}
	, _pb{pb}
{
	engine::layered_renderer().set_layer_texture(layer::GAME_OVERLAY, _atlas);
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

bool colliding(const player& player, const vector<ball>& balls) noexcept
{
	return player._iframes == 0 &&
		   rs::any_of(balls, [&](const ball& b) { return b.tangible() && intersecting(b.hitbox(), player._hitbox); });
}

///////////////////////////////////////////////////////////////// SETTERS /////////////////////////////////////////////////////////////////

void player::hit() noexcept
{
	--_lives;
	_iframes = PLAYER_INVULN_TIME;

	if (game_over()) {
		set_up_death_fragments();
		audio::play(sfx::EXPLOSION, 1, 0);
	}
	else {
		audio::play(sfx::HIT, 1, 0);
	}
}

void player::update() noexcept
{
	if (_iframes > 0) {
		--_iframes;
		set_screen_shake();
	}

	if (!game_over()) {
		++_timer;
		const float life_size{_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
		const int lives_in_line{min(_lives, LIVES_PER_LINE)};
		const int lines{_lives / LIVES_PER_LINE + 1};
		frect2 lives_ui_bounds{{}, {2.5f * life_size * (lives_in_line + 0.5f) + 16, 2.5f * life_size * lines + 16}};
		if (lives_ui_bounds.contains(_hitbox.c)) {
			_lives_hover_time = min(_lives_hover_time + 1, HOVER_TIME);
		}
		else if (_lives_hover_time > 0) {
			--_lives_hover_time;
		}

		const vec2 text_size{timer_text_size(timer_text(_timer), 1 / engine::render_scale())};
		const frect2 timer_text_bounds{TIMER_TEXT_POS - text_size / 2.0f - 8.0f, text_size + 16.0f};
		if (timer_text_bounds.contains(_hitbox.c)) {
			_timer_hover_time = min(_timer_hover_time + 1, HOVER_TIME);
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

void player::update(vec2 target) noexcept
{
	if (_iframes > 0) {
		--_iframes;
		set_screen_shake();
	}

	if (!game_over()) {
		++_timer;
		target = clamp(target, vec2{FIELD_MIN + _hitbox.r}, vec2{FIELD_MAX - _hitbox.r});

		_trail.push(_hitbox.c);
		if (_inertia == 0) {
			_hitbox.c = target;
		}
		else {
			_hitbox.c = _hitbox.c + (target - _hitbox.c) / 1_sf / _inertia;
		}

		const float life_size{_lives > MAX_LARGE_LIVES ? SMALL_LIFE_SIZE : LARGE_LIFE_SIZE};
		const int lives_in_line{min(_lives, LIVES_PER_LINE)};
		const int lines{_lives / LIVES_PER_LINE + 1};
		frect2 lives_ui_bounds{{}, {2.5f * life_size * (lives_in_line + 0.5f) + 16, 2.5f * life_size * lines + 16}};
		if (lives_ui_bounds.contains(target)) {
			_lives_hover_time = min(_lives_hover_time + 1, HOVER_TIME);
		}
		else if (_lives_hover_time > 0) {
			--_lives_hover_time;
		}

		const vec2 text_size{timer_text_size(timer_text(_timer), 1 / engine::render_scale())};
		const frect2 timer_text_bounds{TIMER_TEXT_POS - text_size / 2.0f - 8.0f, text_size + 16.0f};
		if (timer_text_bounds.contains(target)) {
			_timer_hover_time = min(_timer_hover_time + 1, HOVER_TIME);
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
	const u8 opacity{norm_cast<u8>(abs(std::cos(_iframes * PI * 8 / PLAYER_INVULN_TIME)))};
	const fangle rotation{tr::degs(270.0f * _timer / SECOND_TICKS)};
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