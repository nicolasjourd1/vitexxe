#include "application.hpp"
#include "vitexxe/menu_bar.hpp"

#include <raygui.h>
#include <raylib.h>

namespace vx
{

ui::menu_bar make_main_menu_bar()
{
    return ui::menu_bar({"File", "Edit", "View", "Insert", "Format", "Help"});
}

application::application(window_config config)
    : m_config(std::move(config)), m_menu_bar(make_main_menu_bar())
{
    if (m_config.resizable)
    {
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    }

    InitWindow(m_config.width, m_config.height, m_config.title.c_str());
    SetTargetFPS(m_config.target_fps);
}

application::~application()
{
    CloseWindow();
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

    const float menu_height = m_menu_bar.draw(window_width);

    const char *placeholder = "Document zone";
    DrawText(placeholder, 20, static_cast<int>(menu_height) + 20, 20, DARKGRAY);

    const int status_height = 24;
    const int status_y = static_cast<int>(window_height) - status_height;

    DrawRectangle(0, status_y, static_cast<int>(window_width), status_height,
                  GetColor(static_cast<unsigned int>(GuiGetStyle(DEFAULT, BACKGROUND_COLOR))));
    DrawText(TextFormat("%dx%d", GetScreenWidth(), GetScreenHeight()), 10, status_y + 4, 16, GRAY);
}

} // namespace vx
