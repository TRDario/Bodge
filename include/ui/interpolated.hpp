#pragma once
#include "../global.hpp"

// Floating-point value with support for interpolation.
class interpolated_float {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a float with a value.
	interpolated_float(float value) noexcept;
	// Constructs a float with an ongoing interpolation.
	interpolated_float(float start, float end, std::uint16_t time) noexcept;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update() noexcept;
	// Gets the current value of the float.
	operator float() const noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(float end, std::uint16_t time) noexcept;
	// Sets the float.
	interpolated_float& operator=(float r) noexcept;

  private:
	// The value at the start of the interpolation.
	float _start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	float _end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	std::uint16_t _len{0};
	// The current position within the interpolation.
	std::uint16_t _pos;
};

// Two-dimensional vector value with support for interpolation.
class interpolated_vec2 {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a vec2 with a value.
	interpolated_vec2(glm::vec2 value) noexcept;
	// Constructs a vec2 with an ongoing interpolation.
	interpolated_vec2(glm::vec2 start, glm::vec2 end, std::uint16_t time) noexcept;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update() noexcept;
	// Gets the current value of the vector.
	operator glm::vec2() const noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(glm::vec2 end, std::uint16_t time) noexcept;
	// Sets the vector.
	interpolated_vec2& operator=(glm::vec2 r) noexcept;

  private:
	// The value at the start of the interpolation.
	glm::vec2 _start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	glm::vec2 _end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	std::uint16_t _len{0};
	// The current position within the interpolation.
	std::uint16_t _pos;
};

// RGBA value with support for interpolation.
class interpolated_rgba8 {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Constructs a color with a value.
	interpolated_rgba8(tr::rgba8 value) noexcept;
	// Constructs a color with an ongoing interpolation.
	interpolated_rgba8(tr::rgba8 start, tr::rgba8 end, std::uint16_t time) noexcept;

	/////////////////////////////////////////////////////////////// GETTERS ///////////////////////////////////////////////////////////////

	// Updates the interpolation.
	void update() noexcept;
	// Gets the current value of the color.
	operator tr::rgba8() const noexcept;

	/////////////////////////////////////////////////////////////// SETTERS ///////////////////////////////////////////////////////////////

	// Begins an interpolation.
	void change(tr::rgba8 end, std::uint16_t time) noexcept;
	// Sets the color.
	interpolated_rgba8& operator=(tr::rgba8 r) noexcept;

  private:
	// The value at the start of the interpolation.
	tr::rgba8 _start;
	// The value at the end of the interpolated (or the current value if no interpolation is in progress).
	tr::rgba8 _end;
	// The length of the interpolation, or 0 to mark no ongoing interpolation.
	std::uint16_t _len{0};
	// The current position within the interpolation.
	std::uint16_t _pos;
};