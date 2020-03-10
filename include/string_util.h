
#pragma once
#include <string>
#include <string_view>
#include <optional>
#include <variant>
#include <vector>

namespace spiritsaway::string_util
{
    std::string concate_string(const std::vector<std::string>& str_list);
    std::string concate_string(const std::vector<std::string_view>& str_list);
	std::optional<double> cast_numeric(std::string_view s);
	std::optional<int> cast_int(std::string_view s);
	std::string_view strip_blank(std::string_view input);
	template <typename T>
	std::optional<T> cast_string_view(std::string_view _text)
	{
		return std::nullopt;
	}
	template <>
	std::optional<int> cast_string_view<int>(std::string_view _text);
	template <>
	std::optional<bool> cast_string_view<bool>(std::string_view _text);
	template <>
	std::optional<std::uint32_t> cast_string_view<std::uint32_t>(std::string_view _text);
	template <>
	std::optional<std::int64_t> cast_string_view<std::int64_t>(std::string_view _text);
	template <>
	std::optional<std::uint64_t> cast_string_view<std::uint64_t>(std::string_view _text);
	template <>
	std::optional<float> cast_string_view<float>(std::string_view _text);
	template <>
	std::optional<double> cast_string_view<double>(std::string_view _text);
}
