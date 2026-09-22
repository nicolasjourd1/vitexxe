#include "diagnostics.hpp"
#include <iostream>
#include <string_view>

namespace vx
{

log_level logger::m_min_level = log_level::info;

void logger::set_level(log_level level) noexcept
{
    m_min_level = level;
}

void logger::log(log_level level, std::string_view file, int line, std::string_view msg) noexcept
{
    if (level < m_min_level)
        return;

    std::string_view level_str;
    switch (level)
    {
    case log_level::trace:
        level_str = "[TRACE]";
        break;
    case log_level::debug:
        level_str = "[DEBUG]";
        break;
    case log_level::info:
        level_str = "[INFO] ";
        break;
    case log_level::warn:
        level_str = "[WARN] ";
        break;
    case log_level::error:
        level_str = "[ERROR]";
        break;
    case log_level::fatal:
        level_str = "[FATAL]";
        break;
    }

    auto &stream = (level >= log_level::warn) ? std::cerr : std::cout;
    stream << level_str << " " << file << ":" << line << " - " << msg << '\n';
}

} // namespace vx
