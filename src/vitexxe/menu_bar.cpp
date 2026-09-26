#include "menu_bar.hpp"

#include <raygui.h>
#include <raylib.h>

namespace vx::ui
{

menu_bar::menu_bar(std::vector<std::string> label_keys) : m_label_keys(std::move(label_keys))
{
}

float menu_bar::draw(float window_width, const Font &font, const i18n::translator &translator) const
{
    const float font_size = static_cast<float>(GuiGetStyle(DEFAULT, TEXT_SIZE));
    const float bar_height = font_size + 2.0f * m_vertical_padding;

    DrawRectangle(0, 0, static_cast<int>(window_width), static_cast<int>(bar_height),
                  GetColor(static_cast<unsigned int>(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))));
    DrawLine(0, static_cast<int>(bar_height), static_cast<int>(window_width),
             static_cast<int>(bar_height),
             GetColor(static_cast<unsigned int>(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL))));

    float x = 0;
    for (const auto &key : m_label_keys)
    {
        const std::string label = translator.tr(key);
        const Vector2 text_size = MeasureTextEx(font, label.c_str(), font_size, 1.0f);
        const float item_width = text_size.x + 2.0f * m_padding_x;

        const Rectangle bounds{x, 0.0f, item_width, bar_height};
        GuiButton(bounds, label.c_str());

        x += item_width;
    }

    return bar_height;
}

} // namespace vx::ui
