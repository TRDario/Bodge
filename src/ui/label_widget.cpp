///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements label_widget from ui/widget.hpp.                                                                                           //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/ui/widget.hpp"

/////////////////////////////////////////////////////////////// LABEL WIDGET //////////////////////////////////////////////////////////////

label_widget::label_widget(tweened_position pos, tr::align alignment, ticks unhide_time, text_callback tooltip_cb, text_callback text_cb,
						   tr::sys::ttf_style style, float font_size, tr::rgba8 color)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  std::move(tooltip_cb),
				  std::move(text_cb),
				  font::LANGUAGE,
				  style,
				  font_size,
				  tr::sys::UNLIMITED_WIDTH}
	, tint{color}
{
}

void label_widget::tick()
{
	widget::tick();
	tint.tick();
}

void label_widget::add_to_renderer()
{
	add_to_renderer_raw(tint);
}