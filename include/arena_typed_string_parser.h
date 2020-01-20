#pragma once
#include "arena_typed_string.h"

namespace spiritsaway::container
{
    class arena_typed_string_parser
	{
	private:
		spiritsaway::memory::arena& arena;
	public:
		const typed_value_desc* type_desc;
		arena_typed_string_parser(spiritsaway::memory::arena& arena);
		const arena_typed_value* match_node(std::string_view text);
		bool match_node(std::string_view text, arena_typed_value& result);

		typed_value_desc* parse_type(std::string_view type_string);

		arena_typed_value* parse_value_with_type(const typed_value_desc* node_type, std::string_view text);
		bool parse_value_with_type(const typed_value_desc* type_desc, std::string_view text, arena_typed_value& result);

	private:
		bool parse_value_with_address(const typed_value_desc* typed_desc, std::string_view text, arena_typed_value* result);
	};
}