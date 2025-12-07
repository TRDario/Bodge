#pragma once
#include "../global.hpp"

//////////////////////////////////////////////////////////// TWEENED POSITION /////////////////////////////////////////////////////////////

// Interpolated position value.
class tweened_position {
  public:
	// Creates a constant position.
	constexpr tweened_position(glm::vec2 value);
	// Creates an interpolated position.
	constexpr tweened_position(glm::vec2 start, glm::vec2 end, ticks time);

	// Updates the position in the interpolation.
	void tick();
	// Gets whether the position is done interpolating.
	bool done() const;
	// Gets the current value of the position.
	operator glm::vec2() const;

	// Gradually changes the position.
	void move(glm::vec2 end, ticks time);
	// Gradually changes the position horizontally.
	void move_x(float end, ticks time);
	// Gradually changes the position vertically.
	void move_y(float end, ticks time);
	// Sets the position.
	tweened_position& operator=(glm::vec2 r);

  private:
	// The starting point of the interpolation.
	glm::vec2 m_start;
	// The ending point of the interpolation.
	glm::vec2 m_end;
	// The duration of the interpolation, or 0 to mark a constant.
	ticks m_duration;
	// The elapsed interpolation time.
	ticks m_elapsed;
};

////////////////////////////////////////////////////////////// TWEENED COLOR //////////////////////////////////////////////////////////////

// Whether a tweened color should cycle.
enum class cycle : bool {
	NO,
	YES
};

// Interpolated color value.
class tweened_color {
  public:
	// Creates a constant color.
	constexpr tweened_color(tr::rgba8 value);
	// Creates an interpolated color.
	constexpr tweened_color(tr::rgba8 start, tr::rgba8 end, ticks time, cycle cycle = cycle::NO);

	// Updates the position in the interpolation.
	void tick();
	// Gets whether the position is done interpolating.
	bool done() const;
	// Gets whether the color is cycling between two values.
	bool cycling() const;
	// Gets the current value of the color.
	operator tr::rgba8() const;

	// Gradually changes the color.
	void change(tr::rgba8 end, ticks time, cycle cycle = cycle::NO);
	// Sets the color.
	tweened_color& operator=(tr::rgba8 r);

  private:
	// The starting point of the interpolation.
	tr::rgba8 m_start;
	// The ending point of the interpolation.
	tr::rgba8 m_end;
	// The duration of the interpolation, or 0 to mark a constant.
	ticks m_duration;
	// The elapsed interpolation time.
	ticks m_elapsed;
	// Whether the interpolation is one-time or constantly cycling.
	bool m_cycling;
};

///////////////////////////////////////////////////////////// TWEENED OPACITY /////////////////////////////////////////////////////////////

// Interpolated opacity value.
class tweened_opacity {
  public:
	// Creates a constant opacity.
	constexpr tweened_opacity(float value);
	// Creates an interpolated opacity.
	constexpr tweened_opacity(float start, float end, ticks time);

	// Updates the position in the interpolation.
	void tick();
	// Gets whether the position is done interpolating.
	bool done() const;
	// Gets the current value of the opacity.
	operator float() const;

	// Gradually changes the opacity.
	void change(float end, ticks time);
	// Sets the opacity.
	tweened_opacity& operator=(float r);

  private:
	// The starting point of the interpolation.
	float m_start;
	// The ending point of the interpolation.
	float m_end;
	// The duration of the interpolation, or 0 to mark a constant.
	ticks m_duration;
	// The elapsed interpolation time.
	ticks m_elapsed;
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

constexpr tweened_position::tweened_position(glm::vec2 value)
	: m_end{value}, m_duration{0}, m_elapsed{0}
{
}

constexpr tweened_position::tweened_position(glm::vec2 start, glm::vec2 end, ticks time)
	: m_start{start}, m_end{end}, m_duration{time}, m_elapsed{0}
{
}

//

constexpr tweened_color::tweened_color(tr::rgba8 value)
	: m_end{value}, m_duration{0}, m_elapsed{0}, m_cycling{false}
{
}

constexpr tweened_color::tweened_color(tr::rgba8 start, tr::rgba8 end, ticks time, cycle cycle)
	: m_start{start}, m_end{end}, m_duration{time}, m_elapsed{0}, m_cycling{bool(cycle)}
{
}

//

constexpr tweened_opacity::tweened_opacity(float value)
	: m_end{value}, m_duration{0}, m_elapsed{0}
{
}

constexpr tweened_opacity::tweened_opacity(float start, float end, ticks time)
	: m_start{start}, m_end{end}, m_duration{time}, m_elapsed{0}
{
}