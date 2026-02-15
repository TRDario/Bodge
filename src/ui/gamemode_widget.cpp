#include "../../include/ui/widget.hpp"

///////////////////////////////////////////////////////////// GAMEMODE WIDGET /////////////////////////////////////////////////////////////

gamemode_widget::gamemode_widget(tweened_position pos, tr::align alignment, ticks unhide_time, status_callback status_cb,
								 gamemode_widget_action_callback action_cb, std::optional<gamemode_with_path> gamemode)
	: gamemode_widget_data{std::move(gamemode)}
	, text_button_widget{
		  pos,
		  alignment,
		  unhide_time,
		  [this] { return this->gp.has_value() ? std::string{this->gp->gamemode.description_loc()} : ""; },
		  [this] { return this->gp.has_value() ? std::string{this->gp->gamemode.name_loc()} : "---------------------------"; },
		  font::LANGUAGE,
		  64,
		  [scb = std::move(status_cb), this] { return this->gp.has_value() && scb(); },
		  [acb = std::move(action_cb), this] { acb(*this->gp); },
		  sound::CONFIRM}
{
}