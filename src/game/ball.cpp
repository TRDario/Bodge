#include "../../include/audio.hpp"
#include "../../include/engine.hpp"
#include "../../include/game/ball.hpp"

//////////////////////////////////////////////////////////////// CONSTANTS ////////////////////////////////////////////////////////////////

// The amount of time it takes a ball to become tangible.
inline constexpr ticks BALL_SPAWN_TIME{1.5_s};
// The amount of time it takes a ball to become tangible.
inline constexpr ticks BALL_COLLISION_TIME{0.1_s};
// Ball fill color.
inline constexpr tr::rgb8 BALL_FILL_COLOR{0, 0, 0};

///////////////////////////////////////////////////////////////// HELPERS /////////////////////////////////////////////////////////////////

void play_ball_sound(vec2 pos, float velocity)
{
	const float pan{(pos.x - 500) / 500};
	audio::play(sfx::BOUNCE, velocity / 2000, pan, clamp(velocity / 600, 0.5f, 1.25f));
}

////////////////////////////////////////////////////////////// CONSTRUCTORS ///////////////////////////////////////////////////////////////

ball::ball(const tr::fcircle& hitbox, const vec2& velocity) noexcept
	: _hitbox{hitbox}, _trail{hitbox.c}, _velocity{velocity}, _age{0}, _last_collision{0}
{
}

ball::ball(tr::xorshiftr_128p& rng, float size, float velocity) noexcept
	: ball{{{rand(rng, FIELD_MIN + size, FIELD_MAX - size), rand(rng, FIELD_MIN + size, FIELD_MAX - size)}, size},
		   magth(velocity, rand<fangle>(rng))}
{
}

///////////////////////////////////////////////////////////////// GETTERS /////////////////////////////////////////////////////////////////

bool ball::tangible() const noexcept
{
	return _age >= BALL_SPAWN_TIME;
}

const tr::fcircle& ball::hitbox() const noexcept
{
	return _hitbox;
}

bool colliding(const ball& a, const ball& b) noexcept
{
	return intersecting(a.hitbox(), b.hitbox()) && glm::dot(a._hitbox.c - b._hitbox.c, b._velocity - a._velocity) >= 0;
}

///////////////////////////////////////////////////////////////// SETTERS /////////////////////////////////////////////////////////////////

void handle_collision(ball& a, ball& b) noexcept
{
	const vec2 dist_vec{a._hitbox.c - b._hitbox.c};
	const vec2 vel_diff{a._velocity - b._velocity};
	const vec2 dist2{pow(dist_vec.x, 2.0f) + pow(dist_vec.y, 2.0f)};
	const float a_mass{pow(a._hitbox.r, 2.0f)};
	const float b_mass{pow(b._hitbox.r, 2.0f)};
	const float total_mass{a_mass + b_mass};

	play_ball_sound(a._hitbox.c + dist_vec / 2.0f, max(length(a._velocity), length(b._velocity)));

	a._velocity -= ((2 * b_mass / total_mass) * (dot(dist_vec, vel_diff) / dist2) * dist_vec);
	b._velocity -= ((2 * a_mass / total_mass) * (dot(-dist_vec, -vel_diff) / dist2) * -dist_vec);

	a._last_collision = 0;
	b._last_collision = 0;
}

void ball::update() noexcept
{
	++_age;
	++_last_collision;

	if (_age >= BALL_SPAWN_TIME) {
		_trail.push(_hitbox.c);

		vec2 target{_hitbox.c + _velocity / 1_sf};
		vec2 clamped{tr::mirror_repeat(target, vec2{FIELD_MIN + _hitbox.r}, vec2{FIELD_MAX - _hitbox.r})};

		if (target == clamped) {
			_hitbox.c = target;
			return;
		}

		if (clamped.x > target.x) {
			_velocity.x = abs(_velocity.x);
		}
		else if (clamped.x < target.x) {
			_velocity.x = -abs(_velocity.x);
		}

		if (clamped.y > target.y) {
			_velocity.y = abs(_velocity.y);
		}
		else if (clamped.y < target.y) {
			_velocity.y = -abs(_velocity.y);
		}

		if (clamped != target) {
			play_ball_sound(clamped, length(_velocity));
		}

		_hitbox.c = clamped;
		_last_collision = 0;
	}
}

