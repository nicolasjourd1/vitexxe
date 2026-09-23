#include "engine.hpp"
#include "vxlayout/box.hpp"
#include "vxmodel/ast.hpp"
#include "vxmodel/math_parser.hpp"
#include <algorithm>

namespace vx::layout
{

layout_engine::layout_engine(const font_measurer &measurer, layout_config config)
    : m_measurer(measurer), m_config(config)
{
}

box_node layout_engine::build_math(const model::math_node &math_ast)
{
    return std::visit(
        [this](const auto &node_data) -> box_node {
            using T = std::decay_t<decltype(node_data)>;

            if constexpr (std::is_same_v<T, model::math_symbol>)
            {
                text_measure tm = m_measurer.measure(node_data.value);
                box_node box;
                box.data = glyph_box{node_data.value};
                box.metrics = {tm.width, tm.height, tm.depth, {.x = 0.0f, .y = 0.0f}};
                return box;
            }
            else if constexpr (std::is_same_v<T, model::math_row>)
            {
                hbox row_box;
                f32 current_x = 0.0f;
                f32 max_height = 0.0f;
                f32 max_depth = 0.0f;

                for (const auto &elem : node_data.elements)
                {
                    box_node child = build_math(elem);
                    child.metrics.position.x = current_x;
                    child.metrics.position.y = 0.0f;

                    current_x += child.metrics.width;
                    max_height = std::max(max_height, child.metrics.height);
                    max_depth = std::max(max_depth, child.metrics.depth);

                    row_box.children.push_back(std::move(child));
                }

                box_node result;
                result.data = std::move(row_box);
                result.metrics = {current_x, max_height, max_depth, {.x = 0.0f, .y = 0.0f}};
                return result;
            }
            else if constexpr (std::is_same_v<T, model::math_fraction>)
            {
                box_node num = build_math(*node_data.numerator);
                box_node den = build_math(*node_data.denominator);

                // horizontal centering
                f32 max_width = std::max(num.metrics.width, den.metrics.width);
                f32 num_x = (max_width - num.metrics.width) / 2.0f;
                f32 den_x = (max_width - den.metrics.width) / 2.0f;

                // padding
                f32 gap = 2.0f;
                num.metrics.position = {.x = num_x,
                                        .y = m_config.math_axis_height + gap + num.metrics.depth};
                den.metrics.position = {.x = den_x,
                                        .y = m_config.math_axis_height - gap - den.metrics.height};

                fraction_box frac;
                frac.numerator = make_box(std::move(num));
                frac.denominator = make_box(std::move(den));
                frac.bar_thickness = 1.0f;

                box_node result;
                result.data = std::move(frac);

                result.metrics = {max_width,
                                  num.metrics.position.y + num.metrics.height,
                                  std::abs(den.metrics.position.y) + den.metrics.depth,
                                  {.x = 0.0f, .y = 0.0f}};
                return result;
            }
            else if constexpr (std::is_same_v<T, model::math_script>)
            {
                box_node base = build_math(*node_data.base);
                f32 current_x = base.metrics.width;
                f32 max_h = base.metrics.height;
                f32 max_d = base.metrics.depth;

                hbox script_box;
                script_box.children.push_back(std::move(base));

                if (node_data.superscript)
                {
                    box_node sup = build_math(*node_data.superscript);
                    sup.metrics.position = {.x = current_x, .y = max_h * 0.7f};
                    max_h = std::max(max_h, sup.metrics.position.y = sup.metrics.height);
                    script_box.children.push_back(std::move(sup));
                }
                if (node_data.subscript)
                {
                    box_node sub = build_math(*node_data.subscript);
                    sub.metrics.position = {.x = current_x,
                                            .y = -max_d * 0.7f - sub.metrics.height};
                    max_d = std::max(max_d, std::abs(sub.metrics.position.y) + sub.metrics.depth);
                    script_box.children.push_back(std::move(sub));
                }

                box_node result;
                result.data = std::move(script_box);
                result.metrics = {current_x, max_h, max_d, {.x = 0.0f, .y = 0.0f}};
                return result;
            }

            return box_node{};
        },
        math_ast.data);
}

box_node layout_engine::build_document(const model::document &doc)
{
    vbox doc_box;
    f32 current_y = 0.0f;
    f32 doc_width = 0.0f;

    for (const auto &block : doc.get_blocks())
    {
        std::visit(
            [&](const auto &block_data) {
                using T = std::decay_t<decltype(block_data)>;

                if constexpr (std::is_same_v<T, model::paragraph_block>)
                {
                    hbox line;
                    f32 current_x = 0.0f;
                    f32 max_h = 0.0f, max_d = 0.0f;

                    // TODO: later, word wrapping if current_x > m_config.page_width
                    for (const auto &inl : block_data.children)
                    {
                        if (const auto *txt = std::get_if<model::text_inline>(&inl))
                        {
                            text_measure tm = m_measurer.measure(txt->content);
                            box_node txt_box;
                            txt_box.data = glyph_box{txt->content};
                            txt_box.metrics = {
                                tm.width, tm.height, tm.depth, {.x = current_x, .y = 0.0f}};

                            current_x += tm.width;
                            max_h = std::max(max_h, tm.height);
                            max_d = std::max(max_d, tm.depth);
                            line.children.push_back(std::move(txt_box));
                        }
                    }

                    box_node p_box;
                    p_box.data = std::move(line);
                    p_box.metrics = {current_x, max_h, max_d, {.x = 0.0f, .y = current_y}};

                    current_y += (max_h + max_d) + m_config.line_spacing;
                    doc_width = std::max(doc_width, current_x);
                    doc_box.children.push_back(std::move(p_box));
                }
                else if constexpr (std::is_same_v<T, model::math_display_block>)
                {
                    model::math_node math_ast = model::math_parser::parse(block_data.content);
                    box_node math_layout = build_math(math_ast);

                    f32 center_x = (m_config.page_width - math_layout.metrics.width) / 2.0f;
                    math_layout.metrics.position = {.x = center_x,
                                                    .y = current_y + math_layout.metrics.height};

                    current_y +=
                        math_layout.metrics.height + math_layout.metrics.depth + 15.0f; // margin

                    doc_width = std::max(doc_width, math_layout.metrics.width);
                    doc_box.children.push_back(std::move(math_layout));
                }
            },
            block);
    }

    box_node result;
    result.data = std::move(doc_box);
    result.metrics = {doc_width, current_y, 0.0f, {.x = 0.0f, .y = 0.0f}};
    return result;
}

} // namespace vx::layout
