#include "vxlayout/box.hpp"
#include "vxlayout/engine.hpp"
#include "vxlayout/layout_context.hpp"
#include "vxmodel/document.hpp"
#include "vxmodel/math_parser.hpp"
#include <catch2/catch_test_macros.hpp>

class mock_measurer : public vx::layout::font_measurer
{
  public:
    vx::layout::text_measure measure(std::string_view text,
                                     [[maybe_unused]] vx::f32 scale = 1.0f) const override
    {
        // width : 10px, height : 12px, depth = 2px
        vx::f32 w = static_cast<vx::f32>(text.length()) * 10.0f;
        return {w, 12.0f, 2.0f};
    }
};

TEST_CASE("Layout engine calculates math dimensions", "[layout][engine]")
{
    mock_measurer measurer;
    vx::layout::layout_config config;
    config.math_axis_height = 5.0f;
    vx::layout::layout_engine engine(measurer, config);

    SECTION("Fraction layout positioning")
    {
        auto ast = vx::model::math_parser::parse("\\frac{a}{b}");
        vx::layout::box_node result = engine.build_math(ast);

        REQUIRE(std::holds_alternative<vx::layout::fraction_box>(result.data));
        auto &frac = std::get<vx::layout::fraction_box>(result.data);

        // 'a' (1 char) and 'b' (1 char) are both 10px wide
        CHECK(result.metrics.width == 10.0f);

        // 'a'
        REQUIRE(frac.numerator != nullptr);
        CHECK(frac.numerator->metrics.width == 10.0f);
        CHECK(frac.numerator->metrics.position.x == 0.0f);
        // y positioning (axis = 5.0, gap 4.0, 'a' has depth 2.0 -> -11.0 total)
        CHECK(frac.numerator->metrics.position.y == -11.0f);

        // 'b'
        REQUIRE(frac.denominator != nullptr);
        CHECK(frac.denominator->metrics.position.x == 0.0f);
        CHECK(frac.denominator->metrics.width == 10.0f);
        // y positioning (axis = 5.0, gap = 4.0, 'b' has height 12.0 -> 11.0 total)
        CHECK(frac.denominator->metrics.position.y == 11.0f);
    }

    SECTION("Row layout horizontal alignment")
    {
        auto ast = vx::model::math_parser::parse("a+b");
        vx::layout::box_node result = engine.build_math(ast);

        REQUIRE(std::holds_alternative<vx::layout::hbox>(result.data));
        auto &row = std::get<vx::layout::hbox>(result.data);

        // a, +, b = 3 elements of 1 char = 30px wide
        CHECK(result.metrics.width == 30.0f);
        REQUIRE(row.children.size() == 3);

        CHECK(row.children[0].metrics.position.x == 0.0f);
        CHECK(row.children[1].metrics.position.x == 10.0f);
        CHECK(row.children[2].metrics.position.x == 20.0f);
    }
}

TEST_CASE("Layout engine calculates document dimensions", "[layout][engine]")
{
    mock_measurer measurer;
    vx::layout::layout_config config;
    config.line_spacing = 1.5f;
    vx::layout::layout_engine engine(measurer, config);

    SECTION("Paragraph layout")
    {
        vx::model::document doc;
        vx::model::cursor_pos cursor{.block_idx = 0, .inline_idx = 0, .offset_chars = 0};
        doc.insert_text(cursor, "Hello");

        vx::layout::box_node result = engine.build_document(doc);

        REQUIRE(std::holds_alternative<vx::layout::vbox>(result.data));
        auto &v_box = std::get<vx::layout::vbox>(result.data);

        REQUIRE(v_box.children.size() == 1);
        auto &p_box = std::get<vx::layout::hbox>(v_box.children[0].data);

        // "Hello" is 5 chars -> 50px wide
        REQUIRE(p_box.children.size() == 1);
        CHECK(p_box.children[0].metrics.width == 50.0f);
        CHECK(p_box.children[0].metrics.position.x == 0.0f);

        CHECK(result.metrics.width == 50.0f);
    }
}
