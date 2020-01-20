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
		ref_id,// ref to one cell in one worksheet REF(WORKSHEET, str) ref(worksheet, int)
		tuple,// TUPLE(FLOAT, STRING) tuple(float, float, float)
		list,//LIST(FLOAT, 3) LIST(FLOAT, 0) LIST(TUPLE(FLOAT, STRING), 2) LIST(LIST(FLOAT, 3), 3)

	};
	
	struct typed_string_desc
	{
		using ref_detail_t = std::pair<std::string_view, std::string_view>;
		using list_detail_t = std::tuple<typed_string_desc*, std::uint32_t, char>;// detail_type length <seperator>
		using tuple_detail_t = std::pair<std::vector<typed_string_desc*>, char>;//<type1, type2, type3> seperator
		basic_value_type _type;
		std::variant<ref_detail_t, tuple_detail_t, list_detail_t> _type_detail;
		typed_string_desc();
		
		typed_string_desc(basic_value_type in_type);
		typed_string_desc(const tuple_detail_t& tuple_detail);
		typed_string_desc(const list_detail_t& list_detail);
		typed_string_desc(const ref_detail_t& ref_detail);
		std::string to_string() const;
		friend std::ostream& operator<<(std::ostream& output_stream, const typed_string_desc& cur_node);
		static const typed_string_desc* get_basic_type_desc(basic_value_type in_type);
		static const typed_string_desc* get_type_from_str(std::string_view type_string);
		friend bool operator==(const typed_string_desc& cur, const typed_string_desc& other);
		friend bool operator!=(const typed_string_desc& cur, const typed_string_desc& other);
		std::optional<list_detail_t> get_list_detail_t() const;
		std::optional<ref_detail_t> get_ref_detail_t() const;
		std::optional<tuple_detail_t> get_tuple_detail_t() const;
		typed_string_desc(const typed_string_desc& other) = delete;
		typed_string_desc& operator=(const typed_string_desc& other) = delete;
		~typed_string_desc();
		bool check_delimiter_conflict(std::vector<char>& pre_delimiter) const;
		
		static spiritsaway::memory::arena memory_arena;
		static std::uint32_t clear_all_desc();
	};
}