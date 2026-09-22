#include "vxmodel/ast.hpp"
#include "vxmodel/document.hpp"
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
