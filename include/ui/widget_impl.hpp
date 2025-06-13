#pragma once
#include "../engine.hpp"
#include "widget.hpp"

//////////////////////////////////////////////////////////// LINE INPUT WIDGET ////////////////////////////////////////////////////////////

template <size_t S>
line_input_widget<S>::line_input_widget(string_view name, vec2 pos, align alignment, float font_size, status_callback status_cb,
										action_callback enter_cb) noexcept
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  NO_TOOLTIP,
				  true,
				  {},
				  font::LANGUAGE,
				  ttf_style::NORMAL,
				  halign::CENTER,
				  font_size,
				  UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  [this](const static_string<30>&) { return buffer.empty() ? string{localization["empty"]} : string{buffer}; }}
	, _status_cb{std::move(status_cb)}
	, _enter_cb{std::move(enter_cb)}
{
}

template <size_t S> void line_input_widget<S>::add_to_renderer()
{
	const interpolated_rgba8 real_color{color};
	if (!active()) {
		color = {80, 80, 80, 160};
	}
	else if (buffer.empty()) {
		rgba8 real{real_color};
		color = {static_cast<u8>(real.r / 2), static_cast<u8>(real.g / 2), static_cast<u8>(real.b / 2), real.a};
	}
	text_widget::add_to_renderer();
	color = real_color;
}

template <size_t S> bool line_input_widget<S>::active() const noexcept
{
	return _status_cb();
}

template <size_t S> void line_input_widget<S>::on_hover() noexcept
{
	if (!_has_focus) {
		color.change({220, 220, 220, 220}, 0.2_s);
	}
}

template <size_t S> void line_input_widget<S>::on_unhover() noexcept
{
	if (!_has_focus) {
		color.change({160, 160, 160, 160}, 0.2_s);
	}
}

template <size_t S> void line_input_widget<S>::on_hold_begin() noexcept
{
	color = {32, 32, 32, 255};
}

template <size_t S> void line_input_widget<S>::on_hold_transfer_in() noexcept
{
	color = {32, 32, 32, 255};
}

template <size_t S> void line_input_widget<S>::on_hold_transfer_out() noexcept
{
	color = {160, 160, 160, 160};
}

template <size_t S> void line_input_widget<S>::on_hold_end() noexcept
{
	_has_focus = true;
	color = {255, 255, 255, 255};
}

template <size_t S> void line_input_widget<S>::on_gain_focus() noexcept
{
	_has_focus = true;
	color.change({255, 255, 255, 255}, 0.2_s);
}

template <size_t S> void line_input_widget<S>::on_lose_focus() noexcept
{
	_has_focus = false;
	color.change({160, 160, 160, 160}, 0.2_s);
}

template <size_t S> void line_input_widget<S>::on_write(string_view input) noexcept
{
	if (buffer.size() + input.size() <= S) {
		buffer.append(input);
	}
}

template <size_t S> void line_input_widget<S>::on_enter()
{
	_enter_cb();
}

template <size_t S> void line_input_widget<S>::on_erase() noexcept
{
	buffer.pop_back();
}

template <size_t S> void line_input_widget<S>::on_clear() noexcept
{
	buffer.clear();
}

template <size_t S> void line_input_widget<S>::on_copy() noexcept
{
	tr::keyboard::set_clipboard_text(string{buffer}.c_str());
}

template <size_t S> void line_input_widget<S>::on_paste() noexcept
{
	try {
		if (tr::keyboard::clipboard_has_text()) {
			string pasted{tr::keyboard::clipboard_text()};
			std::erase(pasted, '\n');
			buffer += (buffer.size() + pasted.size() > S) ? string_view{pasted}.substr(0, S - buffer.size()) : pasted;
		}
	}
	catch (...) {
	}
}

////////////////////////////////////////////////////////////// REPLAY WIDGET //////////////////////////////////////////////////////////////

