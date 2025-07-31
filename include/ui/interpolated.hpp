#pragma once
#include "../global.hpp"

// Floating-point value with support for interpolation.
class interpolated_float {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a float with a value.
	interpolated_float(float value);
	// Constructs a float with an ongoing interpolation.
	interpolated_float(float start, float end, std::uint16_t time);

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update();
	// Gets the current value of the float.
	operator float() const;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(float end, ticks time);
	// Sets the float.
	interpolated_float& operator=(float r);

  private:
	// The value at the start of the interpolation.
	float start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	float end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	std::uint16_t len{0};
	// The current position within the interpolation.
	std::uint16_t pos;
};

// Two-dimensional vector value with support for interpolation.
class interpolated_vec2 {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a vec2 with a value.
	interpolated_vec2(glm::vec2 value);
	// Constructs a vec2 with an ongoing interpolation.
	interpolated_vec2(glm::vec2 start, glm::vec2 end, std::uint16_t time);

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update();
	// Gets the current value of the vector.
	operator glm::vec2() const;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(glm::vec2 end, ticks time);
	// Sets the vector.
	interpolated_vec2& operator=(glm::vec2 r);

  private:
	// The value at the start of the interpolation.
	glm::vec2 start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	glm::vec2 end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	std::uint16_t len{0};
	// The current position within the interpolation.
	std::uint16_t pos;
};

// RGBA value with support for interpolation.
class interpolated_rgba8 {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a color with a value.
	interpolated_rgba8(tr::rgba8 value);
	// Constructs a color with an ongoing interpolation.
	interpolated_rgba8(tr::rgba8 start, tr::rgba8 end, std::uint16_t time);

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update();
	// Gets the current value of the color.
	operator tr::rgba8() const;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(tr::rgba8 end, ticks time);
	// Sets the color.
	interpolated_rgba8& operator=(tr::rgba8 r);

  private:
	// The value at the start of the interpolation.
	tr::rgba8 start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	tr::rgba8 end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	std::uint16_t len{0};
	// The current position within the interpolation.
	std::uint16_t pos;
};