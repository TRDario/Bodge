#include "../include/legacy_formats.hpp"

std::span<const std::byte> tr::binary_reader<gamemode_v0>::read_from_span(std::span<const std::byte> span, gamemode_v0& out)
{
	span = tr::binary_read(span, out.builtin);
	span = tr::binary_read(span, out.name);
	span = tr::binary_read(span, out.author);
	span = tr::binary_read(span, out.description);
	span = tr::binary_read(span, out.song);
	span = tr::binary_read(span, out.player);
	return tr::binary_read(span, out.ball);
}

std::span<const std::byte> tr::binary_reader<score_v0>::read_from_span(std::span<const std::byte> span, score_v0& out)
{
	span = tr::binary_read(span, out.description);
	span = tr::binary_read(span, out.unix_timestamp);
	span = tr::binary_read(span, out.result);
	return tr::binary_read(span, out.flags);
}

std::span<const std::byte> tr::binary_reader<score_category_v0>::read_from_span(std::span<const std::byte> span, score_category_v0& out)
{
	span = tr::binary_read(span, out.gamemode);
	span = tr::binary_read(span, out.personal_best);
	return tr::binary_read(span, out.scores);
}