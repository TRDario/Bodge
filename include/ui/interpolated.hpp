#pragma once
#include "../global.hpp"

// Floating-point value with support for interpolation.
class interpolated_float {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a float with a value.
	interpolated_float(float value) noexcept;
	// Constructs a float with an ongoing interpolation.
	interpolated_float(float start, float end, u16 time) noexcept;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update() noexcept;
	// Gets the current value of the float.
	operator float() const noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(float end, u16 time) noexcept;
	// Sets the float.
	interpolated_float& operator=(float r) noexcept;

  private:
	// The value at the start of the interpolation.
	float _start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	float _end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	u16 _len{0};
	// The current position within the interpolation.
	u16 _pos;
};

// Two-dimensional vector value with support for interpolation.
class interpolated_vec2 {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a vec2 with a value.
	interpolated_vec2(vec2 value) noexcept;
	// Constructs a vec2 with an ongoing interpolation.
	interpolated_vec2(vec2 start, vec2 end, u16 time) noexcept;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update() noexcept;
	// Gets the current value of the vector.
	operator vec2() const noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(vec2 end, u16 time) noexcept;
	// Sets the vector.
	interpolated_vec2& operator=(vec2 r) noexcept;

  private:
	// The value at the start of the interpolation.
	vec2 _start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	vec2 _end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	u16 _len{0};
	// The current position within the interpolation.
	u16 _pos;
};

// RGBA value with support for interpolation.
class interpolated_rgba8 {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a color with a value.
	interpolated_rgba8(rgba8 value) noexcept;
	// Constructs a color with an ongoing interpolation.
	interpolated_rgba8(rgba8 start, rgba8 end, u16 time) noexcept;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update() noexcept;
	// Gets the current value of the color.
	operator rgba8() const noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(rgba8 end, u16 time) noexcept;
	// Sets the color.
	interpolated_rgba8& operator=(rgba8 r) noexcept;

  private:
	// The value at the start of the interpolation.
	rgba8 _start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	rgba8 _end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	u16 _len{0};
	// The current position within the interpolation.
	u16 _pos;
};