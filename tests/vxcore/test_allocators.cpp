#include "vxcore/allocators.hpp"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Arena allocator allocations", "[core][memory]")
{
    SECTION("Basic sequential allocation and reset")
    {
        vx::arena_allocator arena(1024);

        void *ptr1 = arena.allocate(10, 1);
        REQUIRE(ptr1 != nullptr);

        void *ptr2 = arena.allocate(20, 1);
        REQUIRE(ptr2 != nullptr);
        CHECK(ptr2 > ptr1);

        arena.reset();
        void *ptr3 = arena.allocate(10, 1);
        CHECK(ptr3 == ptr1);
    }

    SECTION("Alignement correctness")
    {
        vx::arena_allocator arena(1024);

        void *ptr = arena.allocate(16, 8);
        REQUIRE(ptr != nullptr);

        auto addr = reinterpret_cast<std::uintptr_t>(ptr);
        CHECK((addr % 8) == 0);
    }

    SECTION("Out of memory returns nullptr")
    {
        vx::arena_allocator arena(32);
        void *ptr1 = arena.allocate(64, 1);
        CHECK(ptr1 == nullptr);
    }
}
