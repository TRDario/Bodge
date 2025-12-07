#pragma once
#include "../global.hpp"

///////////////////////////////////////////////////////////// TOOLTIP MANAGER /////////////////////////////////////////////////////////////

// User interface tooltip rendering manager.
class tooltip_manager {
  public:
	// Creates a tooltip manager.
	tooltip_manager(tr::gfx::renderer_2d& basic);

	// Adds the tooltip to the renderer.
	void add(std::string_view text);

  private:
	// Cached text texture.
	tr::gfx::texture m_texture{{128, 64}};
	// Cached text string.
	std::string m_last_text;
	// Cached text size.
	glm::vec2 m_last_size{};

	// Renders the tooltip text.
	void render_text(std::string_view text);
};