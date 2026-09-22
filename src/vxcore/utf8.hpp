#pragma once

#include "types.hpp"
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace vx
{

constexpr usize utf8_char_length(u8 first_byte) noexcept
{
    if ((first_byte & 0x80) == 0)
        return 1;
    if ((first_byte & 0xE0) == 0xC0)
        return 2;
    if ((first_byte & 0xF0) == 0xE0)
        return 3;
    if ((first_byte & 0xF8) == 0xF0)
        return 4;
    return 1;
}

std::vector<char32_t> utf8_to_utf32(std::string_view input);
std::string utf32_to_utf8(std::span<const char32_t> input);

class utf8_iterator
{
  public:
    utf8_iterator(const char *start, const char *end) noexcept;

    utf8_iterator &operator++() noexcept;
    char32_t operator*() const noexcept
    {
        return m_current;
    };

    bool operator!=(const utf8_iterator &other) const noexcept
    {
        return m_ptr != other.m_ptr;
    }

    bool operator==(const utf8_iterator &other) const noexcept
    {
        return m_ptr == other.m_ptr;
    }

  private:
    void decode_current() noexcept;

    const char *m_ptr;
    const char *m_end;

    char32_t m_current{0};
};

} // namespace vx
