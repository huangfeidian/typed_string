#include <arena_typed_string_parser.h>
#include <string_util.h>

namespace
{
	using namespace std;
	string_view strip_parenthesis(string_view input_string)
	{
		// input (xxx) return xxx
		// if not(xxx) return empty
		auto left_idx = input_string.find_first_of("(");
		if (left_idx == string_view::npos)
		{
			return {};
		}
		input_string.remove_prefix(left_idx + 1);
		auto right_idx = input_string.find_last_of(")");
		if (right_idx == string_view::npos)
		{
			return {};
		}

		input_string.remove_suffix(input_string.size() - right_idx);
		return input_string;
	}

	vector<string_view> split_string(string_view input_string, char sep)
	{
		vector<string_view> tokens;
		size_t start = 0;
		size_t end = 0;

		while ((end = input_string.find(sep, start)) != string_view::npos)
		{
			if (end != start)
			{
				tokens.emplace_back(input_string.substr(start, end - start));
			}
			else
			{
				return {};
			}
			start = end + 1;
		}
		if (start < input_string.size())
		{
			tokens.emplace_back(input_string.substr(start));
		}
		return tokens;
		
	}

}

namespace spiritsaway::container
{
    using namespace spiritsaway::string_util;

    arena_typed_string_parser::arena_typed_string_parser(spiritsaway::memory::arena& in_arena)
		:arena(in_arena)
	{

	}
	arena_typed_value* arena_typed_string_parser::parse_value_with_type(const typed_string_desc* node_type, string_view text)
	{
		arena_typed_value* temp_result = arena.get<arena_typed_value>(1);
		if (parse_value_with_address(node_type, text, temp_result))
		{
			return temp_result;
		}
		else
		{
			return nullptr;
		}
	}
	bool arena_typed_string_parser::parse_value_with_type(const typed_string_desc* node_type, string_view text, arena_typed_value& result)
	{
		return parse_value_with_address(node_type, text, &result);
	}

