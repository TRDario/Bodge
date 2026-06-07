///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements label_widget from ui/widget.hpp.                                                                                           //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/ui/widget.hpp"

/////////////////////////////////////////////////////////////// LABEL WIDGET //////////////////////////////////////////////////////////////

label_widget::label_widget(properties&& properties)
	: text_widget{properties.animation,       properties.alignment, properties.unhide_time, std::move(properties.tooltip_text),
				  std::move(properties.text), font::LANGUAGE,       properties.font_style,  properties.font_size,
				  tr::sys::UNLIMITED_WIDTH}
	, tint{properties.color}
{
}

void label_widget::tick()
{
	widget::tick();
	tint.tick();
}

void label_widget::add_to_renderer(renderer& renderer)
{
	add_to_renderer_raw(renderer, tint);
}