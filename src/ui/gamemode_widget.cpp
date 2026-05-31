///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements gamemode_widget from ui/widget.hpp.                                                                                        //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../include/ui/widget.hpp"

///////////////////////////////////////////////////////////// GAMEMODE WIDGET /////////////////////////////////////////////////////////////

gamemode_widget::gamemode_widget(tweened_position pos, tr::align alignment, ticks unhide_time, status_command status_command,
								 gamemode_widget_action_command action_command, std::optional<gamemode_with_path> gamemode)
	: gamemode_widget_data{std::move(gamemode)}
	, text_button_widget{pos,
						 alignment,
						 unhide_time,
						 [this] { return gp.has_value() ? std::string{gp->gamemode.description_loc()} : ""; },
						 [this] { return gp.has_value() ? std::string{gp->gamemode.name_loc()} : "---------------------------"; },
						 font::LANGUAGE,
						 64,
						 [status = std::move(status_command), this] { return gp.has_value() && status(); },
						 [action = std::move(action_command), this] { action(*gp); },
						 sound::CONFIRM}
{
}