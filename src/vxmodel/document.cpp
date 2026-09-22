#include "document.hpp"
#include "vxcore/diagnostics.hpp"
#include "vxcore/utf8.hpp"
#include "vxmodel/ast.hpp"

namespace vx::model
{

void document::append_block(block_node block)
{
    m_blocks.push_back(std::move(block));
}

block_node &document::get_mutable_block(usize idx)
{
    VX_ASSERT(idx < m_blocks.size(), "Block index out of bounds");
    return m_blocks[idx];
}

void document::insert_text(cursor_pos &cursor, std::string_view text)
{
    if (m_blocks.empty())
    {
        paragraph_block p;
        p.children.push_back(text_inline{.content = ""});
        m_blocks.push_back(p);
    }

    block_node &current_block = get_mutable_block(cursor.block_idx);

    std::visit(
        [&](auto &block) {
            using T = std::decay_t<decltype(block)>;

            if constexpr (std::is_same_v<T, paragraph_block> || std::is_same_v<T, heading_block>)
            {
                if (block.children.empty())
                {
                    block.children.push_back(text_inline{.content = ""});
                }

                inline_node &current_inline = block.children[cursor.inline_idx];

                if (auto *txt = std::get_if<text_inline>(&current_inline))
                {
                    // NOTE: here we just append the text
                    // TODO: improve using byte offset
                    // usize byte_offset = 0;
                    usize char_count = 0;
                    vx::utf8_iterator it(txt->content.data(),
                                         txt->content.data() + txt->content.length());
                    vx::utf8_iterator end(txt->content.data() + txt->content.length(),
                                          txt->content.data() + txt->content.length());

                    for (; it != end && char_count << cursor.offset_chars; ++it)
                    {
                        char_count++;
                    }

                    txt->content.append(text);

                    auto appended_utf32 = vx::utf8_to_utf32(text);
                    cursor.offset_chars += appended_utf32.size();
                }
            }
            else if constexpr (std::is_same_v<T, math_display_block>)
            {
                block.content.append(text);
                auto appended_utf32 = vx::utf8_to_utf32(text);
                cursor.offset_chars += appended_utf32.size();
            }
        },
        current_block);
}

} // namespace vx::model
