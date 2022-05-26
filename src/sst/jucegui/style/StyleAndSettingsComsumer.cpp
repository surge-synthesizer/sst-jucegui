//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>

namespace sst::jucegui::style
{
void StyleConsumer::setStyle(const StyleSheet::ptr_t &s)
{
    stylep = s;
    onStyleChanged();

    auto jc = dynamic_cast<juce::Component *>(this);
    std::function<void(juce::Component *)> rec;
    rec = [&rec, s](juce::Component *comp) {
        if (!comp)
            return;
        for (auto c : comp->getChildren())
        {
            auto sc = dynamic_cast<StyleConsumer *>(c);
            if (sc)
            {
                sc->setStyle(s);
            }
            else
            {
                rec(c);
            }
        }
    };
    if (jc)
    {
        rec(jc);
    }
    jc->repaint();
}

void SettingsConsumer::setSettings(const Settings::ptr_t &s)
{
    settingsp = s;
    onSettingsChanged();

    auto jc = dynamic_cast<juce::Component *>(this);
    std::function<void(juce::Component *)> rec;
    rec = [&rec, s](juce::Component *comp) {
        if (!comp)
            return;
        for (auto c : comp->getChildren())
        {
            auto sc = dynamic_cast<SettingsConsumer *>(c);
            if (sc)
            {
                sc->setSettings(s);
            }
            else
            {
                rec(c);
            }
        }
    };
    if (jc)
    {
        rec(jc);
    }
    jc->repaint();
}
} // namespace sst::jucegui::style