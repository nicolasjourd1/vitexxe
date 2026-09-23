#pragma once

#include "math_ast.hpp"
#include "vxcore/types.hpp"
#include <string_view>

namespace vx::model
{

class math_parser
{
  public:
    static math_node parse(std::string_view input);

  private:
    math_parser(std::string_view input);

    math_node parse_row();
    math_node parse_primary();
    math_node parse_group();

    std::string_view consume_token();
    void skip_whitespace();
    char peek() const;
    char advance();

    std::string_view m_input;
    usize m_pos{0};
};

} // namespace vx::model
