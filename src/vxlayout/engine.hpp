#pragma once

#include "box.hpp"
#include "layout_context.hpp"
#include "vxmodel/document.hpp"
#include "vxmodel/math_ast.hpp"

namespace vx::layout
{

class layout_engine
{
  public:
    layout_engine(const font_measurer &measurer, layout_config config = {});

    /// @brief generates layout for the whole document
    box_node build_document(const model::document &doc);

    /// @brief generates layout for a math ast
    box_node build_math(const model::math_node &math_ast);

  private:
    const font_measurer &m_measurer;
    layout_config m_config;
};

} // namespace vx::layout
