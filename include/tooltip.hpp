#pragma once
#include "global.hpp"

// Tooltip manager.
class tooltip {
  public:
	///////////////////////////////////////////////////////////// CONSTRUCTORS ////////////////////////////////////////////////////////////

	// Creates the tooltip.
	tooltip() = default;

	///////////////////////////////////////////////////////////// OPERATIONS //////////////////////////////////////////////////////////////

	// Adds the tooltip to the renderer.
	void add_to_renderer(std::string_view text);

  private:
	// Texture unit used for tooltip rendering.
	tr::texture_unit _tex_unit;
	// The tooltip text texture.
	tr::texture _texture{{128, 64}};
	// The last drawn text string.
	std::string _last_text;
	// The size of the last drawn text string.
	glm::vec2 _last_size;

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Renders the tooltip text.
	void render(std::string_view text);
};