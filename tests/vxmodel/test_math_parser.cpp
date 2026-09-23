#include "vxmodel/math_ast.hpp"
#include "vxmodel/math_parser.hpp"
#include <catch2/catch_test_macros.hpp>
#include <variant>

TEST_CASE("Math parser AST generation", "[model][math_parser]")
{
    SECTION("Parse simple symbols")
    {
        auto ast = vx::model::math_parser::parse("\\alpha + 2");

        REQUIRE(std::holds_alternative<vx::model::math_row>(ast.data));
        auto &row = std::get<vx::model::math_row>(ast.data);
        REQUIRE(row.elements.size() == 3);

        CHECK(std::get<vx::model::math_symbol>(row.elements[0].data).value == "\\alpha");
        CHECK(std::get<vx::model::math_symbol>(row.elements[1].data).value == "+");
        CHECK(std::get<vx::model::math_symbol>(row.elements[2].data).value == "2");
    }

    SECTION("Parse fractions")
    {
        auto ast = vx::model::math_parser::parse("\\frac{a}{b}");

        REQUIRE(std::holds_alternative<vx::model::math_fraction>(ast.data));
        auto &frac = std::get<vx::model::math_fraction>(ast.data);

        auto &num = std::get<vx::model::math_symbol>(frac.numerator->data);
        auto &den = std::get<vx::model::math_symbol>(frac.denominator->data);

        CHECK(num.value == "a");
        CHECK(den.value == "b");
    }

    SECTION("Parse scripts")
    {
        auto ast = vx::model::math_parser::parse("x_1^2");

        REQUIRE(std::holds_alternative<vx::model::math_script>(ast.data));
        auto &script = std::get<vx::model::math_script>(ast.data);

        REQUIRE(script.base != nullptr);
        REQUIRE(script.subscript != nullptr);
        REQUIRE(script.superscript != nullptr);

        CHECK(std::get<vx::model::math_symbol>(script.base->data).value == "x");
        CHECK(std::get<vx::model::math_symbol>(script.subscript->data).value == "1");
        CHECK(std::get<vx::model::math_symbol>(script.superscript->data).value == "2");
    }
}
