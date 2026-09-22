#pragma once

#include "vxcore/types.hpp"
#include <string>
#include <variant>
#include <vector>

namespace vx::model
{

// Inline (i.e. inside a block)

enum class text_style : u8
{
    normal = 0,
    bold = 1 << 0,
    italic = 1 << 1,
    underline = 1 << 2,
    code = 1 << 3
};

constexpr text_style operator|(text_style a, text_style b) noexcept
{
    return static_cast<text_style>(static_cast<u8>(a) | static_cast<u8>(b));
}

constexpr bool has_style(text_style flags, text_style target) noexcept
{
    return (static_cast<u8>(flags) & static_cast<u8>(target)) != 0;
}

struct text_inline
{
    std::string content; // UTF-8 content
    text_style style{text_style::normal};
};

struct math_inline
{
    std::string content; // maths stuff e.g "\\alpha + \\beta";
};

using inline_node = std::variant<text_inline, math_inline>;

// Block (i.e document structure)

struct paragraph_block
{
    std::vector<inline_node> children;
};

struct heading_block
{
    u8 m_level{1}; // 1 = H1, 2 = H2, ...
    std::vector<inline_node> children;
};

struct math_display_block
{
    std::string content; // like math_inline but centered
};

using block_node = std::variant<paragraph_block, heading_block, math_display_block>;

} // namespace vx::model
