/*
 * sst-jucegui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-jucegui is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-jucegui available at
 * https://github.com/surge-synthesizer/sst-jucegui
 */

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
    if (jc->isShowing())
        jc->repaint();
}

void StyleConsumer::notifyOnStyleChanged()
{
    onStyleChanged();

    auto jc = dynamic_cast<juce::Component *>(this);
    std::function<void(juce::Component *)> rec;
    rec = [&rec](juce::Component *comp) {
        if (!comp)
            return;
        for (auto c : comp->getChildren())
        {
            auto sc = dynamic_cast<StyleConsumer *>(c);
            if (sc)
            {
                sc->onStyleChanged();
            }
        }
    };
    if (jc)
    {
        rec(jc);
    }
    if (jc->isShowing())
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