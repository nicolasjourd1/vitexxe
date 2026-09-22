#pragma once

#include "types.hpp"
#include <cstdlib>
#include <expected>
#include <string>
#include <string_view>

namespace vx
{

enum class log_level
{
    trace,
    debug,
    info,
    warn,
    error,
    fatal
};

struct error
{
    i32 code;
    std::string message;
};

template <typename T> using result = std::expected<T, error>;

class logger
{
  public:
    static void set_level(log_level level) noexcept;
    static void log(log_level level, std::string_view file, int line,
                    std::string_view msg) noexcept;

  private:
    static log_level m_min_level;
};

} // namespace vx

#ifndef NDEBUG
#define VX_ASSERT(cond, msg)                                                                       \
    do                                                                                             \
    {                                                                                              \
        if (!(cond))                                                                               \
        {                                                                                          \
            ::vx::logger::log(::vx::log_level::fatal, __FILE__, __LINE__, msg);                    \
            std::abort();                                                                          \
        }                                                                                          \
    } while (false)
#else
#define VX_ASSERT(cond, msg)                                                                       \
    do                                                                                             \
    {                                                                                              \
        (void)sizeof(cond);                                                                        \
    } while (false)
#endif

#define VX_PANIC(msg)                                                                              \
    do                                                                                             \
    {                                                                                              \
        ::vx::logger::log(::vx::log_level::fatal, __FILE__, __LINE__, msg);                        \
        std::abort();                                                                              \
    } while (false)
