#include "vxcore/diagnostics.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Result propagation and diagnostics", "[core][diagnostics]")
{
    SECTION("Valid result holds value")
    {
        vx::result<int> res = 67;
        REQUIRE(res.has_value());
        CHECK(res.value() == 67);
    }

    SECTION("Error result holds code and message")
    {
        vx::result<int> res = std::unexpected(vx::error{404, "Not found"});
        REQUIRE(!res.has_value());
        CHECK(res.error().code == 404);
        CHECK(res.error().message == "Not found");
    }
}
