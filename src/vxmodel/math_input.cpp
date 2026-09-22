#include "math_input.hpp"
#include "vxcore/types.hpp"

namespace vx::model
{

math_input_handler::math_input_handler()
{
    register_cycle({"a", "\\alpha", "\\forall", "A"});
    register_cycle({"b", "\\beta", "B"});
    register_cycle({"p", "\\pi", "\\prod", "P", "\\Phi"});
    register_cycle({"-", "_", "\\ominus"});
    register_cycle({"*", "\\times", "\\otimes", "\\codt"});
    register_cycle({"<", "\\leq", "\\ll", "\\leftarrow"});
    register_cycle({">", "\\geq", "\\gg", "\\rightarrow"});
}

void math_input_handler::register_cycle(const std::vector<std::string> &cycle)
{
    if (cycle.size() < 2)
        return;

    for (usize i = 0; i < cycle.size(); ++i)
    {
        const std::string &current = cycle[i];
        const std::string &next = cycle[(i + 1) % cycle.size()];

        m_tab_cycles[current] = next;
    }
}

std::optional<std::string_view> math_input_handler::get_next_tab_cycle(
    std::string_view current_symbol) const noexcept
{
    auto it = m_tab_cycles.find(std::string(current_symbol));

    if (it != m_tab_cycles.end())
    {
        return it->second;
    }

    return std::nullopt;
}

} // namespace vx::model
