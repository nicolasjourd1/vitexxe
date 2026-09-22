#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace vx::model
{

class math_input_handler
{
  public:
    math_input_handler();

    std::optional<std::string_view> get_next_tab_cycle(
        std::string_view current_symbol) const noexcept;

  private:
    void register_cycle(const std::vector<std::string> &cycle);

    std::unordered_map<std::string, std::string> m_tab_cycles;
};

} // namespace vx::model
