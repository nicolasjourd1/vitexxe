#include "vxcore/utf8.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("UTF-8 decoding, encoding and iterator", "[core][unicode]")
{
    SECTION("ASCII and multibyte UTF-8 conversion")
    {
        std::string original = "abcd α∑é";
        auto utf32 = vx::utf8_to_utf32(original);

        // a, b, c, d, <space>, alpha, sum, e-acute
        REQUIRE(utf32.size() == 8);
        CHECK(utf32[0] == U'a');
        CHECK(utf32[5] == U'α');
        CHECK(utf32[6] == U'∑');

        std::string converted = vx::utf32_to_utf8(utf32);
        CHECK(converted == original);
    }

    SECTION("utf8_iterator traversal")
    {
        std::string text = "aα";
        vx::utf8_iterator it(text.data(), text.data() + text.length());
        vx::utf8_iterator end(text.data() + text.length(), text.data() + text.length());

        REQUIRE(it != end);
        CHECK(*it == U'a');
        ++it;
        REQUIRE(it != end);
        CHECK(*it == U'α');
        ++it;
        CHECK(it == end);
    }
}
