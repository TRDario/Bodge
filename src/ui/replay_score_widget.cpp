#include "../../include/audio.hpp"
#include "../../include/graphics.hpp"
#include "../../include/ui/widget.hpp"

//

std::string format_score_tooltip(tr::opt_ref<const ::score_entry> score)
{
	if (!score.has_ref()) {
		return std::string{};
	}
	else {
		const score_flags flags{score->flags};
		std::string str{score->description};
		if ((flags.exited_prematurely || flags.modified_game_speed) && !str.empty()) {
			str.push_back('\n');
		}
		if (flags.exited_prematurely) {
			if (!str.empty()) {
				str.push_back('\n');
			}
			str.append(g_loc["exited_prematurely"]);
		}
		if (flags.modified_game_speed) {
			if (!str.empty()) {
				str.push_back('\n');
			}
			str.append(g_loc["modified_game_speed"]);
		}
		return str;
	}
}

std::string format_score_text(tr::opt_ref<const ::score_entry> score, score_widget::type type, usize rank)
{
	if (!score.has_ref()) {
		return std::string{"----------------------------------"};
	}

	switch (type) {
	case score_widget::type::TIME:
		return TR_FMT::format("{}) {} | {}", rank, format_time_long(score->time), format_timestamp(score->timestamp));
	case score_widget::type::SCORE:
		return TR_FMT::format("{}) {} | {}", rank, format_score(score->score), format_timestamp(score->timestamp));
	}
}

std::string format_replay_text(std::optional<std::map<std::string, replay_header>::const_iterator> it)
{
	if (!it.has_value()) {
		return std::string{"-------------------------------------------------"};
	}

	const replay_header& rpy{(*it)->second};
	return TR_FMT::format("{} ({}: {})\n{} | {} | {} | {}", rpy.name, g_loc["by"], rpy.player, rpy.gamemode.name_loc(),
						  format_score(rpy.score), format_time_long(rpy.time), format_timestamp(rpy.timestamp));
}

void add_exited_prematurely_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity)
{
	color = {color.r, color.g, color.b, u8(color.a * opacity)};

	tr::gfx::simple_color_mesh_ref mesh{g_graphics->basic_renderer.new_color_fan(layer::UI, 4)};
	tr::fill_rectangle_vertices(mesh.positions, {pos + glm::vec2{2, 2}, {16, 16}});
	std::ranges::fill(mesh.colors, tr::rgba8{0, 0, 0, tr::norm_cast<u8>(opacity)});
	mesh = g_graphics->basic_renderer.new_color_outline(layer::UI, 4);
	tr::fill_rectangle_outline_vertices(mesh.positions, {pos + glm::vec2{1, 1}, {18, 18}}, 2.0f);
	std::ranges::fill(mesh.colors, color);
	mesh = g_graphics->basic_renderer.new_color_fan(layer::UI, 4);
	tr::fill_rectangle_vertices(mesh.positions, pos + glm::vec2{10, 10}, {7, 1}, {14, 2}, 45_deg);
	std::ranges::fill(mesh.colors, color);
	mesh = g_graphics->basic_renderer.new_color_fan(layer::UI, 4);
	tr::fill_rectangle_vertices(mesh.positions, pos + glm::vec2{10, 10}, {7, 1}, {14, 2}, -45_deg);
	std::ranges::fill(mesh.colors, color);
}

void add_modified_game_speed_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity)
{
	color = {color.r, color.g, color.b, u8(color.a * opacity)};

	tr::gfx::simple_color_mesh_ref mesh{g_graphics->basic_renderer.new_color_fan(layer::UI, 4)};
	tr::fill_rectangle_vertices(mesh.positions, {pos + glm::vec2{2, 2}, {16, 16}});
	std::ranges::fill(mesh.colors, tr::rgba8{0, 0, 0, tr::norm_cast<u8>(opacity)});
	mesh = g_graphics->basic_renderer.new_color_outline(layer::UI, 4);
	tr::fill_rectangle_outline_vertices(mesh.positions, {pos + glm::vec2{1, 1}, {18, 18}}, 2);
	std::ranges::fill(mesh.colors, color);
	mesh = g_graphics->basic_renderer.new_color_outline(layer::UI, 8);
	tr::fill_regular_polygon_outline_vertices(mesh.positions, {pos + glm::vec2{10, 10}, 7}, 25_deg, 2);
	std::ranges::fill(mesh.colors, color);
	mesh = g_graphics->basic_renderer.new_color_fan(layer::UI, 4);
	tr::fill_rectangle_vertices(mesh.positions, {pos + glm::vec2{9, 5}, {2, 5}});
	std::ranges::fill(mesh.colors, color);
}

////////////////////////////////////////////////////////////// SCORE WIDGET ///////////////////////////////////////////////////////////////

score_widget::score_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, enum type type, usize rank,
						   tr::opt_ref<const ::score_entry> score)
	: text_widget{
		pos,
		alignment,
		unhide_time,
		string_text{format_score_tooltip(score)},
		false,
		string_text(format_score_text(score, type, rank)),
		font::LANGUAGE,
		text_style::NORMAL,
		48,
		tr::sys::UNLIMITED_WIDTH,
	},
	m_empty{!score.has_ref()},
	m_flags{score.has_ref() ? score->flags : score_flags{}}
{
}

glm::vec2 score_widget::size() const
{
	const auto icons{m_flags.exited_prematurely + m_flags.modified_game_speed};
	return icons != 0 ? text_widget::size() + glm::vec2{0, 20} : text_widget::size();
}

void score_widget::add_to_renderer()
{
	const tr::rgba8 color{m_empty ? "505050A0"_rgba8 : "A0A0A0A0"_rgba8};
	text_widget::add_to_renderer_raw(color);
	if (!m_empty) {
		const glm::vec2 text_size{text_widget::size()};
		const auto icons{m_flags.exited_prematurely + m_flags.modified_game_speed};
		glm::vec2 offset{text_size.x / 2 - 15 * icons, text_size.y};

		if (m_flags.exited_prematurely) {
			add_exited_prematurely_icon_to_renderer(tl() + offset, color, opacity());
			offset.x += 30;
		}
		if (m_flags.modified_game_speed) {
			add_modified_game_speed_icon_to_renderer(tl() + offset, color, opacity());
			offset.x += 30;
		}
	}
}

////////////////////////////////////////////////////////////// REPLAY_WIDGET //////////////////////////////////////////////////////////////

replay_widget::replay_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, status_callback scb,
							 replay_widget_action_callback acb, std::optional<std::map<std::string, replay_header>::const_iterator> it)
	: text_button_widget{
		  pos,
		  alignment,
		  unhide_time,
		  string_text{it.has_value() ? format_score_tooltip((*it)->second) : "-------------------------------------------------"},
		  string_text{format_replay_text(it)},
		  font::LANGUAGE,
		  34,
		  [=, this] { return scb() && m_it.has_value(); },
		  [=, this] {
			  if (m_it.has_value()) {
				  acb(*m_it);
			  }
		  },
		  sound::CONFIRM}
	, m_it{it}
{
}

glm::vec2 replay_widget::size() const
{
	if (m_it.has_value()) {
		const score_flags flags{(*m_it)->second.flags};
		const auto icons{flags.exited_prematurely + flags.modified_game_speed};
		if (icons != 0) {
			return text_button_widget::size() + glm::vec2{0, 20};
		}
	}
	return text_button_widget::size();
}

void replay_widget::add_to_renderer()
{
	if (!m_it.has_value()) {
		text_widget::add_to_renderer_raw("505050A0"_rgba8);
	}
	else {
		text_button_widget::add_to_renderer();
	}

	if (m_it.has_value()) {
		const score_flags flags{(*m_it)->second.flags};
		const glm::vec2 text_size{text_button_widget::size()};
		const tr::rgba8 color{interactible() ? tr::rgba8{m_interp} : "80808080"_rgba8};
		const auto icons{flags.exited_prematurely + flags.modified_game_speed};
		glm::vec2 offset{text_size.x / 2 - 15 * icons, text_size.y};

		if (flags.exited_prematurely) {
			add_exited_prematurely_icon_to_renderer(tl() + offset, color, opacity());
			offset.x += 30;
		}
		if (flags.modified_game_speed) {
			add_modified_game_speed_icon_to_renderer(tl() + offset, color, opacity());
			offset.x += 30;
		}
	}
}