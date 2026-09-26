#pragma once

#include <string>
#include <string_view>

namespace vx::i18n
{

enum class locale
{
    en,
    fr,
};

class translator
{
  public:
    explicit translator(locale current = locale::en);

    void set_locale(locale new_locale);
    locale current_locale() const;

    std::string tr(std::string_view key) const;

  private:
    locale m_locale;
};

} // namespace vx::i18n
