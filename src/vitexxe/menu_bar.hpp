#pragma once

#include <string>
#include <vector>

namespace vx::ui
{

class menu_bar
{
  public:
    explicit menu_bar(std::vector<std::string> labels);

    float draw(float window_width) const;

  private:
    std::vector<std::string> m_labels;
    float m_padding_x = 12.0f;
    float m_vertical_padding = 8.0f;
};

} // namespace vx::ui
