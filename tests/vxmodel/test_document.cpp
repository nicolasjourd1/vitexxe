#include "vxmodel/ast.hpp"
#include "vxmodel/document.hpp"
#include "vxmodel/math_input.hpp"
#include <catch2/catch_test_macros.hpp>
#include <variant>

TEST_CASE("AST nodes construction", "[model][ast]")
{
    SECTION("Block variants initialization")
    {
        vx::model::paragraph_block p;
        p.children.push_back(
            vx::model::text_inline{.content = "Hello !", .style = vx::model::text_style::bold});
        p.children.push_back(vx::model::math_inline{.content = "\\alpha"});

        vx::model::block_node node = p;

        REQUIRE(std::holds_alternative<vx::model::paragraph_block>(node));

        auto &p_ref = std::get<vx::model::paragraph_block>(node);
        CHECK(p_ref.children.size() == 2);
    }
}

TEST_CASE("Document insertion", "[model][document]")
{
    SECTION("Insert text into empty document creates paragraph")
    {
        vx::model::document doc;
        vx::model::cursor_pos cursor{.block_idx = 0, .inline_idx = 0, .offset_chars = 0};

        doc.insert_text(cursor, "Vitexxe");

        REQUIRE(doc.get_blocks().size() == 1);
        REQUIRE(std::holds_alternative<vx::model::paragraph_block>(doc.get_blocks()[0]));

        CHECK(cursor.offset_chars == 7);
    }
}

TEST_CASE("Document math tabulation", "[model][document]")
{
    SECTION("Tabulation cycles updates AST and cursor")
    {
        vx::model::document doc;
        vx::model::cursor_pos cursor{.block_idx = 0, .inline_idx = 0, .offset_chars = 0};
        vx::model::math_input_handler math_handler;

        vx::model::math_display_block math_block;
        doc.append_block(math_block);

        doc.insert_text(cursor, "a");
        CHECK(cursor.offset_chars == 1);

        bool tab1 = doc.handle_tabulation(cursor, math_handler);
        REQUIRE(tab1 == true);

        auto blocks = doc.get_blocks();
        auto &mb1 = std::get<vx::model::math_display_block>(blocks[0]);
        CHECK(mb1.content == "\\alpha");
        CHECK(cursor.offset_chars == 6); // '\', 'a', 'l', 'p', 'h', 'a'

        bool tab2 = doc.handle_tabulation(cursor, math_handler);
        REQUIRE(tab2 == true);

        blocks = doc.get_blocks();
        auto &mb2 = std::get<vx::model::math_display_block>(blocks[0]);
        CHECK(mb2.content == "\\forall");
        CHECK(cursor.offset_chars == 7); // '\', 'f', 'o', 'r', 'a', 'l', 'l'
    }
}

TEST_CASE("Document split and merge", "[model][document]")
{
    SECTION("Splitting a block creates a new paragraph")
    {
        vx::model::document doc;
        vx::model::cursor_pos cursor{.block_idx = 0, .inline_idx = 0, .offset_chars = 0};

        doc.insert_text(cursor, "Hello World");
        cursor.offset_chars = 6;
        doc.split_block_at_cursor(cursor);

        auto blocks = doc.get_blocks();
        REQUIRE(blocks.size() == 2);

        auto &p1 = std::get<vx::model::paragraph_block>(blocks[0]);
        auto &p2 = std::get<vx::model::paragraph_block>(blocks[1]);

        auto &txt1 = std::get<vx::model::text_inline>(p1.children[0]);
        auto &txt2 = std::get<vx::model::text_inline>(p2.children[0]);

        CHECK(txt1.content == "Hello ");
        CHECK(txt2.content == "World");

        CHECK(cursor.block_idx == 1);
        CHECK(cursor.offset_chars == 0);
    }

    SECTION("Merging a block backspaces into previous")
    {
        vx::model::document doc;
        vx::model::cursor_pos cursor{.block_idx = 0, .inline_idx = 0, .offset_chars = 0};

        doc.insert_text(cursor, "Hello ");
        doc.split_block_at_cursor(cursor);
        doc.insert_text(cursor, "World");

        REQUIRE(doc.get_blocks().size() == 2);

        cursor.offset_chars = 0;

        bool merged = doc.delete_backward(cursor);
        REQUIRE(merged == true);

        auto blocks = doc.get_blocks();
        REQUIRE(blocks.size() == 1);

        CHECK(cursor.block_idx == 0);
        CHECK(cursor.offset_chars == 6);
    }
}
