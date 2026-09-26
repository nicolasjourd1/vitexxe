#pragma once

#include <string>
#include <utility>

#include "menu_bar.hpp"
#include "vitexxe/i18n.hpp"

namespace vx
{

struct window_config
{
    int width = 1280;
    int height = 800;
    int target_fps = 60;
    bool resizable = true;
    std::string title = "Vitexxe";
};

class application
{
  public:
    explicit application(window_config config = {});
    ~application();

    application(const application &) = delete;
    application &operator=(const application &) = delete;

    void run();

  private:
    void load_font();
    void update();
    void draw() const;

    window_config m_config;
    i18n::translator m_translator;
    ui::menu_bar m_menu_bar;
    Font m_font{};
};

} // namespace vx
