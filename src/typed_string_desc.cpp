#include <typed_string_desc.h>
#include <unordered_map>
#include <string_util.h>
#include <iostream>
#include <sstream>
namespace spiritsaway::container{
	using namespace std;
	using namespace spiritsaway::string_util;
	spiritsaway::memory::arena typed_string_desc::memory_arena(4 * 1024);
	vector<string_view> parse_token_from_type_str(string_view type_string)
	{
		uint32_t begin_idx = 0;
		uint32_t end_idx = 0;
		vector<string_view> result;
		char cur_char = type_string[0];
		for(end_idx = 0; end_idx < type_string.size(); end_idx++)
		{
			cur_char = type_string[end_idx];
			if(cur_char == ',' || cur_char == '(' || cur_char == ')' || cur_char == ' ')
			{
				if(begin_idx != end_idx)
				{
					if (cur_char != ' ' && cur_char != ',')
					{
						result.emplace_back(type_string.data() + begin_idx, end_idx - begin_idx);
						result.emplace_back(type_string.data() + end_idx, 1);
					}
					else
					{
						result.emplace_back(type_string.data() + begin_idx, end_idx - begin_idx);
					}
					
				}
				else
				{
					if(cur_char != ' ' && cur_char != ',')
					{
						result.emplace_back(type_string.data() + begin_idx, end_idx + 1 - begin_idx);
					}
				}
				begin_idx = end_idx + 1;
			}
		}
		if(begin_idx != end_idx)
		{
			result.emplace_back(type_string.data() + begin_idx, end_idx - begin_idx);
		}
 
		return result;

	}
	vector<string_view> get_next_node(const vector<string_view>& tokens, size_t begin_idx)
	{
		vector<string_view> result;
		size_t unmatched = 0;
		while(begin_idx < tokens.size())
		{
			if(tokens[begin_idx] == string_view("("))
			{
				unmatched += 1;
			}
			else if(tokens[begin_idx] == string_view(")"))
			{
				
				unmatched -= 1;
			}
			result.push_back(tokens[begin_idx]);
			if(unmatched == 0)
			{
				if(tokens[begin_idx] == "list"sv || tokens[begin_idx] == "tuple"sv || tokens[begin_idx] == "choice_str"sv || tokens[begin_idx] == "choice_int"sv)
				{
					begin_idx += 1;
				}
				else
				{
					return result;
				}
			}
			else
			{
				begin_idx += 1;
			}
			
		}
		if(unmatched != 0)
		{
			return {};
		}
		else
		{
			return result;
		}
	}
	typed_string_desc* parse_type_from_tokens(vector<string_view> tokens)
	{
		static unordered_map<string_view, typed_string_desc*> str_to_type_map = {
			{"comment", new typed_string_desc(basic_value_type::comment)},
			{"str", new typed_string_desc(basic_value_type::string)},
			{"bool", new typed_string_desc(basic_value_type::number_bool)},
			{"uint32", new typed_string_desc(basic_value_type::number_u32)},
			{"int", new typed_string_desc(basic_value_type::number_32)},
			{"uint64", new typed_string_desc(basic_value_type::number_u64)},
			{"int64", new typed_string_desc(basic_value_type::number_64)},
			{"float", new typed_string_desc(basic_value_type::number_float)},
			{"double", new typed_string_desc(basic_value_type::number_double)},
		};
		if(tokens.empty())
		{
			return nullptr;
		}
		if(tokens.size() == 1)
		{
			auto map_iter = str_to_type_map.find(tokens[0]);
			if(map_iter != str_to_type_map.end())
			{
				
				return map_iter->second;
			}
			else
			{
				return nullptr;
			}
		}
		else
		{
			if(tokens[0] == string_view("choice_int"))
			{
				//choice_int(1,2,3)
				if(tokens[1] != string_view("(") || tokens.back() != string_view(")"))
				{
					return nullptr;
				}
				std::vector<int> choice_values;
				for(std::size_t i = 2; i< tokens.size() - 1; i++)
				{
					auto opt_value = cast_string_view<int>(tokens[i]);
					if(!opt_value)
					{
						return nullptr;
					}
					choice_values.push_back(opt_value.value());
				}
				if(choice_values.empty())
				{
					return nullptr;
				}
				auto int_buffer = typed_string_desc::memory_arena.get<int>(choice_values.size());
				for(std::size_t i = 0; i< choice_values.size(); i++)
				{
					int_buffer[i] = choice_values[i];
				}
				typed_string_desc* result = typed_string_desc::memory_arena.get<typed_string_desc>(1);
				new(result) typed_string_desc(basic_value_type::choice_int);
				result->_type_detail = make_pair(int_buffer, choice_values.size());
				return result;
			}
			else if(tokens[0] == "choice_str"sv)
			{
				//choice_int(red,blue,green)
				if(tokens[1] != string_view("(") || tokens.back() != string_view(")"))
				{
					return nullptr;
				}
				std::vector<string_view> choice_values;
				for(std::size_t i = 2; i< tokens.size() - 1; i++)
				{
					auto p_str_view = typed_string_desc::memory_arena.get<char>(tokens[i].size());
					std::copy(tokens[i].begin(), tokens[i].end(), p_str_view);
					auto new_str_view = std::string_view(p_str_view, tokens[i].size());
					choice_values.push_back(new_str_view);
				}
				if(choice_values.empty())
				{
					return nullptr;
				}
				auto sv_buffer = typed_string_desc::memory_arena.get<string_view>(choice_values.size());
				for(std::size_t i = 0; i< choice_values.size(); i++)
				{
					sv_buffer[i] = choice_values[i];
				}
				typed_string_desc* result = typed_string_desc::memory_arena.get<typed_string_desc>(1);
				new(result) typed_string_desc(basic_value_type::choice_str);
				result->_type_detail = make_pair(sv_buffer, choice_values.size());
				return result;
			}
			else if(tokens[0] == string_view("list") || tokens[0] == string_view("tuple"))
			{

				if(tokens[1] != string_view("("))
				{
					return nullptr;
				}
				if(tokens.size() < 5)
				{
					return nullptr;
				}
				if(tokens.back() != string_view(")"))
				{
					return nullptr;
				}
				vector<vector<string_view>> grouped_tokens;
				std::uint32_t sub_token_begin_idx = 2;
				while(tokens[sub_token_begin_idx] != ")"sv)
				{
					auto cur_sub_tokens = get_next_node(tokens, sub_token_begin_idx);
					if(cur_sub_tokens.empty())
					{
						return nullptr;
					}
					sub_token_begin_idx += cur_sub_tokens.size();
					if(sub_token_begin_idx >= tokens.size())
					{
						return nullptr;
					}
					grouped_tokens.push_back(move(cur_sub_tokens));
				}
				
				if(tokens[0] == "list"sv)
				{
					
					char splitor = ',';
					if(grouped_tokens.size() == 3)
					{
						// list(type, len, seperator)
						auto splitor_info = grouped_tokens.back();
						grouped_tokens.pop_back();
						if(splitor_info.size() != 1)
						{
							return nullptr;
						}
						if(splitor_info[0].size() != 1)
						{
							return nullptr;
						}
						splitor = splitor_info[0][0];
					}
					// list(type, len)
					if(grouped_tokens.size() != 2)
					{
						return nullptr;
					}
					auto temp_type_result = parse_type_from_tokens(grouped_tokens[0]);
					if(!temp_type_result)
					{
						return nullptr;
					}
					if(grouped_tokens[1].size() != 1)
					{
						return nullptr;
					}

					auto repeat_times = cast_string_view<int>(grouped_tokens[1][0]);
					if(!repeat_times)
					{
						return nullptr;
					}
					else
					{
						typed_string_desc* result = typed_string_desc::memory_arena.get<typed_string_desc>(1);
						new(result) typed_string_desc(make_tuple(temp_type_result, repeat_times.value(), splitor));
						return result;
					}

				}
				else
				{
					//tuple(xx, xx, seperator)
					vector<typed_string_desc*> type_vec;
					char cur_splitor = ',';
					auto splitor_info = grouped_tokens.back();
					if(splitor_info.size() == 1 && splitor_info[0].size() == 1)
					{
						cur_splitor = splitor_info[0][0];
						grouped_tokens.pop_back();
					}
					for(const auto & one_grouped_tokens: grouped_tokens)
					{
						auto temp_result = parse_type_from_tokens(one_grouped_tokens);
						if(!temp_result)
						{
							return nullptr;
						}
						else
						{
							type_vec.push_back(temp_result);
						}
						
					}
					if (type_vec.empty())
					{
						return nullptr;
					}
					auto p_vec = typed_string_desc::memory_arena.get< typed_string_desc*>(type_vec.size());
					std::copy(type_vec.cbegin(), type_vec.cend(), p_vec);
					typed_string_desc* result = typed_string_desc::memory_arena.get<typed_string_desc>(1);
					new(result) typed_string_desc(make_tuple(p_vec, type_vec.size(), cur_splitor));
					return result;
				}

			}
		}
		return nullptr;
	}
	
	
	optional<typed_string_desc::list_detail_t> typed_string_desc::get_list_detail_t() const
	{
		if(_type != basic_value_type::list)
		{
			return nullopt;
		}
		else
		{
			return std::get<typed_string_desc::list_detail_t>(_type_detail);
		}
	}

