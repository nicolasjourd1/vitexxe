#pragma once

#include "vxcore/math2d.hpp"
#include "vxcore/types.hpp"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace vx::layout
{

struct box_metrics
{
    f32 width{0.0f};
    f32 height{0.0f};
    f32 depth{0.0f};
    vec2f position{.x = 0.0f, .y = 0.0f}; // relative to parent
};

struct box_node;

//

struct glyph_box
{
    std::string text;
};

struct fraction_box
{
    std::unique_ptr<box_node> numerator;
    std::unique_ptr<box_node> denominator;
    f32 bar_thickness{1.0f};
};

struct hbox
{
    std::vector<box_node> children;
};

struct vbox
{
    std::vector<box_node> children;
};

//

struct box_node
{
    std::variant<glyph_box, fraction_box, hbox, vbox> data;
    box_metrics metrics;
};

inline std::unique_ptr<box_node> make_box(box_node node)
{
    return std::make_unique<box_node>(std::move(node));
}

} // namespace vx::layout
