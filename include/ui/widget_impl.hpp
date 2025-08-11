#pragma once
#include "../audio.hpp"
#include "../graphics.hpp"
#include "widget.hpp"

//////////////////////////////////////////////////////////// LINE INPUT WIDGET ////////////////////////////////////////////////////////////

template <std::size_t S>
line_input_widget<S>::line_input_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, tr::system::ttf_style style,
										float font_size, status_callback status_cb, action_callback enter_cb, std::string_view initial_text)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  true,
				  NO_TOOLTIP,
				  true,
				  font::LANGUAGE,
				  style,
				  tr::halign::CENTER,
				  font_size,
				  tr::system::UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  [this] { return buffer.empty() ? std::string{engine::loc["empty"]} : std::string{buffer}; }}
	, buffer{initial_text}
	, m_scb{std::move(status_cb)}
	, m_enter_cb{std::move(enter_cb)}
	, m_has_focus{false}
{
}

template <std::size_t S> void line_input_widget<S>::add_to_renderer()
{
	const interpolator<tr::rgba8> real_color{color};
	if (!active()) {
		color = {80, 80, 80, 160};
	}
	else if (buffer.empty()) {
		tr::rgba8 real{real_color};
		color = {static_cast<std::uint8_t>(real.r / 2), static_cast<std::uint8_t>(real.g / 2), static_cast<std::uint8_t>(real.b / 2),
				 real.a};
	}
	text_widget::add_to_renderer();
	color = real_color;
}

template <std::size_t S> bool line_input_widget<S>::active() const
{
	return m_scb();
}

template <std::size_t S> void line_input_widget<S>::on_hover()
{
	if (!m_has_focus) {
		color.change(interp_mode::LERP, {220, 220, 220, 220}, 0.2_s);
		engine::play_sound(sound::HOVER, 0.15f, 0.0f, engine::rng.generate(0.9f, 1.1f));
	}
}

template <std::size_t S> void line_input_widget<S>::on_unhover()
{
	if (!m_has_focus) {
		color.change(interp_mode::LERP, {160, 160, 160, 160}, 0.2_s);
	}
}

