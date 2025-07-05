#pragma once
#include "../audio.hpp"
#include "../engine.hpp"
#include "widget.hpp"

//////////////////////////////////////////////////////////// LINE INPUT WIDGET ////////////////////////////////////////////////////////////

template <std::size_t S>
line_input_widget<S>::line_input_widget(std::string_view name, glm::vec2 pos, tr::align alignment, tr::ttf_style style, float font_size,
										status_callback status_cb, action_callback enter_cb) noexcept
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  NO_TOOLTIP,
				  true,
				  {},
				  font::LANGUAGE,
				  style,
				  tr::halign::CENTER,
				  font_size,
				  tr::UNLIMITED_WIDTH,
				  {160, 160, 160, 160},
				  [this](auto&) { return buffer.empty() ? std::string{localization["empty"]} : std::string{buffer}; }}
	, _status_cb{std::move(status_cb)}
	, _enter_cb{std::move(enter_cb)}
	, _has_focus{false}
{
}

template <std::size_t S> void line_input_widget<S>::add_to_renderer()
{
	const interpolated_rgba8 real_color{color};
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

template <std::size_t S> bool line_input_widget<S>::active() const noexcept
{
	return _status_cb();
}

template <std::size_t S> void line_input_widget<S>::on_hover() noexcept
{
	if (!_has_focus) {
		color.change({220, 220, 220, 220}, 0.2_s);
		audio::play(sfx::HOVER, 0.15f, 0.0f, tr::rand(rng, 0.9f, 1.1f));
	}
}

template <std::size_t S> void line_input_widget<S>::on_unhover() noexcept
{
	if (!_has_focus) {
		color.change({160, 160, 160, 160}, 0.2_s);
	}
}

template <std::size_t S> void line_input_widget<S>::on_hold_begin() noexcept
{
	color = {32, 32, 32, 255};
	audio::play(sfx::HOLD, 0.2f, 0.0f, tr::rand(rng, 0.9f, 1.1f));
}

template <std::size_t S> void line_input_widget<S>::on_hold_transfer_in() noexcept
{
	on_hold_begin();
}

template <std::size_t S> void line_input_widget<S>::on_hold_transfer_out() noexcept
{
	color = {160, 160, 160, 160};
}

template <std::size_t S> void line_input_widget<S>::on_hold_end() noexcept
{
	_has_focus = true;
	color = "FFFFFF"_rgba8;
}

template <std::size_t S> void line_input_widget<S>::on_gain_focus() noexcept
{
	_has_focus = true;
	color.change("FFFFFF"_rgba8, 0.2_s);
	audio::play(sfx::CONFIRM, 0.5f, 0.0f, tr::rand(rng, 0.9f, 1.1f));
}

template <std::size_t S> void line_input_widget<S>::on_lose_focus() noexcept
{
	_has_focus = false;
	color.change({160, 160, 160, 160}, 0.2_s);
}

template <std::size_t S> void line_input_widget<S>::on_write(std::string_view input) noexcept
{
	if (buffer.size() + input.size() <= S) {
		buffer.append(input);
		audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
	}
}

template <std::size_t S> void line_input_widget<S>::on_enter()
{
	_enter_cb();
}

template <std::size_t S> void line_input_widget<S>::on_erase() noexcept
{
	if (!buffer.empty()) {
		buffer.pop_back();
		audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
	}
}

template <std::size_t S> void line_input_widget<S>::on_clear() noexcept
{
	buffer.clear();
	audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
}

template <std::size_t S> void line_input_widget<S>::on_copy() noexcept
{
	tr::keyboard::set_clipboard_text(std::string{buffer}.c_str());
}

template <std::size_t S> void line_input_widget<S>::on_paste() noexcept
{
	try {
		if (tr::keyboard::clipboard_has_text()) {
			std::string pasted{tr::keyboard::clipboard_text()};
			std::erase(pasted, '\n');
			buffer += (buffer.size() + pasted.size() > S) ? std::string_view{pasted}.substr(0, S - buffer.size()) : pasted;
			audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
		}
	}
	catch (...) {
	}
}

////////////////////////////////////////////////////////////// REPLAY WIDGET //////////////////////////////////////////////////////////////

replay_widget::replay_widget(std::string_view name, glm::vec2 pos, tr::align alignment, auto base_status_cb, auto base_action_cb,
							 std::optional<std::map<std::string, replay_header>::iterator> it, tr::keycode shortcut)
	: clickable_text_widget{name,
							pos,
							alignment,
							font::LANGUAGE,
							40,
							[this](auto&) {
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
												   localization["by"], rpy.player, rpy.gamemode.name_loc(), result_m, result_s, result_ms,
												   year, month, day, hour, minute);
							},
							[=, this] { return base_status_cb() && this->it.has_value(); },
							[=, this] {
								if (this->it.has_value()) {
									base_action_cb(*this->it);
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
										str.append(localization["exited_prematurely"]);
									}
									if (header.flags.modified_game_speed) {
										if (!str.empty()) {
											str.push_back('\n');
										}
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

template <std::size_t S>
multiline_input_widget<S>::multiline_input_widget(std::string_view name, glm::vec2 pos, tr::align alignment, float width,
												  std::uint8_t max_lines, tr::halign text_alignment, float font_size,
												  status_callback status_cb) noexcept
	: text_widget{name,
				  pos,
				  alignment,
				  true,
				  NO_TOOLTIP,
				  true,
				  {},
				  font::LANGUAGE,
				  tr::ttf_style::NORMAL,
				  text_alignment,
				  font_size,
				  static_cast<int>(width),
				  {160, 160, 160, 160},
				  [this](auto&) { return buffer.empty() ? std::string{localization["empty"]} : std::string{buffer}; }}
	, _status_cb{std::move(status_cb)}
	, _size{width, font_manager.font_line_skip(font::LANGUAGE, font_size) * max_lines + 4}
	, _max_lines{max_lines}
	, _has_focus{false}
{
}

template <std::size_t S> glm::vec2 multiline_input_widget<S>::size() const
{
	return _size;
}

template <std::size_t S> void multiline_input_widget<S>::add_to_renderer()
{
	tr::rgba color{active() ? tr::rgba8{this->color} : "505050A0"_rgba8};
	color.a *= opacity();

	const tr::simple_color_mesh_ref outline{tr::renderer_2d::new_color_outline(layer::UI, 4)};
	tr::fill_rect_outline_vtx(outline.positions, {tl() + 1.0f, size() - 2.0f}, 2.0f);
	std::ranges::fill(outline.colors, color);
	const tr::simple_color_mesh_ref fill{tr::renderer_2d::new_color_fan(layer::UI, 4)};
	tr::fill_rect_vtx(fill.positions, {tl() + 2.0f, size() - 4.0f});
	std::ranges::fill(fill.colors, tr::rgba8{0, 0, 0, static_cast<std::uint8_t>(160 * opacity())});

	const interpolated_rgba8 real_color{this->color};
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

template <std::size_t S> bool multiline_input_widget<S>::active() const noexcept
{
	return _status_cb();
}

template <std::size_t S> void multiline_input_widget<S>::on_hover() noexcept
{
	if (!_has_focus) {
		color.change({220, 220, 220, 220}, 0.2_s);
		audio::play(sfx::HOVER, 0.5f, 0.0f, tr::rand(rng, 0.9f, 1.1f));
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_unhover() noexcept
{
	if (!_has_focus) {
		color.change({160, 160, 160, 160}, 0.2_s);
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_hold_begin() noexcept
{
	color = {32, 32, 32, 255};
	audio::play(sfx::HOLD, 0.2f, 0.0f, tr::rand(rng, 0.9f, 1.1f));
}

template <std::size_t S> void multiline_input_widget<S>::on_hold_transfer_in() noexcept
{
	on_hold_begin();
}

template <std::size_t S> void multiline_input_widget<S>::on_hold_transfer_out() noexcept
{
	color = {160, 160, 160, 160};
}

template <std::size_t S> void multiline_input_widget<S>::on_hold_end() noexcept
{
	_has_focus = true;
	color = "FFFFFF"_rgba8;
	audio::play(sfx::CONFIRM, 0.5f, 0.0f, tr::rand(rng, 0.9f, 1.1f));
}

template <std::size_t S> void multiline_input_widget<S>::on_gain_focus() noexcept
{
	_has_focus = true;
	color.change("FFFFFF"_rgba8, 0.2_s);
}

template <std::size_t S> void multiline_input_widget<S>::on_lose_focus() noexcept
{
	_has_focus = false;
	color.change({160, 160, 160, 160}, 0.2_s);
}

template <std::size_t S> void multiline_input_widget<S>::on_write(std::string_view input) noexcept
{
	if (buffer.size() + input.size() <= S) {
		buffer.append(input);
		if (font_manager.count_lines(buffer, font::LANGUAGE, tr::ttf_style::NORMAL, _font_size, _font_size / 12, _size.x) > _max_lines) {
			buffer.pop_back();
			audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
		}
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_enter()
{
	if (font_manager.count_lines(buffer, font::LANGUAGE, tr::ttf_style::NORMAL, _font_size, _font_size / 12, _size.x) < _max_lines) {
		buffer.append('\n');
		audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_erase() noexcept
{
	if (!buffer.empty()) {
		buffer.pop_back();
		audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
	}
}

template <std::size_t S> void multiline_input_widget<S>::on_clear() noexcept
{
	buffer.clear();
	audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
}

template <std::size_t S> void multiline_input_widget<S>::on_copy() noexcept
{
	tr::keyboard::set_clipboard_text(std::string{buffer}.c_str());
}

template <std::size_t S> void multiline_input_widget<S>::on_paste() noexcept
{
	try {
		if (tr::keyboard::clipboard_has_text()) {
			std::string pasted{tr::keyboard::clipboard_text()};
			tr::static_string<S> copy{buffer};
			copy += (buffer.size() + pasted.size() > S) ? std::string_view{pasted}.substr(0, S - buffer.size()) : pasted;
			// Replace this with a smarter solution eventually, maybe.
			if (font_manager.count_lines(copy, font::LANGUAGE, tr::ttf_style::NORMAL, _font_size, _font_size / 12, _size.x) <= _max_lines) {
				buffer = copy;
			}
			audio::play(sfx::TYPE, 0.2f, 0.0f, tr::rand(rng, 0.75f, 1.25f));
		}
	}
	catch (...) {
	}
}