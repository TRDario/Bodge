#pragma once
#include "global.hpp"

// Tooltip manager.
class tooltip_manager {
  public:
	///////////////////////////////////////////////////////////// OPERATIONS //////////////////////////////////////////////////////////////

	// Adds the tooltip to the renderer.
	void add_to_renderer(std::string_view text);

  private:
	// Texture unit used for tooltip rendering.
	tr::texture_unit tex_unit;
	// The tooltip text texture.
	tr::texture texture{{128, 64}};
	// The last drawn text string.
	std::string last_text;
	// The size of the last drawn text string.
	glm::vec2 last_size{};

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Renders the tooltip text.
	void render(std::string_view text);
};