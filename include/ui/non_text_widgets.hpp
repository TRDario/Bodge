#pragma once
#include "widget.hpp"

// Color preview widget.
class color_preview_widget : public widget {
  public:
	//////////////////////////////////////////////////////////// CONSTRUCTORS /////////////////////////////////////////////////////////////

	// Creates a color preview widget.
	color_preview_widget(glm::vec2 pos, tr::align alignment, std::uint16_t& hue_ref) noexcept;

	/////////////////////////////////////////////////////////// VIRTUAL METHODS ///////////////////////////////////////////////////////////

	glm::vec2 size() const noexcept override;
	void add_to_renderer() override;

  private:
	// Reference to the hue value.
	std::uint16_t& _hue_ref;
};