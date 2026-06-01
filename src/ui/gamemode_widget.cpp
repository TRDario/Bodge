///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements gamemode_widget from ui/widget.hpp.                                                                                        //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/ui/widget.hpp"

///////////////////////////////////////////////////////////// GAMEMODE WIDGET /////////////////////////////////////////////////////////////

gamemode_widget::gamemode_widget(properties&& properties)
	: gamemode_widget_data{std::move(properties.gamemode)}
	, text_button_widget{{
		  .animation = properties.animation,
		  .alignment = properties.alignment,
		  .unhide_time = properties.unhide_time,
		  .tooltip_text = [this] { return gp.has_value() ? std::string{gp->gamemode.description_loc()} : ""; },
		  .text = [this] { return gp.has_value() ? std::string{gp->gamemode.name_loc()} : "---------------------------"; },
		  .font = font::LANGUAGE,
		  .font_size = 64,
		  .status = [status = std::move(properties.status), this] { return gp.has_value() && status(); },
		  .action = [action = std::move(properties.action), this] { action(*gp); },
	  }}
{
}