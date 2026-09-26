#pragma once

#include <string>
#include <vector>

#include <raylib.h>

#include "i18n.hpp"

namespace vx::ui
{

class menu_bar
{
  public:
    explicit menu_bar(std::vector<std::string> label_keys);

    float draw(float window_width, const Font &font, const i18n::translator &translator) const;

  private:
    std::vector<std::string> m_label_keys;
    float m_padding_x = 12.0f;
    float m_vertical_padding = 8.0f;
};

} // namespace vx::ui
