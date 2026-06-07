///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Provides an interface for rendering UI tooltips.                                                                                      //
// 																																         //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "../global.hpp"

class text_engine;

///////////////////////////////////////////////////////////// TOOLTIP MANAGER /////////////////////////////////////////////////////////////

// User interface tooltip rendering manager.
class tooltip_manager {
  public:
	// Creates a tooltip manager.
	tooltip_manager(tr::gfx::renderer_2d& renderer);

	// Adds the tooltip to the renderer.
	void add(text_engine& text_engine, tr::gfx::renderer_2d& renderer, float scale, glm::vec2 tl, std::string_view text_string);

  private:
	// Cached text texture.
	tr::gfx::texture m_texture{{128, 64}};
	// Cached text string.
	std::string m_last_text;
	// Cached text size.
	glm::vec2 m_last_size{};

	// Renders the tooltip text.
	void render_text(text_engine& text_engine, float scale, std::string_view text_string);
};