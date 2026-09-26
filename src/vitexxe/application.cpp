#include "application.hpp"
#include "vitexxe/menu_bar.hpp"

#include <raygui.h>
#include <raylib.h>

namespace vx
{

namespace
{
constexpr int k_font_size = 20;
constexpr const char *k_font_path = "assets/fonts/Inter_18pt-Regular.ttf";

ui::menu_bar make_main_menu_bar()
{
    return ui::menu_bar(
        {"menu.file", "menu.edit", "menu.view", "menu.insert", "menu.format", "menu.help"});
}

std::vector<int> build_codepoints()
{
    std::vector<int> codepoints;

    for (int c = 32; c <= 126; ++c)
    {
        codepoints.push_back(c);
    }
    for (int c = 0xC0; c <= 0xFF; ++c)
    { // accents
        codepoints.push_back(c);
    }

    return codepoints;
}

} // namespace

application::application(window_config config)
    : m_config(std::move(config)), m_menu_bar(make_main_menu_bar())
{
    if (m_config.resizable)
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    }

    InitWindow(m_config.width, m_config.height, m_config.title.c_str());
    SetTargetFPS(m_config.target_fps);

    load_font();
}

application::~application()
{
    UnloadFont(m_font);
    CloseWindow();
}

void application::load_font()
{
    std::vector<int> codepoints = build_codepoints();

    m_font = LoadFontEx(k_font_path, k_font_size, codepoints.data(),
                        static_cast<int>(codepoints.size()));

    if (!IsFontValid(m_font))
    {
        TraceLog(LOG_WARNING, "Cant find font (%s). Using default font.", k_font_path);
        m_font = GetFontDefault();
    }
    else
    {
        SetTextureFilter(m_font.texture, TEXTURE_FILTER_BILINEAR);
    }

    GuiSetFont(m_font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, k_font_size);
}

void application::run()
{
    while (!WindowShouldClose())
    {
        update();

        BeginDrawing();
        draw();
        EndDrawing();
    }
}

void application::update()
{
    // TODO
}

void application::draw() const
{
    ClearBackground(RAYWHITE);

    const auto window_width = static_cast<float>(GetScreenWidth());
    const auto window_height = static_cast<float>(GetScreenHeight());

    const float menu_height = m_menu_bar.draw(window_width, m_font, m_translator);

    const std::string placeholder = m_translator.tr("status.placeholder");
    DrawText(placeholder.c_str(), 20, static_cast<int>(menu_height) + 20, 20, DARKGRAY);

    const int status_height = 24;
    const int status_y = static_cast<int>(window_height) - status_height;

    DrawRectangle(0, status_y, static_cast<int>(window_width), status_height,
                  GetColor(static_cast<unsigned int>(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))));
    DrawText(TextFormat("%dx%d", GetScreenWidth(), GetScreenHeight()), 10, status_y + 4, 16, GRAY);
}

} // namespace vx