	bool arena_typed_string_parser::parse_value_with_address(const typed_string_desc* node_type, string_view text, arena_typed_value* result)
	{
		text.remove_prefix(min(text.find_first_not_of(" "), text.size()));
		text.remove_suffix(text.size() - min(text.find_last_not_of(" ") + 1, text.size()));
		if (text.size() == 0)
		{
			return false;
		}
		
		switch (node_type->_type)
		{
		case basic_value_type::comment:
			return false;
		case basic_value_type::string:
			new(result) arena_typed_value(&arena, text);
			return true;
		case basic_value_type::number_bool:
			if (text == "1"sv)
			{
				new(result) arena_typed_value(&arena, true);
				return true;
			}
			else if (text == "0"sv)
			{
				new(result) arena_typed_value(&arena, false);
				return true;
			}
			else
			{
				return false;
			}
		case basic_value_type::number_32:
		{
			auto cur_numer_value = string_util::cast_string_view<int>(text);
			if (cur_numer_value.has_value())
			{
				new(result) arena_typed_value(&arena, cur_numer_value.value());
				return true;
			}
			else
			{
				return false;
			}
		}
			
		case basic_value_type::number_u32:
		{
			auto cur_numer_value = string_util::cast_string_view<uint32_t>(text);
			if (cur_numer_value.has_value())
			{
				new(result) arena_typed_value(&arena, cur_numer_value.value());
				return true;
			}
			else
			{
				return false;
			}
		}
		case basic_value_type::number_64:
		{
			auto cur_numer_value = string_util::cast_string_view<int64_t>(text);
			if (cur_numer_value.has_value())
			{
				new(result) arena_typed_value(&arena, cur_numer_value.value());
				return true;
			}
			else
			{
				return false;
			}
		}
		case basic_value_type::number_u64:
		{
			auto cur_numer_value = string_util::cast_string_view<uint64_t>(text);
			if (cur_numer_value.has_value())
			{
				new(result) arena_typed_value(&arena, cur_numer_value.value());
				return true;
			}
			else
			{
				return false;
			}
		}
		case basic_value_type::number_float:
		{
			auto cur_numer_value = string_util::cast_string_view<float>(text);
			if (cur_numer_value.has_value())
			{
				new(result) arena_typed_value(&arena, cur_numer_value.value());
				return true;
			}
			else
			{
				return false;
			}
		}
		case basic_value_type::number_double:
		{
			auto cur_numer_value = string_util::cast_string_view<double>(text);
			if (cur_numer_value.has_value())
			{
				new(result) arena_typed_value(&arena, cur_numer_value.value());
				return true;
			}
			else
			{
				return false;
			}
		}
		case basic_value_type::choice_int:
		{
			auto cur_choice_detail = std::get<typed_string_desc::choice_int_detail_t>(node_type->_type_detail);
			auto[vec_p, count] = cur_choice_detail;

			auto opt_value = cast_string_view<std::int32_t>(text);
			if (!opt_value)
			{
				return false;
			}
			auto temp_value = opt_value.value();
			for (std::size_t i = 0; i < count; i++)
			{
				if (vec_p[i] == temp_value)
				{
					new(result) arena_typed_value(&arena, node_type, temp_value);
					return true;
				}
			}
			return false;
			
		}
		case basic_value_type::choice_str:
		{
			auto cur_choice_detail = std::get<typed_string_desc::choice_str_detail_t>(node_type->_type_detail);
			auto[vec_p, count] = cur_choice_detail;
			auto temp_value = strip_blank(text);
			for (std::size_t i = 0; i < count; i++)
			{
				if (vec_p[i] == temp_value)
				{
					new(result) arena_typed_value(&arena, node_type, temp_value);
					return true;
				}
			}
			return false;
		}
		case basic_value_type::tuple:
		{
			auto cur_tuple_detail = std::get<typed_string_desc::tuple_detail_t>(node_type->_type_detail);
			auto[type_vec_p, count, sep] = cur_tuple_detail;

			text = strip_parenthesis(text);
			auto tokens = split_string(text, sep);
			if (tokens.size() != count)
			{
				return false;
			}
			vector<arena_typed_value *> sub_values;
			for (std::uint32_t i = 0; i < count; i++)
			{
				arena_typed_value* temp_result = arena.get<arena_typed_value>(1);
				temp_result->~arena_typed_value();
				auto cur_value = parse_value_with_type(type_vec_p[i], tokens[i], *temp_result);
				if (!cur_value)
				{
					return false;
				}
				sub_values.push_back(temp_result);
			}
			auto temp_result_p = arena.get<arena_typed_value*>(sub_values.size());
			for (std::uint32_t i = 0; i < sub_values.size(); i++)
			{
				temp_result_p[i] = sub_values[i];
			}
			arena_typed_vec cur_arena_vec;
			cur_arena_vec.p_value = temp_result_p;
			cur_arena_vec.size = sub_values.size();
			new(result) arena_typed_value(&arena, node_type, cur_arena_vec);
			return true;
		}
		case basic_value_type::list:
		{
			auto cur_list_detail = std::get<typed_string_desc::list_detail_t>(node_type->_type_detail);
			uint32_t list_size = std::get<uint32_t>(cur_list_detail);
			char sep = std::get<char>(cur_list_detail);
			auto unit_type = std::get<typed_string_desc *>(cur_list_detail);
			text = strip_parenthesis(text);
			auto tokens = split_string(text, sep);
			vector<arena_typed_value *> sub_values;
			if (list_size == 0)
			{
				for (auto one_token : tokens)
				{
					arena_typed_value* temp_result = arena.get<arena_typed_value>(1);
					temp_result->~arena_typed_value();
					auto cur_value = parse_value_with_type(unit_type, one_token, *temp_result);
					if (!cur_value)
					{
						return false;
					}
					sub_values.push_back(temp_result);
				}
			}
			else
			{
				if (tokens.size() != list_size)
				{
					return nullptr;
				}
				sub_values.reserve(list_size);
				for (auto one_token : tokens)
				{
					arena_typed_value* temp_result = arena.get<arena_typed_value>(1);
					temp_result->~arena_typed_value();
					auto cur_value = parse_value_with_type(unit_type, one_token, *temp_result);
					if (!cur_value)
					{
						return false;
					}
					sub_values.push_back(temp_result);
				}
			}
			auto temp_result_p = arena.get<arena_typed_value*>(sub_values.size());
			for (std::uint32_t i = 0; i < sub_values.size(); i++)
			{
				temp_result_p[i] = sub_values[i];
			}
			arena_typed_vec cur_arena_vec;
			cur_arena_vec.p_value = temp_result_p;
			cur_arena_vec.size = sub_values.size();
			new(result) arena_typed_value(&arena, node_type, cur_arena_vec);
			return true;
		}
		default:
			return false;
		}
	}
	const arena_typed_value* arena_typed_string_parser::match_node(string_view text)
	{
		return arena_typed_string_parser::parse_value_with_type(type_desc, text);
	}
	bool arena_typed_string_parser::match_node(string_view text, arena_typed_value& result)
	{
		return arena_typed_string_parser::parse_value_with_type(type_desc, text, result);
	}
}