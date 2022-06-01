//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_STYLEANDSETTINGSCONSUMER_H
#define SST_JUCEGUI_STYLEANDSETTINGSCONSUMER_H

#include <string>
#include <vector>
#include "StyleSheet.h"
#include "Settings.h"

namespace sst::jucegui::style
{
struct StyleConsumer
{
    explicit StyleConsumer(const StyleSheet::Class &c) : styleClass(c){};
    virtual ~StyleConsumer() = default;

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

  private:
    StyleSheet::ptr_t stylep;
    const StyleSheet::Class &styleClass;
};

struct SettingsConsumer
{
    virtual ~SettingsConsumer() = default;

    void setSettings(const Settings::ptr_t &s);
    inline Settings::ptr_t settings()
    {
        if (!settingsp)
            settingsp = std::make_shared<Settings>();
        return settingsp;
    }
    virtual void onSettingsChanged() {}

  private:
    Settings::ptr_t settingsp;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLEANDSETTINGSCONSUMER_H
