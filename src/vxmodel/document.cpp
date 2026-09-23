#include "document.hpp"
#include "vxcore/diagnostics.hpp"
#include "vxcore/utf8.hpp"
#include "vxmodel/ast.hpp"
#include "vxmodel/math_input.hpp"

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

bool document::handle_tabulation(cursor_pos &cursor, const math_input_handler &handler)
{
    if (m_blocks.empty() || cursor.block_idx >= m_blocks.size())
        return false;

    bool replaced = false;
    block_node &current_block = get_mutable_block(cursor.block_idx);

    std::visit(
        [&](auto &block) {
            using T = std::decay_t<decltype(block)>;
            std::string *content_ptr = nullptr;

            if constexpr (std::is_same_v<T, paragraph_block> || std::is_same_v<T, heading_block>)
            {
                if (cursor.inline_idx < block.children.size())
                {
                    auto &current_inline = block.children[cursor.inline_idx];

                    if (auto *math = std::get_if<math_inline>(&current_inline))
                    {
                        content_ptr = &math->content;
                    }
                    // NOTE: if tabulation cycles are added to text later
                    // else if (auto *txt = std::get_if<text_inline>(&current_inline))
                    // {
                    //     content_ptr = &txt->content;
                    // }
                }
            }
            else if constexpr (std::is_same_v<T, math_display_block>)
            {
                content_ptr = &block.content;
            }

            if (!content_ptr || content_ptr->empty())
                return;

            std::string &content = *content_ptr;

            // search for previous delimitiers
            usize slash_pos = content.rfind('\\');
            usize space_pos = content.rfind(' ');

            std::string symbol_to_check;
            usize replace_start = 0;

            if (slash_pos != std::string::npos &&
                (space_pos == std::string::npos || slash_pos > space_pos))
            {
                // i.e \foo
                replace_start = slash_pos;
                symbol_to_check = content.substr(replace_start);
            }
            else
            {
                // a char
                replace_start = content.length() - 1;
                symbol_to_check = content.substr(replace_start);
            }

            auto next_sym = handler.get_next_tab_cycle(symbol_to_check);
            if (next_sym)
            {
                auto old_utf32 = vx::utf8_to_utf32(symbol_to_check);
                auto new_utf32 = vx::utf8_to_utf32(*next_sym);

                content.replace(replace_start, std::string::npos, *next_sym);

                cursor.offset_chars -= old_utf32.size();
                cursor.offset_chars += new_utf32.size();
                replaced = true;
            }
        },
        current_block);

    return replaced;
}

void document::split_block_at_cursor(cursor_pos &cursor)
{
    if (m_blocks.empty() || cursor.block_idx > +m_blocks.size())
        return;

    block_node &current_block = get_mutable_block(cursor.block_idx);

    std::visit(
        [&](auto &block) {
            using T = std::decay_t<decltype(block)>;

            if constexpr (std::is_same_v<T, paragraph_block> || std::is_same_v<T, heading_block>)
            {
                paragraph_block new_paragraph;

                if (!block.children.empty() && cursor.inline_idx < block.children.size())
                {
                    auto &current_inline = block.children[cursor.inline_idx];

                    if (auto *txt = std::get_if<text_inline>(&current_inline))
                    {
                        // NOTE: later use ut8 iterator to find the exact byte offset
                        usize byte_offset = cursor.offset_chars;
                        if (byte_offset <= txt->content.length())
                        {
                            std::string right_part = txt->content.substr(byte_offset);
                            txt->content.erase(byte_offset);

                            new_paragraph.children.push_back(
                                text_inline{.content = std::move(right_part), .style = txt->style});
                        }
                    }
                }

                m_blocks.insert(m_blocks.begin() + cursor.block_idx + 1, new_paragraph);

                cursor.block_idx += 1;
                cursor.inline_idx = 0;
                cursor.offset_chars = 0;
            }
            else if constexpr (std::is_same_v<T, math_display_block>)
            {
                // NOTE: splitting after a math block creates an empty paragraph
                paragraph_block new_paragraph;
                new_paragraph.children.push_back(
                    text_inline{.content = "", .style = text_style::normal});
                m_blocks.insert(m_blocks.begin() + cursor.block_idx + 1, new_paragraph);

                cursor.block_idx += 1;
                cursor.inline_idx = 0;
                cursor.offset_chars = 0;
            }
        },
        current_block);
}

bool document::delete_backward(cursor_pos &cursor)
{
    if (m_blocks.empty())
        return false;

    // 1 : delete char
    if (cursor.offset_chars > 0)
    {
        block_node &current_block = get_mutable_block(cursor.block_idx);

        std::visit(
            [&](auto &block) {
                using T = std::decay_t<decltype(block)>;
                if constexpr (std::is_same_v<T, paragraph_block>)
                {
                    if (auto *txt = std::get_if<text_inline>(&block.children[cursor.inline_idx]))
                    {
                        txt->content.erase(cursor.offset_chars - 1, 1);
                        cursor.offset_chars -= 1;
                    }
                }
            },
            current_block);
        return true;
    }

    // 2 : merge with previous block
    if (cursor.block_idx > 0 && cursor.offset_chars == 0)
    {
        block_node &prev_block_node = get_mutable_block(cursor.block_idx - 1);
        block_node &current_block_node = get_mutable_block(cursor.block_idx);

        if (std::holds_alternative<paragraph_block>(prev_block_node) &&
            std::holds_alternative<paragraph_block>(current_block_node))
        {
            auto &prev_p = std::get<paragraph_block>(prev_block_node);
            auto &curr_p = std::get<paragraph_block>(current_block_node);

            // position cursor
            usize prev_last_inline_idx = prev_p.children.empty() ? 0 : prev_p.children.size() - 1;
            usize cursor_new_offset = 0;

            if (!prev_p.children.empty())
            {
                if (auto *txt = std::get_if<text_inline>(&prev_p.children.back()))
                {
                    cursor_new_offset = txt->content.length();
                }
            }

            // transfer children and delete curr bloc
            for (auto &child : curr_p.children)
            {
                prev_p.children.push_back(std::move(child));
            }

            m_blocks.erase(m_blocks.begin() + cursor.block_idx);

            cursor.block_idx -= 1;
            cursor.inline_idx = prev_last_inline_idx;
            cursor.offset_chars = cursor_new_offset;
            return true;
        }
    }

    return false;
}

} // namespace vx::model
