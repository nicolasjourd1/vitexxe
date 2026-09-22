#include "utf8.hpp"

namespace vx
{

std::vector<char32_t> utf8_to_utf32(std::string_view input)
{
    std::vector<char32_t> result;
    result.reserve(input.length());

    utf8_iterator it(input.data(), input.data() + input.length());
    utf8_iterator end(input.data() + input.length(), input.data() + input.length());

    for (; it != end; ++it)
    {
        result.push_back(*it);
    }

    return result;
}

std::string utf32_to_utf8(std::span<const char32_t> input)
{
    std::string result;
    result.reserve(input.size() * 4);

    for (char32_t cp : input)
    {
        if (cp <= 0x7F)
        {
            result.push_back(static_cast<char>(cp));
        }
        else if (cp <= 0x7FF)
        {
            result.push_back(static_cast<char>(0xC0 | ((cp >> 6) & 0x1F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0xFFFF)
        {
            result.push_back(static_cast<char>(0xE0 | ((cp >> 12) & 0x0F)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
        else if (cp <= 0x10FFFF)
        {
            result.push_back(static_cast<char>(0xF0 | ((cp >> 18) & 0x07)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
            result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
        }
    }
    return result;
}

utf8_iterator::utf8_iterator(const char *start, const char *end) noexcept : m_ptr(start), m_end(end)
{
    if (m_ptr != m_end)
    {
        decode_current();
    }
}

utf8_iterator &utf8_iterator::operator++() noexcept
{
    if (m_ptr == m_end)
        return *this;

    usize len = utf8_char_length(static_cast<u8>(*m_ptr));
    m_ptr += len;

    if (m_ptr > m_end)
    {
        m_ptr = m_end;
    }

    if (m_ptr != m_end)
    {
        decode_current();
    }
    else
    {
        m_current = 0;
    }

    return *this;
}

void utf8_iterator::decode_current() noexcept
{
    u8 first_byte = static_cast<u8>(*m_ptr);
    usize len = utf8_char_length(first_byte);

    if (len == 1)
    {
        m_current = first_byte;
    }
    else if (len == 2 && m_ptr + 1 < m_end)
    {
        m_current = ((first_byte & 0x1F) << 6) | (static_cast<u8>(m_ptr[1]) & 0x3F);
    }
    else if (len == 3 && m_ptr + 2 < m_end)
    {
        m_current = ((first_byte & 0x0F) << 12) | ((static_cast<u8>(m_ptr[1]) & 0x3F) << 6) |
                    (static_cast<u8>(m_ptr[2]) & 0x3F);
    }
    else if (len == 4 && m_ptr + 3 < m_end)
    {
        m_current = ((first_byte & 0x07) << 18) | ((static_cast<u8>(m_ptr[1]) & 0x3F) << 12) |
                    ((static_cast<u8>(m_ptr[2]) & 0x3F) << 6) | (static_cast<u8>(m_ptr[3]) & 0x3F);
    }
    else
    {
        m_current = 0;
    }
}

} // namespace vx
