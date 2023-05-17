/*
 * sst-juce-guil - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-basic-blocks is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-juce-gui available at
 * https://github.com/surge-synthesizer/sst-juce-gui
 */

#ifndef INCLUDE_SST_JUCEGUI_STYLE_STYLEANDSETTINGSCONSUMER_H
#define INCLUDE_SST_JUCEGUI_STYLE_STYLEANDSETTINGSCONSUMER_H

#include <string>
#include <vector>
#include "StyleSheet.h"
#include "Settings.h"

/**
 * Pretty straight forward base classes for objects which consume stylesheets
 * and settings objects.
 */
namespace sst::jucegui::style
{
struct StyleConsumer
{
    explicit StyleConsumer(const StyleSheet::Class &c) : styleClass(c){};
    virtual ~StyleConsumer() { stylep = nullptr; };

    juce::Colour getColour(const StyleSheet::Property &p)
    {
        return style()->getColour(getStyleClass(), p);
    }

    juce::Font getFont(const StyleSheet::Property &p)
    {
        return style()->getFont(getStyleClass(), p);
    }

    // these don't belong on instances they belong on stylesheets
    StyleSheet::Class customClass{""};
    void setCustomClass(const StyleSheet::Class &sc)
    {
        customClass = sc;
        StyleSheet::extendInheritanceMap(customClass, styleClass);
    }

    const StyleSheet::Class &getStyleClass()
    {
        if (customClass.cname[0] != 0)
            return customClass;
        return styleClass;
    }

    void setStyle(const StyleSheet::ptr_t &s);
    inline StyleSheet::ptr_t style()
    {
        if (!stylep)
            stylep = StyleSheet::getBuiltInStyleSheet(StyleSheet::DARK);
        return stylep;
    }
    virtual void onStyleChanged() {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleConsumer);

  private:
    StyleSheet::ptr_t stylep;
    const StyleSheet::Class &styleClass;
};

struct SettingsConsumer
{
    SettingsConsumer() = default;
    virtual ~SettingsConsumer() = default;

    void setSettings(const Settings::ptr_t &s);
    inline Settings::ptr_t settings()
    {
        if (!settingsp)
            settingsp = std::make_shared<Settings>();
        return settingsp;
    }
    virtual void onSettingsChanged() {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsConsumer);

  private:
    Settings::ptr_t settingsp;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLEANDSETTINGSCONSUMER_H
