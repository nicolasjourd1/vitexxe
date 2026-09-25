#pragma once

#include "vxcore/types.hpp"
#include <string_view>

namespace vx::layout
{

struct text_measure
{
    f32 width;
    f32 height;
    f32 depth;
};

/// @brief abstract interface for measuring text
class font_measurer
{
  public:
    virtual ~font_measurer() = default;
    virtual text_measure measure(std::string_view text, f32 scale = 1.0f) const = 0;
};

/// @brief Global config for layout engine
struct layout_config
{
    f32 page_width{800.0f};
    f32 line_spacing{1.5f};
    f32 math_axis_height{5.0f}; // fraction bar height relative to baseline
};

} // namespace vx::layout