///////////////////////////////////////////////////////////////// GRAPHICS ////////////////////////////////////////////////////////////////

void ball::add_to_renderer() const
{
	const tr::rgb8 tint{tr::color_cast<tr::rgb8>(tr::hsv{static_cast<float>(settings.secondary_hue), 1, 1})};
	const float raw_age_factor{min(static_cast<float>(_age) / BALL_SPAWN_TIME, 1.0f)};
	const float eased_age_factor{raw_age_factor == 1.0f ? raw_age_factor : 1.0f - pow(2.0f, -10.0f * raw_age_factor)};
	const float size{_hitbox.r * (5 - 4 * eased_age_factor)};
	const size_t vertices{tr::smooth_poly_vtx(size, engine::render_scale())};
	const u8 opacity{norm_cast<u8>(raw_age_factor)};
	const float thickness{3 + 4 * max((static_cast<float>(BALL_COLLISION_TIME) - _last_collision) / BALL_COLLISION_TIME, 0.0f)};

	// Add the ball.
	const color_alloc fill{tr::renderer_2d::new_color_fan(layer::BALLS, vertices)};
	tr::fill_poly_vtx(fill.positions, vertices, {_hitbox.c - thickness / 2, size});
	rs::fill(fill.colors, rgba8{BALL_FILL_COLOR, opacity});
	const color_alloc outline{tr::renderer_2d::new_color_outline(layer::BALLS, vertices)};
	tr::fill_poly_outline_vtx(outline.positions, vertices, {_hitbox.c, size}, 0_degf, thickness);
	rs::fill(outline.colors, rgba8{tint, opacity});

	// Add the trail.
	if (_age > BALL_SPAWN_TIME) {
		size_t trail_vertices{vertices};
		for (size_t i = 0; i < _trail.SIZE; ++i) {
			if (!(i == 0 && tr::collinear(_hitbox.c, _trail[0], _trail[1])) &&
				!(i > 0 && i < _trail.SIZE - 1 && tr::collinear(_trail[i - 1], _trail[i], _trail[i + 1]))) {
				trail_vertices += vertices;
			}
		}
		const size_t trail_indices{6 * (trail_vertices - 1)};

		color_mesh_alloc trail{tr::renderer_2d::new_color_mesh(layer::BALL_TRAILS, trail_vertices, trail_indices)};
		tr::fill_poly_vtx(trail.positions | vs::take(vertices), vertices, _hitbox);
		rs::fill(trail.colors | vs::take(vertices), rgba8{tint, norm_cast<u8>(0.4f)});
		size_t drawn_trails{1};
		std::vector<u16>::iterator indices_it{trail.indices.begin()};
		for (size_t i = 0; i < _trail.SIZE; ++i) {
			// Cull unnecessary trail vertices.
			if ((i == 0 && tr::collinear(_hitbox.c, _trail[0], _trail[1])) ||
				(i > 0 && i < _trail.SIZE - 1 && tr::collinear(_trail[i - 1], _trail[i], _trail[i + 1]))) {
				continue;
			}

			const u8 opacity{norm_cast<u8>((_trail.SIZE - i - 1) * 0.4f / _trail.SIZE)};
			tr::fill_poly_vtx(trail.positions | vs::drop(drawn_trails * vertices) | vs::take(vertices), vertices, {_trail[i], _hitbox.r});
			rs::fill(trail.colors | vs::drop(drawn_trails * vertices) | vs::take(vertices), rgba8{tint, opacity});
			for (size_t j = 0; j < vertices; ++j) {
				*indices_it++ = trail.base_index + drawn_trails * vertices + j;
				*indices_it++ = trail.base_index + drawn_trails * vertices + (j + 1) % vertices;
				*indices_it++ = trail.base_index + (drawn_trails - 1) * vertices + (j + 1) % vertices;
				*indices_it++ = trail.base_index + drawn_trails * vertices + j;
				*indices_it++ = trail.base_index + (drawn_trails - 1) * vertices + (j + 1) % vertices;
				*indices_it++ = trail.base_index + (drawn_trails - 1) * vertices + j;
			}
			++drawn_trails;
		}
	}
}