	optional<typed_string_desc::choice_int_detail_t> typed_string_desc::get_choice_int_detail_t() const
	{
		if(_type != basic_value_type::choice_int)
		{
			return nullopt;
		}
		else
		{
			return std::get<typed_string_desc::choice_int_detail_t>(_type_detail);
		}
	}

	optional<typed_string_desc::choice_str_detail_t> typed_string_desc::get_choice_str_detail_t() const
	{
		if(_type != basic_value_type::choice_str)
		{
			return nullopt;
		}
		else
		{
			return std::get<typed_string_desc::choice_str_detail_t>(_type_detail);
		}
	}

	optional<typed_string_desc::tuple_detail_t> typed_string_desc::get_tuple_detail_t() const
	{
		if(_type != basic_value_type::tuple)
		{
			return nullopt;
		}
		else
		{
			return std::get<typed_string_desc::tuple_detail_t>(_type_detail);
		}
	}

	typed_string_desc::typed_string_desc()
	{
		_type = basic_value_type::comment;
	}
	typed_string_desc::typed_string_desc(basic_value_type in_type)
	{
		_type = in_type;
	}
	typed_string_desc::typed_string_desc(const typed_string_desc::tuple_detail_t& tuple_detail):
		_type(basic_value_type::tuple), _type_detail(tuple_detail)
	{

	}
	typed_string_desc::typed_string_desc(const typed_string_desc::choice_int_detail_t& choice_int_detail):
		_type(basic_value_type::choice_int), _type_detail(choice_int_detail)
	{

	}
	typed_string_desc::typed_string_desc(const typed_string_desc::choice_str_detail_t& choice_str_detail):
		_type(basic_value_type::choice_str), _type_detail(choice_str_detail)
	{

	}
	typed_string_desc::typed_string_desc(const typed_string_desc::list_detail_t& list_detail):
		_type(basic_value_type::list), _type_detail(list_detail)
	{

	}


