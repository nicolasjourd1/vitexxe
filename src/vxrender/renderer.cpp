#include "renderer.hpp"
#include "vxlayout/box.hpp"
#include <raylib.h>
#include <type_traits>

namespace vx::render
{

box_renderer::box_renderer(Font font, float font_size) : m_font(font), m_font_size(font_size)
{
}

void box_renderer::draw(const layout::box_node &root, float start_x, float start_y) const
{
    draw_node(root, start_x, start_y);
}

void box_renderer::draw_node(const layout::box_node &node, float abs_x, float abs_y) const
{
    float current_x = abs_x + node.metrics.position.x;
    float current_y = abs_y + node.metrics.position.y;

    std::visit(
        [&](const auto &data) {
            using T = std::decay_t<decltype(data)>;

            if constexpr (std::is_same_v<T, layout::glyph_box>)
            {
                Vector2 pos = {.x = current_x, .y = current_y - node.metrics.height};
                DrawTextEx(m_font, data.text.c_str(), pos, m_font_size * node.metrics.scale, 1.0f,
                           BLACK);
            }
            else if constexpr (std::is_same_v<T, layout::fraction_box>)
            {
                if (data.numerator)
                    draw_node(*data.numerator, current_x, current_y);
                if (data.denominator)
                    draw_node(*data.denominator, current_x, current_y);
                float bar_y = current_y - 5.0f;
                Vector2 start = {.x = current_x, .y = bar_y};
                Vector2 end = {.x = current_x + node.metrics.width, .y = bar_y};
                DrawLineEx(start, end, data.bar_thickness, BLACK);
            }
            else if constexpr (std::is_same_v<T, layout::hbox> || std::is_same_v<T, layout::vbox>)
            {
                for (const auto &child : data.children)
                {
                    draw_node(child, current_x, current_y);
                }
            }
        },
        node.data);
}

} // namespace vx::render
