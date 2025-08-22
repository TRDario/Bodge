#pragma once
#include "global.hpp"

class tooltip_manager {
  public:
	void add_to_renderer(std::string_view text);

  private:
	tr::gfx::texture_unit m_tex_unit;
	tr::gfx::texture m_texture{{128, 64}};
	std::string m_last_text;
	glm::vec2 m_last_size{};

	void render(std::string_view text);
};