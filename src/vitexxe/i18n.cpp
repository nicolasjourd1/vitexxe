#include "i18n.hpp"

#include <array>

namespace vx::i18n
{

namespace
{

struct translation_entry
{
    std::string_view key;
    std::string_view en;
    std::string_view fr;
};

constexpr std::array<translation_entry, 7> table{
    {{"menu.file", "File", "Fichier"},
     {"menu.edit", "Edit", "Edition"},
     {"menu.view", "View", "Affichage"},
     {"menu.insert", "Insert", "Insertion"},
     {"menu.format", "Format", "Format"},
     {"menu.help", "Help", "Aide"},
     {"status.placeholder", "Document area", "Zone de document"}}};

} // namespace

translator::translator(locale current) : m_locale(current)
{
}

void translator::set_locale(locale new_locale)
{
    m_locale = new_locale;
}

locale translator::current_locale() const
{
    return m_locale;
}

std::string translator::tr(std::string_view key) const
{
    for (const auto &entry : table)
    {
        if (entry.key == key)
        {
            return std::string(m_locale == locale::fr ? entry.fr : entry.en);
        }
    }

    return std::string(key);
}

} // namespace vx::i18n
