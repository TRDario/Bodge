#include "../../include/audio.hpp"
#include "../../include/graphics.hpp"
#include "../../include/ui/widget.hpp"

//

namespace engine {
	void add_exited_prematurely_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity);
	void add_modified_game_speed_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity);
} // namespace engine

void engine::add_exited_prematurely_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity)
{
	color = {color.r, color.g, color.b, u8(color.a * opacity)};

	tr::gfx::simple_color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{2, 2}, {16, 16}});
	std::ranges::fill(mesh.colors, tr::rgba8{0, 0, 0, tr::norm_cast<u8>(opacity)});
	mesh = tr::gfx::renderer_2d::new_color_outline(layer::UI, 4);
	tr::fill_rect_outline_vtx(mesh.positions, {pos + glm::vec2{1, 1}, {18, 18}}, 2.0f);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::gfx::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(mesh.positions, pos + glm::vec2{10, 10}, {7, 1}, {14, 2}, 45_deg);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::gfx::renderer_2d::new_color_fan(layer::UI, 4);
	fill_rotated_rect_vtx(mesh.positions, pos + glm::vec2{10, 10}, {7, 1}, {14, 2}, -45_deg);
	std::ranges::fill(mesh.colors, color);
}

void engine::add_modified_game_speed_icon_to_renderer(glm::vec2 pos, tr::rgba8 color, float opacity)
{
	color = {color.r, color.g, color.b, u8(color.a * opacity)};

	tr::gfx::simple_color_mesh_ref mesh{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{2, 2}, {16, 16}});
	std::ranges::fill(mesh.colors, tr::rgba8{0, 0, 0, tr::norm_cast<u8>(opacity)});
	mesh = tr::gfx::renderer_2d::new_color_outline(layer::UI, 4);
	tr::fill_rect_outline_vtx(mesh.positions, {pos + glm::vec2{1, 1}, {18, 18}}, 2);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::gfx::renderer_2d::new_color_outline(layer::UI, 8);
	tr::fill_poly_outline_vtx(mesh.positions, 8, {pos + glm::vec2{10, 10}, 7}, 25_deg, 2);
	std::ranges::fill(mesh.colors, color);
	mesh = tr::gfx::renderer_2d::new_color_fan(layer::UI, 4);
	tr::fill_rect_vtx(mesh.positions, {pos + glm::vec2{9, 5}, {2, 5}});
	std::ranges::fill(mesh.colors, color);
}

////////////////////////////////////////////////////////////// SCORE WIDGET ///////////////////////////////////////////////////////////////

score_widget::score_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, usize rank, tr::opt_ref<::score> score)
	: text_widget{
		  pos,
		  alignment,
		  unhide_time,
		  [this] {
			  if (!this->score.has_value()) {
				  return std::string{};
			  }
			  else {
				  const score_flags flags{this->score->flags};
				  std::string str{this->score->description};
				  if ((flags.exited_prematurely || flags.modified_game_speed) && !str.empty()) {
					  str.push_back('\n');
				  }
				  if (flags.exited_prematurely) {
					  if (!str.empty()) {
						  str.push_back('\n');
					  }
					  str.append(engine::loc["exited_prematurely"]);
				  }
				  if (flags.modified_game_speed) {
					  if (!str.empty()) {
						  str.push_back('\n');
					  }
					  str.append(engine::loc["modified_game_speed"]);
				  }
				  return str;
			  }
		  },
		  false,
		  [this] {
			  if (!this->score.has_value()) {
				  return std::string{"----------------------------------"};
			  }

			  const ticks result{this->score->result};
			  const ticks result_m{result / 60_s};
			  const ticks result_s{(result % 60_s) / 1_s};
			  const ticks result_ms{(result % 1_s) * 100 / 1_s};
			  const tm* tm{std::localtime(&this->score->unix_timestamp)};
			  if (this->rank == DONT_SHOW_RANK) {
				  return TR_FMT::format("{}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", result_m, result_s, result_ms, tm->tm_year + 1900,
										tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
			  }
			  else {
				  return TR_FMT::format("{}) {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", this->rank, result_m, result_s, result_ms,
										tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
			  }
		  },
		  font::LANGUAGE,
		  tr::system::ttf_style::NORMAL,
		  48,
		  tr::system::UNLIMITED_WIDTH,
	  }
	, rank{rank}
	, score{score}
{
}

glm::vec2 score_widget::size() const
{
	if (score.has_value()) {
		const auto icons{score->flags.exited_prematurely + score->flags.modified_game_speed};
		if (icons != 0) {
			return text_widget::size() + glm::vec2{0, 20};
		}
	}
	return text_widget::size();
}

void score_widget::add_to_renderer()
{
	const tr::rgba8 color{score.has_value() ? "A0A0A0A0"_rgba8 : "505050A0"_rgba8};
	text_widget::add_to_renderer_raw(color);
	if (score.has_value()) {
		const score_flags flags{score->flags};
		const glm::vec2 text_size{text_widget::size()};
		const auto icons{flags.exited_prematurely + flags.modified_game_speed};
		glm::vec2 offset{text_size.x / 2 - 15 * icons, text_size.y};

		if (flags.exited_prematurely) {
			engine::add_exited_prematurely_icon_to_renderer(tl() + offset, color, opacity());
			offset.x += 30;
		}
		if (flags.modified_game_speed) {
			engine::add_modified_game_speed_icon_to_renderer(tl() + offset, color, opacity());
			offset.x += 30;
		}
	}
}

////////////////////////////////////////////////////////////// REPLAY_WIDGET //////////////////////////////////////////////////////////////

replay_widget::replay_widget(tweener<glm::vec2> pos, tr::align alignment, ticks unhide_time, status_callback scb,
							 replay_widget_action_callback acb, std::optional<std::map<std::string, replay_header>::iterator> it)
	: text_button_widget{pos,
						 alignment,
						 unhide_time,
						 [this] {
							 if (!this->it.has_value()) {
								 return std::string{};
							 }
							 else {
								 const replay_header& header{(*this->it)->second};
								 std::string str{header.description};
								 if ((header.flags.exited_prematurely || header.flags.modified_game_speed) && !str.empty()) {
									 str.push_back('\n');
								 }
								 if (header.flags.exited_prematurely) {
									 if (!str.empty()) {
										 str.push_back('\n');
									 }
									 str.append(engine::loc["exited_prematurely"]);
								 }
								 if (header.flags.modified_game_speed) {
									 if (!str.empty()) {
										 str.push_back('\n');
									 }
									 str.append(engine::loc["modified_game_speed"]);
								 }
								 return str;
							 }
						 },
						 [this] {
							 if (!this->it.has_value()) {
								 return std::string{"----------------------------------"};
							 }

							 replay_header& rpy{(*this->it)->second};
							 const ticks result{rpy.result};
							 const ticks result_m{result / 60_s};
							 const ticks result_s{(result % 60_s) / 1_s};
							 const ticks result_ms{(result % 1_s) * 100 / 1_s};
							 const tm* tm{std::localtime(&rpy.unix_timestamp)};
							 return TR_FMT::format("{} ({}: {})\n{} | {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", rpy.name,
												   engine::loc["by"], rpy.player, rpy.gamemode.name_loc(), result_m, result_s, result_ms,
												   tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min);
						 },
						 font::LANGUAGE,
						 40,
						 [=, this] { return scb() && this->it.has_value(); },
						 [=, this] {
							 if (this->it.has_value()) {
								 acb(*this->it);
							 }
						 },
						 sound::CONFIRM}
	, it{it}
{
}

glm::vec2 replay_widget::size() const
{
	if (it.has_value()) {
		const score_flags flags{(*it)->second.flags};
		const auto icons{flags.exited_prematurely + flags.modified_game_speed};
		if (icons != 0) {
			return text_button_widget::size() + glm::vec2{0, 20};
		}
	}
	return text_button_widget::size();
}

void replay_widget::add_to_renderer()
{
	if (!it.has_value()) {
		text_widget::add_to_renderer_raw("505050A0"_rgba8);
	}
	else {
		text_button_widget::add_to_renderer();
	}

	if (it.has_value()) {
		const score_flags flags{(*it)->second.flags};
		const glm::vec2 text_size{text_button_widget::size()};
		const tr::rgba8 color{interactible() ? tr::rgba8{m_interp} : "80808080"_rgba8};
		const auto icons{flags.exited_prematurely + flags.modified_game_speed};
		glm::vec2 offset{text_size.x / 2 - 15 * icons, text_size.y};

		if (flags.exited_prematurely) {
			engine::add_exited_prematurely_icon_to_renderer(tl() + offset, color, opacity());
			offset.x += 30;
		}
		if (flags.modified_game_speed) {
			engine::add_modified_game_speed_icon_to_renderer(tl() + offset, color, opacity());
			offset.x += 30;
		}
	}
}