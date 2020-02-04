#pragma once

#include <string_view>
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <ostream>
#include "memory_arena.h"

namespace spiritsaway::container
{

	enum class basic_value_type
	{
		comment = 0,//COMMENT
		string,//shared_string table
		number_bool,//BOOL
		number_u32,//UINT
		number_32,//INT
		number_u64,//LLINT
		number_64,//ULLINT
		number_float,//FLOAT
		number_double,//DOUBLE
		// complicated types
		choice_int, // one value in selected interger choice_int(0,1,2) 
		choice_str,// one value in slected strings choice_str(red, blue, green)
		tuple,// TUPLE(FLOAT, STRING) tuple(float, float, float)
		list,//LIST(FLOAT, 3) LIST(FLOAT, 0) LIST(TUPLE(FLOAT, STRING), 2) LIST(LIST(FLOAT, 3), 3)

	};
	
	struct typed_string_desc
	{
		using choice_int_detail_t = std::pair<int*, std::uint32_t>;
		using choice_str_detail_t = std::pair<std::string_view*, std::uint32_t>;
		using list_detail_t = std::tuple<typed_string_desc*, std::uint32_t, char>;// detail_type length <seperator>
		using tuple_detail_t = std::tuple<typed_string_desc**, std::uint32_t, char>;//<type1, type2, type3> seperator
		basic_value_type _type;
		std::variant<choice_int_detail_t, choice_str_detail_t, tuple_detail_t, list_detail_t> _type_detail;
		typed_string_desc();
		
		typed_string_desc(basic_value_type in_type);
		typed_string_desc(const tuple_detail_t& tuple_detail);
		typed_string_desc(const list_detail_t& list_detail);
		typed_string_desc(const choice_str_detail_t& choice_detail);
		typed_string_desc(const choice_int_detail_t& choice_detail);
		typed_string_desc(const std::vector<int>& choice_values);
		typed_string_desc(const std::vector<std::string_view>& choice_values);
		std::string to_string() const;
		friend std::ostream& operator<<(std::ostream& output_stream, const typed_string_desc& cur_node);
		static const typed_string_desc* get_basic_type_desc(basic_value_type in_type);
		static const typed_string_desc* get_type_from_str(std::string_view type_string);
		friend bool operator==(const typed_string_desc& cur, const typed_string_desc& other);
		friend bool operator!=(const typed_string_desc& cur, const typed_string_desc& other);
		std::optional<list_detail_t> get_list_detail_t() const;
		std::optional<choice_str_detail_t> get_choice_str_detail_t() const;
		std::optional<choice_int_detail_t> get_choice_int_detail_t() const;
		std::optional<tuple_detail_t> get_tuple_detail_t() const;
		typed_string_desc(const typed_string_desc& other) = delete;
		typed_string_desc& operator=(const typed_string_desc& other) = delete;
		~typed_string_desc();
		bool check_delimiter_conflict(std::vector<char>& pre_delimiter) const;
		
		static spiritsaway::memory::arena memory_arena;
		static std::uint32_t clear_all_desc();
	};
}