replay_widget::replay_widget(string_view name, vec2 pos, align alignment, auto base_status_cb, auto base_action_cb,
							 optional<map<string, replay_header>::iterator> it, key shortcut)
	: clickable_text_widget{name,
							pos,
							alignment,
							font::LANGUAGE,
							40,
							[this](const static_string<30>&) {
								if (!this->it.has_value()) {
									return string{"----------------------------------"};
								}

								replay_header& rpy{(*this->it)->second};
								const ticks result{rpy.result};
								const ticks result_m{result / 60_s};
								const ticks result_s{(result % 60_s) / 1_s};
								const ticks result_ms{(result % 1_s) * 100 / 1_s};
								const string_view gamemode{rpy.gamemode.builtin ? localization[rpy.gamemode.name]
																				: string_view{rpy.gamemode.name}};
								const ch::system_clock::time_point utc_tp{ch::seconds{rpy.timestamp}};
								const auto tp{std::chrono::current_zone()->ch::time_zone::to_local(utc_tp)};
								const ch::hh_mm_ss hhmmss{tp - ch::floor<ch::days>(tp)};
								const ch::year_month_day ymd{ch::floor<ch::days>(tp)};
								const int year{ymd.year()};
								const unsigned int month{ymd.month()};
								const unsigned int day{ymd.day()};
								const auto hour{hhmmss.hours().count()};
								const auto minute{hhmmss.minutes().count()};
								return format("{} ({}{})\n{} | {}:{:02}:{:02} | {}/{:02}/{:02} {:02}:{:02}", rpy.name, localization["by"],
											  rpy.player, gamemode, result_m, result_s, result_ms, year, month, day, hour, minute);
							},
							[=, this] { return base_status_cb() && this->it.has_value(); },
							[=, this] {
								if (this->it.has_value()) {
									base_action_cb(*this->it);
								}
							},
							[this] {
								if (!this->it.has_value()) {
									return string{};
								}
								else {
									const replay_header& header{(*this->it)->second};
									string str{header.description};
									if (header.flags.exited_prematurely || header.flags.modified_game_speed) {
										str.push_back('\n');
									}
									if (header.flags.exited_prematurely) {
										str.push_back('\n');
										str.append(localization["exited_prematurely"]);
									}
									if (header.flags.modified_game_speed) {
										str.push_back('\n');
										str.append(localization["modified_game_speed"]);
									}
									return str;
								}
							},
							{{shortcut}}}
	, it{it}
{
}

////////////////////////////////////////////////////////// MULTILINE TEXT INPUT ///////////////////////////////////////////////////////////

template <size_t S>
multiline_input_widget<S>::multiline_input_widget(string_view name, vec2 pos, align alignment, float width, u8 max_lines,
												  halign text_alignment, float font_size, status_callback status_cb) noexcept
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  NO_TOOLTIP,
				  true,
				  {},
				  font::LANGUAGE,
				  ttf_style::NORMAL,
				  text_alignment,
				  font_size,
				  static_cast<int>(width),
				  {160, 160, 160, 160},
				  [this](const static_string<30>&) { return buffer.empty() ? string{localization["empty"]} : string{buffer}; }}
	, _status_cb{std::move(status_cb)}
	, _size{width, font_manager.font_line_skip(font::LANGUAGE, font_size) * max_lines + 4}
	, _max_lines{max_lines}
	, _has_focus{false}
{
}

template <size_t S> vec2 multiline_input_widget<S>::size() const
{
	return _size;
}

