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
	tr::gfx::texture_unit m_tex_unit;
	// The tooltip text texture.
	tr::gfx::texture m_texture{{128, 64}};
	// The last drawn text string.
	std::string m_last_text;
	// The size of the last drawn text string.
	glm::vec2 m_last_size{};

	/////////////////////////////////////////////////////////////// HELPERS ///////////////////////////////////////////////////////////////

	// Renders the tooltip text.
	void render(std::string_view text);
};