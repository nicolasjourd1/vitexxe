#include <catch2/catch_test_macros.hpp>

#include "vitexxe/application.hpp"

TEST_CASE("window_config - sane defaults", "[application]")
{
    const vx::window_config config;

    REQUIRE(config.width > 0);
    REQUIRE(config.height > 0);
    REQUIRE(config.target_fps > 0);
    REQUIRE(config.title == "Vitexxe");
}
