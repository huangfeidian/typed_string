#pragma once
#include <vector>
#include <cstdint>
namespace spiritsaway::memory
{

	constexpr std::uint32_t align_to(std::uint32_t pre_size, std::uint32_t align_size)
	{
		if ((pre_size & (align_size - 1)) != 0)
		{
			pre_size = (pre_size ^ (align_size - 1)) + align_size;
		}
		return pre_size;
	}
    class arena
	{
		static const std::uint32_t buffer_page_size = 16 * 1024;
		std::vector<char*> buffers;
		std::uint32_t cur_buffer_used = 0;
	public:
		template <typename T>
		T* get(std::uint32_t count)
		{
			if (count == 0)
			{
				return nullptr;
			}
			return reinterpret_cast<T*>(get(sizeof(T) * count, alignof(T)));
		}
		char* get(std::uint32_t m_size, std::uint32_t align_size)
		{
			m_size = align_to(m_size, align_size);
            //for big page
			if (m_size >= buffer_page_size)
			{
				auto result = static_cast<char*>(malloc(m_size));
				if (buffers.empty())
				{
					buffers.push_back(result);
					cur_buffer_used = buffer_page_size;
					return result;
				}
				else
				{
					auto cur_buffer_size = buffers.size();
					buffers.push_back(result);
					std::swap(buffers[cur_buffer_size], buffers[cur_buffer_size - 1]);
					return result;
				}
			}
            // for small page
			cur_buffer_used = align_to(cur_buffer_used, align_size);
			if (buffers.empty())
			{
				auto new_buffer_p = static_cast<char*>(malloc(buffer_page_size));
				buffers.push_back(new_buffer_p);

			}
			if (cur_buffer_used + m_size > buffer_page_size)
			{
				auto new_buffer_p = static_cast<char*>(malloc(buffer_page_size));
				cur_buffer_used = m_size;
				buffers.push_back(new_buffer_p);
				return new_buffer_p;

			}
			else
			{
				auto result = cur_buffer_used;
				cur_buffer_used += m_size;
				auto cur_buffer_p = buffers.back();
				return cur_buffer_p + result;
			}
		}
        ~arena()
        {
            for(auto one_buffer:buffers)
            {
                free(one_buffer);
            }
            return;
        }
	};
}