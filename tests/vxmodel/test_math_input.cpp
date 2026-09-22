#include "vxmodel/math_input.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Mathematical tab cycles", "[model][math_input]")
{
    vx::model::math_input_handler math_handler;

    SECTION("Cyclic mutation of 'a'")
    {
        auto next = math_handler.get_next_tab_cycle("a");
        REQUIRE(next.has_value());
        CHECK(next.value() == "\\alpha");

        next = math_handler.get_next_tab_cycle("\\alpha");
        REQUIRE(next.has_value());
        CHECK(next.value() == "\\forall");

        next = math_handler.get_next_tab_cycle("\\forall");
        REQUIRE(next.has_value());
        CHECK(next.value() == "A");

        next = math_handler.get_next_tab_cycle("A");
        REQUIRE(next.has_value());
        CHECK(next.value() == "a");
    }

    SECTION("Unknown symbol returns nullopt")
    {
        auto next = math_handler.get_next_tab_cycle("unknown_symbol");
        CHECK(!next.has_value());

        next = math_handler.get_next_tab_cycle("z");
        CHECK(!next.has_value());
    }

    SECTION("Operator cycling")
    {
        auto next = math_handler.get_next_tab_cycle("<");
        REQUIRE(next.has_value());
        CHECK(next.value() == "\\leq");
    }
}
