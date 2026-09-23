#include "math_parser.hpp"
#include "vxmodel/math_ast.hpp"
#include <cctype>

namespace vx::model
{

math_node math_parser::parse(std::string_view input)
{
    math_parser parser(input);
    return parser.parse_row();
}

math_parser::math_parser(std::string_view input) : m_input(input)
{
}

void math_parser::skip_whitespace()
{
    while (m_pos < m_input.size() && std::isspace(m_input[m_pos]))
    {
        m_pos++;
    }
}

char math_parser::peek() const
{
    return m_pos < m_input.size() ? m_input[m_pos] : '\0';
}

char math_parser::advance()
{
    return m_pos < m_input.size() ? m_input[m_pos++] : '\0';
}

std::string_view math_parser::consume_token()
{
    skip_whitespace();
    if (m_pos >= m_input.size())
        return {};

    usize start = m_pos;
    char c = advance();

    // LaTeX command
    if (c == '\\')
    {
        while (m_pos < m_input.size() && std::isalpha(m_input[m_pos]))
        {
            m_pos++;
        }
    }
    // Number
    else if (std::isdigit(c))
    {
        while (m_pos < m_input.size() && std::isdigit(m_input[m_pos]))
        {
            m_pos++;
        }
    }
    // else => single character

    return m_input.substr(start, m_pos - start);
}

math_node math_parser::parse_group()
{
    skip_whitespace();
    // consume { ... }
    if (peek() == '{')
    {
        advance();
        math_node row = parse_row();
        if (peek() == '}')
            advance();
        return row;
    }

    // if no { ... }, just parse an element
    return parse_primary();
}

math_node math_parser::parse_primary()
{
    skip_whitespace();
    if (m_pos >= m_input.size())
        return math_node{math_row{}};

    if (peek() == '{')
    {
        return parse_group();
    }

    std::string_view token = consume_token();

    if (token == "\\frac")
    {
        math_fraction frac;
        frac.numerator = make_math_node(parse_group());
        frac.denominator = make_math_node(parse_group());
        return math_node{std::move(frac)};
    }

    return math_node{math_symbol{std::string(token)}};
}

math_node math_parser::parse_row()
{
    math_row row;

    while (m_pos < m_input.size())
    {
        skip_whitespace();
        char next = peek();

        if (next == '}')
            break;

        if (next == '^' || next == '_')
        {
            if (row.elements.empty())
            {
                row.elements.push_back(math_node{math_symbol{""}});
            }

            math_node base = std::move(row.elements.back());
            row.elements.pop_back();

            math_script script;
            script.base = make_math_node(std::move(base));

            while (peek() == '^' || peek() == '_')
            {
                char op = advance();
                if (op == '^')
                    script.superscript = make_math_node(parse_group());
                if (op == '_')
                    script.subscript = make_math_node(parse_group());
            }

            row.elements.push_back(math_node{std::move(script)});
        }
        else
        {
            row.elements.push_back(parse_primary());
        }
    }

    if (row.elements.size() == 1)
    {
        return std::move(row.elements.front());
    }

    return math_node{std::move(row)};
}

} // namespace vx::model
