#include "vxcore/math2d.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Geometry vectors and rectangles", "[core][geometry]")
{
    SECTION("vec2f arithmetic operators")
    {
        vx::vec2f v1{.x = 1.0f, .y = 2.0f};
        vx::vec2f v2{.x = 3.0f, .y = 4.0f};

        vx::vec2f sum = v1 + v2;
        CHECK(sum.x == 4.0f);
        CHECK(sum.y == 6.0f);

        vx::vec2f scaled = v1 * 2.0f;
        CHECK(scaled.x == 2.0f);
        CHECK(scaled.y == 4.0f);
    }

    SECTION("rectf containment and intersection")
    {
        vx::rectf r1{.x = 0.0f, .y = 0.0f, .width = 10.0f, .height = 10.0f};

        CHECK(r1.contains({.x = 5.0f, .y = 5.0f}));
        CHECK(!r1.contains({.x = 15.0f, .y = 15.0f}));

        vx::rectf r2{.x = 5.0f, .y = 5.0f, .width = 10.0f, .height = 10.0f};
        vx::rectf r3{.x = 20.0f, .y = 20.0f, .width = 5.0f, .height = 5.0f};

        CHECK(r1.intersects(r2));
        CHECK(!r1.intersects(r3));

        vx::vec2f center = r1.get_center();
        CHECK(center.x == 5.0f);
        CHECK(center.y == 5.0f);
    }
}