template <size_t S> void multiline_input_widget<S>::add_to_renderer()
{
	tr::rgba color{active() ? rgba8{this->color} : rgba8{80, 80, 80, 160}};
	color.a *= opacity();

	const simple_color_mesh outline{tr::renderer_2d::new_color_outline(layer::UI, 4)};
	fill_rect_outline_vtx(outline.positions, {tl() + 1.0f, size() - 2.0f}, 2.0f);
	rs::fill(outline.colors, color);
	const simple_color_mesh fill{tr::renderer_2d::new_color_fan(layer::UI, 4)};
	fill_rect_vtx(fill.positions, {tl() + 2.0f, size() - 4.0f});
	rs::fill(fill.colors, rgba8{0, 0, 0, static_cast<u8>(160 * opacity())});

	const interpolated_rgba8 real_color{this->color};
	if (!active()) {
		this->color = {80, 80, 80, 160};
	}
	else if (buffer.empty()) {
		rgba8 real{real_color};
		this->color = {static_cast<u8>(real.r / 2), static_cast<u8>(real.g / 2), static_cast<u8>(real.b / 2), real.a};
	}
	text_widget::add_to_renderer();
	this->color = real_color;
}

template <size_t S> bool multiline_input_widget<S>::active() const noexcept
{
	return _status_cb();
}

template <size_t S> void multiline_input_widget<S>::on_hover() noexcept
{
	if (!_has_focus) {
		color.change({220, 220, 220, 220}, 0.2_s);
	}
}

template <size_t S> void multiline_input_widget<S>::on_unhover() noexcept
{
	if (!_has_focus) {
		color.change({160, 160, 160, 160}, 0.2_s);
	}
}

template <size_t S> void multiline_input_widget<S>::on_hold_begin() noexcept
{
	color = {32, 32, 32, 255};
}

template <size_t S> void multiline_input_widget<S>::on_hold_transfer_in() noexcept
{
	color = {32, 32, 32, 255};
}

template <size_t S> void multiline_input_widget<S>::on_hold_transfer_out() noexcept
{
	color = {160, 160, 160, 160};
}

template <size_t S> void multiline_input_widget<S>::on_hold_end() noexcept
{
	_has_focus = true;
	color = {255, 255, 255, 255};
}

template <size_t S> void multiline_input_widget<S>::on_gain_focus() noexcept
{
	_has_focus = true;
	color.change({255, 255, 255, 255}, 0.2_s);
}

template <size_t S> void multiline_input_widget<S>::on_lose_focus() noexcept
{
	_has_focus = false;
	color.change({160, 160, 160, 160}, 0.2_s);
}

template <size_t S> void multiline_input_widget<S>::on_write(string_view input) noexcept
{
	if (buffer.size() + input.size() <= S) {
		buffer.append(input);
		if (font_manager.count_lines(buffer, font::LANGUAGE, ttf_style::NORMAL, _font_size, _font_size / 12, _size.x) > _max_lines) {
			buffer.pop_back();
		}
	}
}

template <size_t S> void multiline_input_widget<S>::on_enter()
{
	if (font_manager.count_lines(buffer, font::LANGUAGE, ttf_style::NORMAL, _font_size, _font_size / 12, _size.x) < _max_lines) {
		buffer.append('\n');
	}
}

template <size_t S> void multiline_input_widget<S>::on_erase() noexcept
{
	buffer.pop_back();
}

template <size_t S> void multiline_input_widget<S>::on_clear() noexcept
{
	buffer.clear();
}

template <size_t S> void multiline_input_widget<S>::on_copy() noexcept
{
	tr::keyboard::set_clipboard_text(string{buffer}.c_str());
}

template <size_t S> void multiline_input_widget<S>::on_paste() noexcept
{
	try {
		if (tr::keyboard::clipboard_has_text()) {
			string pasted{tr::keyboard::clipboard_text()};
			static_string<S> copy{buffer};
			copy += (buffer.size() + pasted.size() > S) ? string_view{pasted}.substr(0, S - buffer.size()) : pasted;
			// Replace this with a smarter solution eventually, maybe.
			if (font_manager.count_lines(copy, font::LANGUAGE, ttf_style::NORMAL, _font_size, _font_size / 12, _size.x) <= _max_lines) {
				buffer = copy;
			}
		}
	}
	catch (...) {
	}
}