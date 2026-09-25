#include "application.hpp"

#include <raygui.h>
#include <raylib.h>

namespace vx
{

application::application(window_config config) : m_config(std::move(config))
{
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

    DrawText("Vitexxe", 20, 20, 20, DARKGRAY);
    DrawText("Main window", 20, 60, 18, GRAY);

    GuiLabel({20.0f, 100.0f, 340.0f, 24.0f}, "with raygui !");
}

} // namespace vx
