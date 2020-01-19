#pragma once
#include <unordered_map>
#include <algorithm>

#include <numeric>
#include <functional>

#include <arena_typed_string.h>

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
	using namespace std;
	using namespace spiritsaway::string_util;
	ostream& operator<<(ostream& output_stream, const arena_typed_value& cur_value)
	{
		switch(cur_value.type_desc->_type)
		{
			case basic_value_type::comment:
				return output_stream<<cur_value.v_text;
			case basic_value_type::number_bool:
				return output_stream<<(cur_value.v_bool?"1":"0");
			case basic_value_type::number_32:
				return output_stream<<cur_value.v_int32;
			case basic_value_type::number_u32:
				return output_stream<<cur_value.v_uint32;
			case basic_value_type::number_64:
				return output_stream<<cur_value.v_int64;
			case basic_value_type::number_u64:
				return output_stream<<cur_value.v_uint64;
			case basic_value_type::number_float:
				return output_stream<<cur_value.v_float;
			case basic_value_type::number_double:
				return output_stream<<cur_value.v_double;
			case basic_value_type::ref_id:
				return output_stream<<cur_value.v_text;
			case basic_value_type::string:
				return output_stream<<cur_value.v_text;
			case basic_value_type::list:
			{
				auto cur_list_detail = std::get<typed_value_desc::list_detail_t>(cur_value.type_desc->_type_detail);
				char sep = std::get<2>(cur_list_detail);
				output_stream<<"(";
				int cur_size = cur_value.v_vec.size;
				for(int i = 0;i<cur_size; i++)
				{
					output_stream<<*(cur_value.v_vec.p_value[i]);
					if(i != cur_size -1)
					{
						output_stream << sep ;
					}
				}
				return output_stream<<")";
			}
			case basic_value_type::tuple:
			{
				auto cur_tuple_detail = std::get<typed_value_desc::tuple_detail_t>(cur_value.type_desc->_type_detail);
				char sep = cur_tuple_detail.second;
				output_stream<<"(";
				int cur_size = cur_value.v_vec.size;
				for(int i = 0;i<cur_size; i++)
				{
					output_stream<<*cur_value.v_vec.p_value[i];
					if(i != cur_size -1)
					{
						output_stream<<sep;
					}
				}
				return output_stream<<")";
			}
			default:
				return output_stream;

		}
	}
	
	

	arena_typed_value::arena_typed_value()
		:type_desc(nullptr)
	{

	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, bool in_value)
		:type_desc(typed_value_desc::get_basic_type_desc(basic_value_type::number_bool))
		, arena(arena)
	{
		v_bool = in_value;
	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, uint32_t in_value) 
		: type_desc(typed_value_desc::get_basic_type_desc(basic_value_type::number_u32))
		, arena(arena)
	{
		v_uint32 = in_value;
	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, int32_t in_value)
		: type_desc(typed_value_desc::get_basic_type_desc(basic_value_type::number_32))
		, arena(arena)
	{
		v_int32 = in_value;
	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, uint64_t in_value)
		: type_desc(typed_value_desc::get_basic_type_desc(basic_value_type::number_u64))
		, arena(arena)
	{
		v_uint64 = in_value;
	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, int64_t in_value)
		: type_desc(typed_value_desc::get_basic_type_desc(basic_value_type::number_64))
		, arena(arena)
	{
		v_int64 = in_value;
	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, float in_value)
		: type_desc(typed_value_desc::get_basic_type_desc(basic_value_type::number_float))
		, arena(arena)
	{
		v_float = in_value;
	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, double in_value)
		: type_desc(typed_value_desc::get_basic_type_desc(basic_value_type::number_double))
		, arena(arena)
	{
		v_double = in_value;
	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, string_view in_value)
		: type_desc(typed_value_desc::get_basic_type_desc(basic_value_type::string))
		, arena(arena)
	{
		v_text = in_value;
	}
	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, const typed_value_desc* in_type_desc, string_view in_value)
		: type_desc(in_type_desc)
		, arena(arena)
		, v_text(in_value)
	{
	}

	arena_typed_value::arena_typed_value(const spiritsaway::memory::arena* arena, const typed_value_desc* in_type_desc, arena_typed_vec in_value)
		: type_desc(in_type_desc)
		, arena(arena)
		, v_vec(in_value)
	{
	}


	bool operator==(const arena_typed_value& cur, const arena_typed_value& other)
	{
		if(!cur.type_desc || ! other.type_desc)
		{
			return false;
		}
		if(*cur.type_desc!=(*other.type_desc))
		{
			// maybe one in ref 
			if (cur.type_desc->_type == basic_value_type::ref_id || other.type_desc->_type == basic_value_type::ref_id)
			{
				if (cur.type_desc->_type == basic_value_type::ref_id && other.type_desc->_type == basic_value_type::ref_id)
				{
					return false;
				}
				else
				{
					if (!cur.v_text.empty() || !other.v_text.empty())
					{
						if (cur.v_text != other.v_text)
						{
							return false;
						}
						else
						{
							return true;
						}
					}
					else
					{
						return cur.v_int32 == other.v_int32;
					}

				}
			}
			else
			{
				return false;
			}
		}
		switch(cur.type_desc->_type)
		{
		case basic_value_type::comment:
		case basic_value_type::string:
			return cur.v_text == other.v_text;
		case basic_value_type::number_double:
			return cur.v_double == other.v_double;
		case basic_value_type::number_32:
			return cur.v_int32 == other.v_int32;
		case basic_value_type::number_u32:
			return cur.v_uint32 == other.v_uint32;
		case basic_value_type::number_64:
			return cur.v_int64 == other.v_int64;
		case basic_value_type::number_u64:
			return cur.v_uint64 == other.v_uint64;
		case basic_value_type::number_bool:
			return cur.v_bool == other.v_bool;
		case basic_value_type::number_float:
			return cur.v_float == other.v_float;
		case basic_value_type::list:
		case basic_value_type::tuple:
		{
			if (cur.v_vec.size != other.v_vec.size)
			{
				return false;
			}
			auto cur_size = cur.v_vec.size;
			for (std::uint32_t  i = 0; i < cur_size; i++)
			{
				if (*cur.v_vec.p_value[i] == *other.v_vec.p_value[i])
				{
					continue;
				}
				else
				{
					return false;
				}
			}
			return true;
		}
		case basic_value_type::ref_id:
		{
			auto cur_ref_detail_opt = cur.type_desc->get_ref_detail_t();
			if (!cur_ref_detail_opt)
			{
				return 0;
			}
			auto cur_ref_detail = cur_ref_detail_opt.value();
			if (cur_ref_detail.second == "str")
			{
				return cur.v_text == other.v_text;
			}
			else
			{
				return cur.v_int32 == other.v_int32;
			}
		}
			
		default:
			return false;
		}
	}
	bool operator!=(const arena_typed_value& cur, const arena_typed_value& other)
	{
		return !(cur == other);
	}

	size_t arena_typed_value_hash::operator()(const arena_typed_value* s) const
	{
		if (!s)
		{
			return 0;
		}
		switch(s->type_desc->_type)
		{
		case basic_value_type::comment:
		case basic_value_type::string:
			return std::hash<std::string_view>()(s->v_text);
		case basic_value_type::number_double:
			return std::hash<double>()(s->v_double);
		case basic_value_type::number_32:
			return std::hash<std::int32_t>()(s->v_int32);
		case basic_value_type::number_u32:
			return std::hash<std::uint32_t>()(s->v_uint32);
		case basic_value_type::number_64:
			return std::hash<std::int64_t>()(s->v_int64);
		case basic_value_type::number_u64:
			return std::hash<std::uint64_t>()(s->v_uint64);
		case basic_value_type::number_bool:
			return std::hash<bool>()(s->v_bool);
		case basic_value_type::number_float:
			return std::hash<float>()(s->v_float);
		case basic_value_type::list:
		case basic_value_type::tuple:
		{
			auto cur_size = s->v_vec.size;
			std::size_t result_hash = 0;
			for(std::uint32_t i = 0; i < s->v_vec.size; i++)
			{
				result_hash += operator()(s->v_vec.p_value[i]) / cur_size;
			}
			return result_hash;
		}
		case basic_value_type::ref_id:
		{
			auto cur_ref_detail_opt = s->type_desc->get_ref_detail_t();
			if (!cur_ref_detail_opt)
			{
				return 0;
			}
			auto cur_ref_detail = cur_ref_detail_opt.value();
			if (cur_ref_detail.second == "str")
			{
				return std::hash<std::string_view>()(s->v_text);
			}
			else
			{
				return std::hash<std::int32_t>()(s->v_int32);
			}
		}
			
		default:
			return 0;
		}
	}
	bool arena_typed_value_ptr_equal::operator()(const arena_typed_value* from, const arena_typed_value* to) const
	{
		if(from == to)
		{
			return true;
		}
		if(!from ||!to)
		{
			return false;
		}
		return (*from == *to);
	}

	
	template <>
	optional<std::uint32_t> arena_typed_value::expect_simple_value<uint32_t>() const
	{
		auto cur_type = type_desc->_type;
		if(cur_type == basic_value_type::number_u32)
		{
			return v_uint32;
		}
		else if(cur_type == basic_value_type::number_32)
		{
			if (v_int32 < 0)
			{
				return nullopt;
			}
			return static_cast<std::uint32_t>(v_int32);
		}
		else
		{
			return nullopt;
		}
	}

	template <>
	optional<std::int32_t> arena_typed_value::expect_simple_value<int32_t>() const
	{
		auto cur_type = type_desc->_type;
		if(cur_type == basic_value_type::number_32)
		{
			return v_int32;
		}
		else if(cur_type == basic_value_type::number_u32)
		{
			if (v_uint32 > numeric_limits<uint32_t>::max() / 2)
			{
				return nullopt;
			}
			else
			{
				return static_cast<int>(v_uint32);
			}
			
		}
		else
		{
			return nullopt;
		}
	}

	template <>
	optional<std::int64_t> arena_typed_value::expect_simple_value<int64_t>() const
	{
		if(type_desc->_type != basic_value_type::number_64)
		{
			return nullopt;
		}
		else
		{
			return v_int64;
		}
	}
	template <>
	optional<std::uint64_t> arena_typed_value::expect_simple_value<uint64_t>() const
	{
		if(type_desc->_type != basic_value_type::number_u64)
		{
			return nullopt;
		}
		else
		{
			return v_uint64;
		}
	}
	template <>
	optional<bool> arena_typed_value::expect_simple_value<bool>() const
	{
		if(type_desc->_type != basic_value_type::number_bool)
		{
			return nullopt;
		}
		else
		{
			return v_bool;
		}
	}

	template <>
	optional<float> arena_typed_value::expect_simple_value<float>() const
	{
		if(type_desc->_type != basic_value_type::number_float)
		{
			return nullopt;
		}
		else
		{
			return v_float;
		}
	}
	template <>
	optional<double> arena_typed_value::expect_simple_value<double>() const
	{
		if(type_desc->_type != basic_value_type::number_double)
		{
			return nullopt;
		}
		else
		{
			return v_double;
		}
	}
	template <>
	optional<string_view> arena_typed_value::expect_simple_value<string_view>() const
	{

		if(v_text.empty())
		{
			return nullopt;
		}
		else
		{
			return v_text;
		}
	}

	arena_typed_value_parser::arena_typed_value_parser(spiritsaway::memory::arena& in_arena)
		:arena(in_arena)
	{

	}
	arena_typed_value* arena_typed_value_parser::parse_value_with_type(const typed_value_desc* node_type, string_view text)
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
	bool arena_typed_value_parser::parse_value_with_type(const typed_value_desc* node_type, string_view text, arena_typed_value& result)
	{
		return parse_value_with_address(node_type, text, &result);
	}

	bool arena_typed_value_parser::parse_value_with_address(const typed_value_desc* node_type, string_view text, arena_typed_value* result)
	{
		text.remove_prefix(min(text.find_first_not_of(" "), text.size()));
		text.remove_suffix(text.size() - min(text.find_last_not_of(" ") + 1, text.size()));
		if (text.size() == 0)
		{
			return false;
		}
		auto current_double_value = cast_string_view<double>(text);
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
			if (current_double_value.has_value())
			{
				new(result) arena_typed_value(&arena, static_cast<int32_t>(current_double_value.value()));
				return true;
			}
			else
			{
				return false;
			}
		case basic_value_type::number_u32:
			if (current_double_value.has_value())
			{
				new(result) arena_typed_value(&arena, static_cast<uint32_t>(current_double_value.value()));
				return true;
			}
			else
			{
				return false;
			}
		case basic_value_type::number_64:
			if (current_double_value.has_value())
			{
				new(result) arena_typed_value(&arena, static_cast<int64_t>(current_double_value.value()));
				return true;
			}
			else
			{
				return false;
			}
		case basic_value_type::number_u64:
			if (current_double_value.has_value())
			{
				return new(result) arena_typed_value(&arena, static_cast<uint64_t>(current_double_value.value()));
			}
			else
			{
				return false;
			}
		case basic_value_type::number_float:
			if (current_double_value.has_value())
			{
				new(result) arena_typed_value(&arena, static_cast<float>(current_double_value.value()));
				return true;
			}
			else
			{
				return false;
			}
		case basic_value_type::number_double:
			if (current_double_value.has_value())
			{
				new(result) arena_typed_value(&arena, current_double_value.value());
				return true;
			}

			else
			{
				return false;
			}
		case basic_value_type::ref_id:
		{
			auto cur_ref_detail = std::get<typed_value_desc::ref_detail_t>(node_type->_type_detail);
			if (cur_ref_detail.second == "str"sv)
			{
				new(result) arena_typed_value(&arena, node_type, text);
				return true;
			}
			else
			{
				new(result) arena_typed_value(&arena, static_cast<uint64_t>(current_double_value.value()));
				return true;
			}
		}
		case basic_value_type::tuple:
		{
			auto cur_tuple_detail = std::get<typed_value_desc::tuple_detail_t>(node_type->_type_detail);
			char sep = cur_tuple_detail.second;
			auto type_list = cur_tuple_detail.first;
			text = strip_parenthesis(text);
			auto tokens = split_string(text, sep);
			if (tokens.size() != type_list.size())
			{
				return false;
			}
			vector<arena_typed_value *> sub_values;
			for (std::uint32_t i = 0; i < type_list.size(); i++)
			{
				arena_typed_value* temp_result = arena.get<arena_typed_value>(1);
				temp_result->~arena_typed_value();
				auto cur_value = parse_value_with_type(type_list[i], tokens[i], *temp_result);
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
			auto cur_list_detail = std::get<typed_value_desc::list_detail_t>(node_type->_type_detail);
			uint32_t list_size = std::get<uint32_t>(cur_list_detail);
			char sep = std::get<char>(cur_list_detail);
			auto unit_type = std::get<typed_value_desc *>(cur_list_detail);
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
	const arena_typed_value* arena_typed_value_parser::match_node(string_view text)
	{
		return arena_typed_value_parser::parse_value_with_type(type_desc, text);
	}
	bool arena_typed_value_parser::match_node(string_view text, arena_typed_value& result)
	{
		return arena_typed_value_parser::parse_value_with_type(type_desc, text, result);
	}

}