	ostream& operator<<(ostream& output_stream, const typed_string_desc& cur_type)
	{
		static unordered_map<basic_value_type, string_view> type_to_string = {
			{basic_value_type::comment, "comment"},
			{basic_value_type::string, "string"},
			{basic_value_type::number_bool, "bool"},
			{basic_value_type::number_u32, "uint32"},
			{basic_value_type::number_32, "int32"},
			{basic_value_type::number_64, "int64"},
			{basic_value_type::number_u64, "uint64"},
			{basic_value_type::number_float, "float"},
			{basic_value_type::number_double, "double"},
			{basic_value_type::list, "list"},
			{basic_value_type::choice_int, "choice_int"},
			{basic_value_type::choice_str, "choice_str"},
			{basic_value_type::tuple, "tuple"},
		};
		auto temp_iter = type_to_string.find(cur_type._type);
		if(temp_iter == type_to_string.end())
		{
			return output_stream<<"invalid";
		}
		output_stream<<temp_iter->second;
		if(cur_type._type == basic_value_type::choice_int)
		{
			auto temp_detail = std::get<typed_string_desc::choice_int_detail_t>(cur_type._type_detail);
			auto [buffer_p, count] = temp_detail;
			output_stream<<"(";
			for(std::size_t i = 0 ; i< count; i++)
			{
				output_stream<<*(buffer_p + i);
				if(i != count - 1)
				{
					output_stream<<", ";
				}
			}
			output_stream<<")";
			return output_stream;
		}
		else if(cur_type._type == basic_value_type::choice_str)
		{
			auto temp_detail = std::get<typed_string_desc::choice_str_detail_t>(cur_type._type_detail);
			auto [buffer_p, count] = temp_detail;
			output_stream <<"(";
			for(std::size_t i = 0 ; i< count; i++)
			{
				output_stream<<*(buffer_p + i);
				if(i != count - 1)
				{
					output_stream<<", ";
				}
			}
			output_stream<<")";
			return output_stream;
		}
		else if(cur_type._type == basic_value_type::tuple)
		{
			auto temp_detail = std::get<typed_string_desc::tuple_detail_t>(cur_type._type_detail);
			output_stream<<"(";
			auto[p_vec, count, sep] = temp_detail;
			for(std::uint32_t i = 0; i< count; i++)
			{
				output_stream<<*p_vec[i];
				if(i != count - 1)
				{
					output_stream<<",";
				}
			}
			if(sep != ',')
			{
				output_stream<<", "<< sep; 
			}
			output_stream<<")";
			return output_stream;
		}
		else if(cur_type._type == basic_value_type::list)
		{
			auto temp_detail = std::get<typed_string_desc::list_detail_t>(cur_type._type_detail);
			output_stream<<"("<<*std::get<0>(temp_detail)<<", "<<std::get<1>(temp_detail);
			auto sep = std::get<2>(temp_detail);
			if(sep != ',')
			{
				output_stream<<", "<<sep;
			}
			output_stream<<")";
			return output_stream;
		}
		else
		{
			return output_stream;
		}
	}
	std::string typed_string_desc::to_string() const
	{
		std::ostringstream buffer;
		buffer << *this;
		return buffer.str();
	}
	bool operator==(const typed_string_desc& cur, const typed_string_desc& other)
	{
		if(cur._type != other._type)
		{
			return false;
		}
		switch(cur._type)
		{
			case basic_value_type::comment:
			case basic_value_type::string:
			case basic_value_type::number_bool:
			case basic_value_type::number_32:
			case basic_value_type::number_64:
			case basic_value_type::number_double:
			case basic_value_type::number_float:
			case basic_value_type::number_u32:
			case basic_value_type::number_u64:
				return true;
			case basic_value_type::list:
			{
				if(cur._type_detail.index() != other._type_detail.index())
				{
					return false;
				}
				auto cur_detail = std::get<typed_string_desc::list_detail_t>(cur._type_detail);
				auto other_detail = std::get<typed_string_desc::list_detail_t>(other._type_detail);
				if(std::get<1>(cur_detail) != std::get<1>(other_detail))
				{
					return false;
				}
				if(*std::get<0>(cur_detail) == *std::get<0>(other_detail))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
				
			case basic_value_type::choice_int:
			{
				if(cur._type_detail.index() != other._type_detail.index())
				{
					return false;
				}
				auto [cur_buffer, cur_count] = std::get<typed_string_desc::choice_int_detail_t>(cur._type_detail);
				auto [other_buffer, other_count] = std::get<typed_string_desc::choice_int_detail_t>(other._type_detail);
				if(cur_count != other_count)
				{
					return false;
				}
				for(std::size_t i = 0; i< cur_count; i++)
				{
					bool found = false;
					for(std::size_t j = 0; j < cur_count; j++)
					{
						if(cur_buffer[i] == other_buffer[j])
						{
							found = true;
							break;
						}
					}
					if(!found)
					{
						return false;
					}
				}
				return true;
			}
			case basic_value_type::choice_str:
			{
				if(cur._type_detail.index() != other._type_detail.index())
				{
					return false;
				}
				auto [cur_buffer, cur_count] = std::get<typed_string_desc::choice_str_detail_t>(cur._type_detail);
				auto [other_buffer, other_count] = std::get<typed_string_desc::choice_str_detail_t>(other._type_detail);
				if(cur_count != other_count)
				{
					return false;
				}
				for(std::size_t i = 0; i< cur_count; i++)
				{
					bool found = false;
					for(std::size_t j = 0; j < cur_count; j++)
					{
						if(cur_buffer[i] == other_buffer[j])
						{
							found = true;
							break;
						}
					}
					if(!found)
					{
						return false;
					}
				}
				return true;
			}
			case basic_value_type::tuple:
			{
				if(cur._type_detail.index() != other._type_detail.index())
				{
					return false;
				}
				auto cur_detail = std::get<typed_string_desc::tuple_detail_t>(cur._type_detail);
				auto[cur_vec_p, cur_count, cur_sep] = cur_detail;
				auto other_detail = std::get<typed_string_desc::tuple_detail_t>(other._type_detail);
				auto[other_vec_p, other_count, other_sep] = other_detail;
				if (cur_count != other_count || cur_sep != other_sep)
				{
					return false;
				}
				
				for(std::uint32_t i = 0; i < cur_count; i++)
				{
					if(*cur_vec_p[i] == *other_vec_p[i])
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
			default:
				return false;
		}
	}
	bool operator!=(const typed_string_desc& cur, const typed_string_desc& other)
	{
		return !(cur == other);
	}
    const typed_string_desc* typed_string_desc::get_basic_type_desc(basic_value_type in_type)
	{
		static std::vector<typed_string_desc*> result = {
			new typed_string_desc(basic_value_type::comment),
			new typed_string_desc(basic_value_type::string),
			new typed_string_desc(basic_value_type::number_bool),
			new typed_string_desc(basic_value_type::number_u32),
			new typed_string_desc(basic_value_type::number_32),
			new typed_string_desc(basic_value_type::number_u64),
			new typed_string_desc(basic_value_type::number_64),
			new typed_string_desc(basic_value_type::number_float),
			new typed_string_desc(basic_value_type::number_double),
		};

		if (static_cast<uint32_t>(in_type) > static_cast<uint32_t>(basic_value_type::number_double))
		{
			return result[0];
		}
		else
		{
			return result[static_cast<uint32_t>(in_type)];
		}
	}
    const typed_string_desc* typed_string_desc::get_type_from_str(string_view type_string)
	{
		auto all_tokens = parse_token_from_type_str(type_string);
		auto result = parse_type_from_tokens(all_tokens);
		// check delimiter conflict
		if (!result)
		{
			std::cout << "parse value desc fail for " << type_string << std::endl;
			return result;
		}
		std::vector<char> pre_delimiter;
		if (result->check_delimiter_conflict(pre_delimiter))
		{
			std::cout << "conflict delimiter in " << *result << std::endl;
			return nullptr;
		}
		else
		{
			return result;
		}
	}
	bool typed_string_desc::check_delimiter_conflict(std::vector<char>& pre_delimiter) const
	{
		switch (_type)
		{
		case basic_value_type::list:
		{
			auto cur_list_detail_opt = get_list_detail_t();
			if (!cur_list_detail_opt)
			{
				return true;
			}
			auto cur_delimiter = std::get<2>(cur_list_detail_opt.value());
			if (std::find(pre_delimiter.cbegin(), pre_delimiter.cend(), cur_delimiter) != pre_delimiter.cend())
			{
				return true;
			}
			pre_delimiter.push_back(cur_delimiter);
			if (std::get<0>(cur_list_detail_opt.value())->check_delimiter_conflict(pre_delimiter))
			{
				return true;
			}
			else
			{
				pre_delimiter.pop_back();
				return false;
			}
		}
		case basic_value_type::tuple:
		{
			auto cur_tuple_detail_opt = get_tuple_detail_t();
			if (!cur_tuple_detail_opt)
			{
				return true;
			}
			auto[vec_p, count, sep] = cur_tuple_detail_opt.value();
			if (std::find(pre_delimiter.cbegin(), pre_delimiter.cend(), sep) != pre_delimiter.cend())
			{
				return true;
			}
			pre_delimiter.push_back(sep);
			for (std::size_t i = 0; i < count; i++)
			{
				if (vec_p[i]->check_delimiter_conflict(pre_delimiter))
				{

					return true;
				}
			}
			pre_delimiter.pop_back();
			return false;
		}
		default:
			return false;
		}
	}
	typed_string_desc::~typed_string_desc()
	{
		return;
	}
	std::uint32_t typed_string_desc::clear_all_desc()
	{
		auto result = typed_string_desc::memory_arena.consumption();
		typed_string_desc::memory_arena.drop();
		return result;
	}
}