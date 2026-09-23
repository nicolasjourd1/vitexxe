#pragma once

#include "ast.hpp"
#include "math_input.hpp"
#include <span>

namespace vx::model
{

struct cursor_pos
{
    /// Block index in document
    usize block_idx{0};

    /// Inline node index in block
    usize inline_idx{0};

    /// Character position in inline node
    usize offset_chars{0};

    bool operator==(const cursor_pos &) const = default;
};

class document
{
  public:
    document() = default;

    void append_block(block_node block);
    void insert_text(cursor_pos &cursor, std::string_view text);

    /// @return true if the symbol is replaced
    bool handle_tabulation(cursor_pos &cursor, const math_input_handler &handler);

    void split_block_at_cursor(cursor_pos &cursor);
    bool delete_backward(cursor_pos &cursor);

    std::span<const block_node> get_blocks() const noexcept
    {
        return m_blocks;
    }
    block_node &get_mutable_block(usize idx);

  private:
    std::vector<block_node> m_blocks;
};

} // namespace vx::model
