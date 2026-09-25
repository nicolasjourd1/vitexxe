#include "vxcore/diagnostics.hpp"
#include "vxlayout/engine.hpp"
#include "vxlayout/layout_context.hpp"
#include "vxmodel/ast.hpp"
#include "vxmodel/document.hpp"
#include "vxrender/renderer.hpp"
#include <raylib.h>

class raylib_measurer : public vx::layout::font_measurer
{
  public:
    raylib_measurer(Font font, float size) : m_font(font), m_size(size)
    {
    }

    vx::layout::text_measure measure(std::string_view text, vx::f32 scale = 1.0f) const override
    {
        std::string t(text);
        Vector2 dim = MeasureTextEx(m_font, t.c_str(), m_size * scale, 1.0f);

        return {dim.x, dim.y * 0.8f, dim.y * 0.2f};
    }

  private:
    Font m_font;
    float m_size;
};

int main()
{
    InitWindow(1080, 720, "Vitexxe");
    SetTargetFPS(60);

    Font font = GetFontDefault();
    float font_size = 30.0f;

    raylib_measurer measurer(font, font_size);
    vx::layout::layout_config config{
        .page_width = 800.f, .line_spacing = 1.5f, .math_axis_height = font_size * 0.3f};
    vx::layout::layout_engine engine(measurer, config);
    vx::render::box_renderer renderer(font, font_size);

    vx::model::document doc;
    vx::model::cursor_pos cursor{0, 0, 0};

    doc.insert_text(cursor, "Vitexxe Editor v0.1.0");

    vx::model::math_display_block math_block;
    math_block.content = "\\frac{a + b}{x^2}";
    doc.append_block(math_block);

    while (!WindowShouldClose())
    {
        vx::layout::box_node root_layout = engine.build_document(doc);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        renderer.draw(root_layout, 50.0f, 50.0f);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
