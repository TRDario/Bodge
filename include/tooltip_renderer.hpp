#pragma once
#include "global.hpp"

// Tooltip renderer.
class tooltip_renderer {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates the tooltip renderer.
	tooltip_renderer() = default;

	///////////////////////////////////////////////////////////// OPERATIONS //////////////////////////////////////////////////////////////

	// Draws a tooltip to the screen.
	void draw(std::string_view text);

  private:
	// Texture unit used for tooltip rendering.
	tr::texture_unit _tex_unit;
	// The tooltip text texture.
	tr::texture _texture{{128, 64}};
	// The last drawn text string.
	std::string_view _last_text;
	// The size of the last drawn text string.
	glm::vec2 _last_size;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Renders the tooltip text.
	void render(std::string_view text);
};