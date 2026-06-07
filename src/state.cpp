///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Implements state.hpp.                                                                                                                 //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../include/state.hpp"

////////////////////////////////////////////////////////////// CURRENT STATE //////////////////////////////////////////////////////////////

current_state::current_state()
{
	savefile{}.unnamed() ? emplace<name_entry_state>() : emplace<title_state>();
}

current_state& current_state::instance()
{
	static current_state instance{};
	return instance;
}

//

state* current_state::operator->()
{
	return &state_machine::get<state>();
}

//

tr::sys::signal current_state::handle_event(const tr::sys::event& event)
{
	if (event.is<tr::sys::quit_event>()) {
		clear();
		return tr::sys::signal::SUCCESS;
	}
	else {
		state_machine::handle_event(event);
		return empty() ? tr::sys::signal::SUCCESS : tr::sys::signal::CONTINUE;
	}
}

tr::sys::signal current_state::tick()
{
	state_machine::tick();
	return empty() ? tr::sys::signal::SUCCESS : tr::sys::signal::CONTINUE;
}