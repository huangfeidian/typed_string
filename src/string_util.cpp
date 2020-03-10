#include <string_util.h>
#include <charconv>
#include <sstream>

namespace spiritsaway::string_util
{
    using namespace std;
    string concate_string(const vector<string>& str_list)
	{
		// return accumulate(str_list.cbegin(), str_list.cend(), "", [](const string& a, const string& b)
		// {
		// 	return a + b;
		// }
		string result_str;
		int total_size = 0;
		for(const auto& i : str_list)
		{
			total_size += i.size();
		}
		result_str.reserve(total_size);
		for (const auto& i : str_list)
		{
			result_str += i;
		}
		return result_str;
	}
    string concate_string(const vector<string_view>& str_list)
	{
		// return accumulate(str_list.cbegin(), str_list.cend(), "", [](const string& a, const string& b)
		// {
		// 	return a + b;
		// }
		string result_str;
		int total_size = 0;
		for(const auto& i : str_list)
		{
			total_size += i.size();
		}
		result_str.reserve(total_size);
		for (const auto& i : str_list)
		{
			result_str += i;
		}
		return result_str;
	}
    string_view strip_blank(std::string_view input)
    {
        int left = 0;
        int right = input.size();
        while (left < right)
        {
            auto cur_char = input[left];
            if (cur_char == ' ' || cur_char == '\t' || cur_char == '\n')
            {
                left++;
            }
            else
            {
                break;
            }
        }
        while (left < right)
        {
            
            auto cur_char = input[right - 1];
            if (cur_char == ' ' || cur_char == '\t' || cur_char == '\n')
            {
                right--;
            }
            else
            {
                break;
            }
        }
        if (left == right)
        {
            return string_view();
        }
        else
        {
            return input.substr(left, right - left);
        }
    }
    template<>
    optional<bool> cast_string_view<bool>(string_view _text)
    {
        if (_text == "0")
        {
            return false;
        }
        else if (_text == "1")
        {
            return true;
        }
        else
        {
            return nullopt;
        }
    }
    template<>
    optional<uint32_t> cast_string_view<uint32_t>(string_view _text)
    {
        uint32_t result;
        if (auto[p, ec] = std::from_chars(_text.data(), _text.data() + _text.size(), result); ec == std::errc())
        {
            return result;
        }
        return nullopt;
    }
    template<>
    optional<int> cast_string_view<int>(string_view _text)
    {
        int result;
        if (auto[p, ec] = std::from_chars(_text.data(), _text.data() + _text.size(), result); ec == std::errc())
        {
            return result;
        }
        return nullopt;
    }
    template<>
    optional<uint64_t> cast_string_view<uint64_t>(string_view _text)
    {
        uint64_t result;
        if (auto[p, ec] = std::from_chars(_text.data(), _text.data() + _text.size(), result); ec == std::errc())
        {
            return result;
        }
        return nullopt;
    }
    template<>
    optional<int64_t> cast_string_view<int64_t>(string_view _text)
    {
        int64_t result;
        if (auto[p, ec] = std::from_chars(_text.data(), _text.data() + _text.size(), result); ec == std::errc())
        {
            return result;
        }
        return nullopt;
    }
    template<>
    optional<float> cast_string_view<float>(string_view _text)
    {
		// gcc and clang cant support from_chars to float or double
        //float result;
        //if (auto[p, ec] = std::from_chars(_text.data(), _text.data() + _text.size(), result); ec == std::errc())
        //{
        //    return result;
        //}
        //return nullopt;
		std::istringstream cur_stream(std::string(_text.data(), _text.size()));
		float result;
		cur_stream >> result;
		if (cur_stream.fail() || !cur_stream.eof())
		{
			return std::nullopt;
		}
		else
		{
			return result;
		}
    }
    template<>
    optional<double> cast_string_view<double>(string_view _text)
    {
        /*double result;
        if (auto[p, ec] = std::from_chars(_text.data(), _text.data() + _text.size(), result); ec == std::errc())
        {
            return result;
        }
        return nullopt;*/
		std::istringstream cur_stream(std::string(_text.data(), _text.size()));
		double result;
		cur_stream >> result;
		if (cur_stream.fail() || !cur_stream.eof())
		{
			return std::nullopt;
		}
		else
		{
			return result;
		}
    }
}
