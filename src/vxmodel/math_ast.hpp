#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace vx::model
{

struct math_node;

//

struct math_symbol
{
    std::string value; // "a", "\\alpha", "+", ...
};

struct math_fraction
{
    std::unique_ptr<math_node> numerator;
    std::unique_ptr<math_node> denominator;
};

struct math_script
{
    std::unique_ptr<math_node> base;
    std::unique_ptr<math_node> subscript;
    std::unique_ptr<math_node> superscript;
};

struct math_row
{
    std::vector<math_node> elements; // e.g a + b
};

//

struct math_node
{
    std::variant<math_symbol, math_fraction, math_script, math_row> data;
};

inline std::unique_ptr<math_node> make_math_node(math_node node)
{
    return std::make_unique<math_node>(std::move(node));
}

} // namespace vx::model
