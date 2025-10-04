#include "../../include/ui/widget.hpp"

label_widget::label_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb,
						   text_style style, float font_size, tr::rgba8 color)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  std::move(tooltip_cb),
				  false,
				  std::move(text_cb),
				  font::LANGUAGE,
				  style,
				  font_size,
				  tr::sys::UNLIMITED_WIDTH}
	, color{color}
{
}

void label_widget::update()
{
	widget::update();
	color.update();
}

void label_widget::add_to_renderer()
{
	add_to_renderer_raw(color);
}