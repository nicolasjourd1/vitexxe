#pragma once

#include "vxlayout/box.hpp"
#include <raylib.h>

namespace vx::render
{

class box_renderer
{
  public:
    box_renderer(Font font, float font_size);

    void draw(const layout::box_node &root, float start_x, float start_y) const;

  private:
    void draw_node(const layout::box_node &node, float abs_x, float abs_y) const;

    Font m_font;
    float m_font_size;
};

} // namespace vx::render
