#pragma once

#include "types.hpp"

namespace vx
{

struct vec2f
{
    f32 x{0.0f};
    f32 y{0.0f};

    constexpr vec2f operator+(const vec2f &other) const noexcept
    {
        return {.x = x + other.x, .y = y + other.y};
    }

    constexpr vec2f operator-(const vec2f &other) const noexcept
    {
        return {.x = x - other.x, .y = y - other.y};
    }

    const vec2f operator*(f32 scalar) const noexcept
    {
        return {.x = x * scalar, .y = y * scalar};
    }
};

struct rectf
{
    f32 x{0.0f};
    f32 y{0.0f};
    f32 width{0.0f};
    f32 height{0.0f};

    constexpr bool contains(const vec2f &point) const noexcept
    {
        return point.x >= x && point.x <= (x + width) && point.y >= y && point.y <= y + height;
    }

    constexpr bool intersects(const rectf &other) const noexcept
    {
        return !(other.x > x + width || other.x + other.width < x || other.y > y + height ||
                 other.y + other.height < y);
    }

    constexpr vec2f get_center() const noexcept
    {
        return {.x = x + width / 2.0f, .y = y + height / 2.0f};
    }
};

} // namespace vx
