///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                                                       //
// Widget hierarchy:                                                                                                                     //
// • widget                               - Base widget interface.                                                                       //
//     • text_widget                      - Base text widget interface.                                                                  //
//         • label_widget                 - Displays text.                                                                               //
//         • text_button_widget           - Interactive labeled button.                                                                  //
//             • replay_widget            - Displays replay information, interactive.                                                    //
//         • basic_numeric_input_widget   - Takes numeric input.                                                                         //
//         • line_input_widget            - Takes text input on a single line.                                                           //
//         • multiline_input_widget       - Takes text input across multiple lines.                                                      //
//         • score_widget                 - Displays score information.                                                                  //
//     • image_widget                     - Displays an image.                                                                           //
//     • color_preview_widget             - Displays a hue.                                                                              //
//     • arrow_widget                     - Interactive left or right arrow button.                                                      //
//     • replay_playback_indicator_widget - Displays a playback speed indicator (<, >, >>).                                              //
//                                                                                                                                       //
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ui/widget_base.hpp"

/////////////////////////////////////////////////////////////// UI MANAGER ////////////////////////////////////////////////////////////////

// User interface manager.
class ui_manager {
  public:
	// Creates an empty interface.
	ui_manager(selection_tree selection_tree, shortcut_table shortcuts);

	// Gets access to a widget in the interface.
	widget& operator[](tag tag);
	// Gets access to a widget in the interface.
	template <class T> T& as(tag tag);

	// Emplaces a new widget into the interface.
	template <class T, class... Args>
		requires(std::constructible_from<T, Args...>)
	T& emplace(tag tag, Args&&... args);
	// Replaces the widgets in the interface with a new set of widgets.
	void replace(std::unordered_map<tag, std::unique_ptr<widget>>&& widgets);

	// Unselects the selected widget, if it exists.
	void clear_selection();
	// Selects a specific widget.
	void select_widget(tag tag);
	// Selects the next widget in the selection tree, or loops around.
	void select_next_widget();

	// Gradually hides all widgets in the interface.
	void hide_all_widgets(ticks time);
	// Releases the graphical resources of all widgets in the interface.
	void release_graphical_resources();

	// Handles user interface events.
	void handle_event(const tr::sys::event& event);
	// Updates the state of all widgets in the interface.
	void tick();
	// Adds all widgets of the interface to the renderer.
	void add_to_renderer();

  private:
	// Shorthand for the widget map value type.
	using kv_pair = std::pair<const tag, std::unique_ptr<widget>>;
	// Structure returned by find_in_selection_tree().
	struct selection_node {
		// Iterator to the row the tag is in.
		const selection_tree_row* row;
		// Iterator to the tag within the row.
		const tag* tag;
	};

	// Tree defining the keyboard navigation topology of the interface.
	selection_tree m_selection_tree;
	// List of widgets in the interface.
	std::unordered_map<tag, std::unique_ptr<widget>> m_widgets;
	// Table of shortcuts mapped to widgets in the interface.
	std::unordered_map<tr::sys::key_chord, tag> m_shortcuts;
	// Reference to the currently hovered-over object (if it exists).
	tr::opt_ref<kv_pair> m_hovered;
	// Reference to the currently selected object (if it exists).
	tr::opt_ref<kv_pair> m_selection;

	// Finds a tag in the selection tree.
	selection_node find_in_selection_tree(tag tag) const;

	// Sets the selected object (or std::nullopt to unselect).
	void change_selection(tr::opt_ref<kv_pair> new_selection);
	// Selects the first widget in the selection tree.
	void select_first_widget();
	// Selects the last widget in the selection tree.
	void select_last_widget();
	// Selects the previous widget in the selection tree, or loops around.
	void select_prev_widget();
	// Selects the widget above the currently selected one, or loops around.
	void select_widget_above();
	// Selects the widget below the currently selected one, or loops around.
	void select_widget_below();
	// Selects the widget to the left of the currently selected one, or loops around.
	void select_widget_to_the_left();
	// Selects the widget to the right of the currently selected one, or loops around.
	void select_widget_to_the_right();

	// Handles a mouse motion event.
	void handle_mouse_motion_event();
	// Handles a mouse button press event.
	void handle_mouse_down_event(const tr::sys::mouse_down_event& event);
	// Handles a mouse button unpress event.
	void handle_mouse_up_event(const tr::sys::mouse_up_event& event);
	// Handles a keyboard key press event.
	void handle_key_down_event(const tr::sys::key_down_event& event);
	// Handles a text input event.
	void handle_text_input_event(const tr::sys::text_input_event& event);
};

///////////////////////////////////////////////////////////// IMPLEMENTATION //////////////////////////////////////////////////////////////

template <class T> T& ui_manager::as(tag tag)
{
	return (T&)((*this)[tag]);
}

template <class T, class... Args>
	requires(std::constructible_from<T, Args...>)
T& ui_manager::emplace(tag tag, Args&&... args)
{
	return (T&)(*m_widgets.emplace(tag, std::make_unique<T>(std::forward<Args>(args)...)).first->second);
}