template <std::size_t S> void line_input_widget<S>::on_hold_begin()
{
	color = {32, 32, 32, 255};
	engine::play_sound(sound::HOLD, 0.2f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

template <std::size_t S> void line_input_widget<S>::on_hold_transfer_in()
{
	on_hold_begin();
}

template <std::size_t S> void line_input_widget<S>::on_hold_transfer_out()
{
	color = {160, 160, 160, 160};
}

template <std::size_t S> void line_input_widget<S>::on_hold_end()
{
	m_has_focus = true;
	color = "FFFFFF"_rgba8;
}

template <std::size_t S> void line_input_widget<S>::on_gain_focus()
{
	m_has_focus = true;
	color.change(interp_mode::LERP, "FFFFFF"_rgba8, 0.2_s);
	engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

template <std::size_t S> void line_input_widget<S>::on_lose_focus()
{
	m_has_focus = false;
	color.change(interp_mode::LERP, {160, 160, 160, 160}, 0.2_s);
}

template <std::size_t S> void line_input_widget<S>::on_write(std::string_view input)
{
	if (tr::utf8::length(buffer) + tr::utf8::length(input) <= S) {
		buffer.append(input);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <std::size_t S> void line_input_widget<S>::on_enter()
{
	m_enter_cb();
}

template <std::size_t S> void line_input_widget<S>::on_erase()
{
	if (!buffer.empty()) {
		tr::utf8::pop_back(buffer);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <std::size_t S> void line_input_widget<S>::on_clear()
{
	buffer.clear();
	engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
}

template <std::size_t S> void line_input_widget<S>::on_copy()
{
	tr::system::set_clipboard_text(std::string{buffer});
}

template <std::size_t S> void line_input_widget<S>::on_paste()
{
	const std::size_t buffer_length{tr::utf8::length(buffer)};
	if (!tr::system::clipboard_empty() && buffer_length < S) {
		std::string pasted{tr::system::clipboard_text()};
		std::erase(pasted, '\n');
		buffer += (buffer_length + tr::utf8::length(pasted) > S)
					  ? std::string_view{pasted.begin(), tr::utf8::next(pasted.begin(), S - buffer_length)}
					  : pasted;
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

////////////////////////////////////////////////////////////// REPLAY WIDGET //////////////////////////////////////////////////////////////

replay_widget::replay_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, auto base_scb, auto base_acb,
							 std::optional<std::map<std::string, replay_header>::iterator> it)
	: clickable_text_widget{pos,
							alignment,
							unhide_time,
							font::LANGUAGE,
							40,
							[this] {
								if (!this->it.has_value()) {
									return std::string{"----------------------------------"};
								}

								replay_header& rpy{(*this->it)->second};
								const ticks result{rpy.result};
								const ticks result_m{result / 60_s};
								const ticks result_s{(result % 60_s) / 1_s};
								const ticks result_ms{(result % 1_s) * 100 / 1_s};
								const std::chrono::system_clock::time_point utc_tp{std::chrono::seconds{rpy.timestamp}};
								const auto tp{std::chrono::current_zone()->std::chrono::time_zone::to_local(utc_tp)};
								const std::chrono::hh_mm_ss hhmmss{tp - std::chrono::floor<std::chrono::days>(tp)};
								const std::chrono::year_month_day ymd{std::chrono::floor<std::chrono::days>(tp)};
								const int year{ymd.year()};
								const unsigned int month{ymd.month()};
								const unsigned int day{ymd.day()};
								const auto hour{hhmmss.hours().count()};
								const auto minute{hhmmss.minutes().count()};
								return std::format("{} ({}: {})\n{} | {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", rpy.name,
												   engine::loc["by"], rpy.player, ::name(rpy.gamemode), result_m, result_s, result_ms, year,
												   month, day, hour, minute);
							},
							[=, this] { return base_scb() && this->it.has_value(); },
							[=, this] {
								if (this->it.has_value()) {
									base_acb(*this->it);
								}
							},
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
							}}
	, it{it}
{
}

////////////////////////////////////////////////////////// MULTILINE TEXT INPUT ///////////////////////////////////////////////////////////

template <std::size_t S>
multiline_input_widget<S>::multiline_input_widget(interpolator<glm::vec2> pos, tr::align alignment, ticks unhide_time, float width,
												  std::uint8_t max_lines, tr::halign text_alignment, float font_size,
												  status_callback status_cb)
	: text_widget{pos,
				  alignment,
				  unhide_time,
				  true,
				  NO_TOOLTIP,
				  true,
				  font::LANGUAGE,
				  tr::system::ttf_style::NORMAL,
				  text_alignment,
				  font_size,
				  static_cast<int>(width),
				  {160, 160, 160, 160},
				  [this] { return buffer.empty() ? std::string{engine::loc["empty"]} : std::string{buffer}; }}
	, m_scb{std::move(status_cb)}
	, m_size{width, engine::line_skip(font::LANGUAGE, font_size) * max_lines + 4}
	, m_max_lines{max_lines}
	, m_has_focus{false}
{
}

template <std::size_t S> glm::vec2 multiline_input_widget<S>::size() const
{
	return m_size;
}

template <std::size_t S> void multiline_input_widget<S>::add_to_renderer()
{
	tr::rgba color{active() ? tr::rgba8{this->color} : "505050A0"_rgba8};
	color.a *= opacity();

	const tr::gfx::simple_color_mesh_ref outline{tr::gfx::renderer_2d::new_color_outline(layer::UI, 4)};
	tr::fill_rect_outline_vtx(outline.positions, {tl() + 1.0f, size() - 2.0f}, 2.0f);
	std::ranges::fill(outline.colors, color);
	const tr::gfx::simple_color_mesh_ref fill{tr::gfx::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(fill.positions, {tl() + 2.0f, size() - 4.0f});
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, static_cast<std::uint8_t>(160 * opacity())});

	const interpolator<tr::rgba8> real_color{this->color};
	if (!active()) {
		this->color = {80, 80, 80, 160};
	}
	else if (buffer.empty()) {
		tr::rgba8 real{real_color};
		this->color = {static_cast<std::uint8_t>(real.r / 2), static_cast<std::uint8_t>(real.g / 2), static_cast<std::uint8_t>(real.b / 2),
					   real.a};
	}
	text_widget::add_to_renderer();
	this->color = real_color;
}

template <std::size_t S> bool multiline_input_widget<S>::active() const
{
	return m_scb();
}

template <std::size_t S> void multiline_input_widget<S>::on_hover()
{
	if (!m_has_focus) {
		color.change(interp_mode::LERP, {220, 220, 220, 220}, 0.2_s);
		engine::play_sound(sound::HOVER, 0.2f, 0.0f, engine::rng.generate(0.9f, 1.1f));
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_unhover()
{
	if (!m_has_focus) {
		color.change(interp_mode::LERP, {160, 160, 160, 160}, 0.2_s);
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_hold_begin()
{
	color = {32, 32, 32, 255};
	engine::play_sound(sound::HOLD, 0.2f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

template <std::size_t S> void multiline_input_widget<S>::on_hold_transfer_in()
{
	on_hold_begin();
}

template <std::size_t S> void multiline_input_widget<S>::on_hold_transfer_out()
{
	color = {160, 160, 160, 160};
}

template <std::size_t S> void multiline_input_widget<S>::on_hold_end()
{
	m_has_focus = true;
	color = "FFFFFF"_rgba8;
}

template <std::size_t S> void multiline_input_widget<S>::on_gain_focus()
{
	m_has_focus = true;
	color.change(interp_mode::LERP, "FFFFFF"_rgba8, 0.2_s);
	engine::play_sound(sound::CONFIRM, 0.5f, 0.0f, engine::rng.generate(0.9f, 1.1f));
}

template <std::size_t S> void multiline_input_widget<S>::on_lose_focus()
{
	m_has_focus = false;
	color.change(interp_mode::LERP, {160, 160, 160, 160}, 0.2_s);
}

template <std::size_t S> void multiline_input_widget<S>::on_write(std::string_view input)
{
	if (tr::utf8::length(buffer) + tr::utf8::length(input) <= S) {
		buffer.append(input);
		if (engine::count_lines(buffer, font::LANGUAGE, tr::system::ttf_style::NORMAL, m_font_size, m_font_size / 12, m_size.x) >
			m_max_lines) {
			buffer.resize(buffer.size() - input.size());
		}
		else {
			engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
		}
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_enter()
{
	if (tr::utf8::length(buffer) < S &&
		engine::count_lines(buffer, font::LANGUAGE, tr::system::ttf_style::NORMAL, m_font_size, m_font_size / 12, m_size.x) < m_max_lines) {
		buffer.append('\n');
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_erase()
{
	if (!buffer.empty()) {
		tr::utf8::pop_back(buffer);
		engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_clear()
{
	buffer.clear();
	engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
}

template <std::size_t S> void multiline_input_widget<S>::on_copy()
{
	tr::system::set_clipboard_text(std::string{buffer}.c_str());
}

template <std::size_t S> void multiline_input_widget<S>::on_paste()
{
	try {
		if (!tr::system::clipboard_empty()) {
			std::string pasted{tr::system::clipboard_text()};
			tr::static_string<S * 4> copy{buffer};
			const std::size_t buffer_length{tr::utf8::length(buffer)};
			copy += (buffer_length + tr::utf8::length(pasted) > S)
						? std::string_view{pasted.begin(), tr::utf8::next(pasted.begin(), S - buffer_length)}
						: pasted;
			// Replace this with a smarter solution eventually, maybe.
			if (engine::count_lines(copy, font::LANGUAGE, tr::system::ttf_style::NORMAL, m_font_size, m_font_size / 12, m_size.x) <=
				m_max_lines) {
				buffer = copy;
			}
			engine::play_sound(sound::TYPE, 0.2f, 0.0f, engine::rng.generate(0.75f, 1.25f));
		}
	}
	catch (...) {
	}
}