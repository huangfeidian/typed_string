#pragma once
#include <string_view>
#include <variant>
#include <optional>

#include "memory_arena.h"
#include "typed_string_desc.h"

namespace spiritsaway::container
{
    
	class arena_typed_value;
	struct arena_typed_vec
	{
		arena_typed_value** p_value;
		std::uint32_t size;
	};
	class arena_typed_value
	{
	public:
		
		const typed_value_desc* type_desc;
		const spiritsaway::memory::arena* arena;
		union {
			bool v_bool;
			std::uint32_t v_uint32; 
			std::int32_t v_int32;
			std::uint64_t v_uint64;
			std::int64_t v_int64;
			float v_float;
			double v_double;
			std::string_view v_text;
			arena_typed_vec v_vec; 
		};
		arena_typed_value();
		arena_typed_value(const spiritsaway::memory::arena* arena, bool in_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, std::uint32_t in_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, std::int32_t in_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, std::int64_t in_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, std::uint64_t in_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, std::string_view in_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, float in_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, double in_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, const typed_value_desc* in_type_desc, std::string_view in_ref_value);
		arena_typed_value(const spiritsaway::memory::arena* arena, const typed_value_desc* in_type_desc, arena_typed_vec in_arena_typed_vec);
		friend std::ostream& operator<<(std::ostream& output_stream, const arena_typed_value& cur_node);
		
		friend bool operator==(const arena_typed_value& cur, const arena_typed_value& other);
		friend bool operator!=(const arena_typed_value& cur, const arena_typed_value& other);
		template <typename T> 
		std::optional<T> expect_simple_value() const;
		template <typename T>
		std::optional<T> expect_value() const;
		template <typename... args>
		std::optional<std::tuple<args ...>> expect_tuple_value() const;

	private:
		template<typename T> struct deduce_type{};
		template <typename T>
		auto expect_value_dispatch(deduce_type<T>) const
		{
			return expect_simple_value<T>();
		}
		template <typename... args>
		auto expect_value_dispatch(deduce_type<std::tuple<args...>>) const
		{
			return expect_tuple_value<args...>();
		}
		
	};

	template <>
	std::optional<std::uint32_t> arena_typed_value::expect_simple_value<std::uint32_t>() const;
	template <>
	std::optional<std::int32_t> arena_typed_value::expect_simple_value<std::int32_t>() const;
	template <>
	std::optional<std::int64_t> arena_typed_value::expect_simple_value<std::int64_t>() const;
	template <>
	std::optional<std::uint64_t> arena_typed_value::expect_simple_value<std::uint64_t>() const;
	template <>
	std::optional<float> arena_typed_value::expect_simple_value<float>() const;
	template <>
	std::optional<double> arena_typed_value::expect_simple_value<double>() const;
	template <>
	std::optional<bool> arena_typed_value::expect_simple_value<bool>() const;
	template <>
	std::optional<std::string_view> arena_typed_value::expect_simple_value<std::string_view>() const;

	template<typename... args, size_t... arg_idx>
	std::optional<std::tuple<args...>> get_tuple_value_from_vector(const arena_typed_value* const * v_vec, std::index_sequence<arg_idx...>)
	{
		if(!(v_vec[arg_idx] &&...))
		{
			return std::nullopt;
		}
		auto temp_result = std::make_tuple((*v_vec[arg_idx]).expect_value<args>()...);

		if(!(std::get<arg_idx>(temp_result) &&...))
		{
			return std::nullopt;
		}
		return std::make_tuple(std::get<arg_idx>(temp_result).value()...);

	}

	template<typename... args>
	std::optional<std::tuple<args...>> arena_typed_value::expect_tuple_value() const
	{
		if(v_vec.size == 0)
		{
			return std::nullopt;
		}
		for(std::uint32_t i = 0; i< v_vec.size; i++)
		{
			if(!v_vec.p_value[i])
			{
				return std::nullopt;
			}
		}
		auto the_tuple_size = sizeof...(args);
		if(v_vec.size != the_tuple_size)
		{
			return std::nullopt;
		}
		return get_tuple_value_from_vector<args...>(v_vec.p_value, std::index_sequence_for<args...>{});

	}
	template <typename T>
	std::optional<T> arena_typed_value::expect_value() const
	{
		if (!type_desc)
		{
			return std::nullopt;
		}
		return expect_value_dispatch(deduce_type<T>());
	}

	struct arena_typed_value_hash
	{
		std::size_t operator()(const arena_typed_value* s) const;
	};
	struct arena_typed_value_ptr_equal
	{
		bool operator()(const arena_typed_value* from, const arena_typed_value* to) const;
	};
    class arena_typed_value_parser
	{
	private:
		spiritsaway::memory::arena& arena;
	public:
		const typed_value_desc* type_desc;
		arena_typed_value_parser(spiritsaway::memory::arena& arena);
		const arena_typed_value* match_node(std::string_view text);
		bool match_node(std::string_view text, arena_typed_value& result);

		typed_value_desc* parse_type(std::string_view type_string);

		arena_typed_value* parse_value_with_type(const typed_value_desc* node_type, std::string_view text);
		bool parse_value_with_type(const typed_value_desc* type_desc, std::string_view text, arena_typed_value& result);

		template <typename T>
		arena_typed_value* prase_node_with_number(std::string_view text)
		{
			std::optional<T> result_opt = cast_string_view<T>(text);
			if (!result_opt)
			{
				return nullptr;
			}
			return new arena_typed_value(result_opt.value());
		}
		template <typename T>
		bool prase_node_with_number(std::string_view text, arena_typed_value& result)
		{
			std::optional<T> result_opt = cast_string_view<T>(text);
			if (!result_opt)
			{
				return false;
			}
			new(&result) arena_typed_value(result_opt.value());
			return true;
		}
	private:
		bool parse_value_with_address(const typed_value_desc* typed_desc, std::string_view text, arena_typed_value* result);
	};

}