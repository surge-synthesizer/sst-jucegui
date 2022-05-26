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
struct StyleAndSettingsConsumer
{
    explicit StyleAndSettingsConsumer(const StyleSheet::Class &c) : styleClass(c){};
    virtual ~StyleAndSettingsConsumer() = default;

    juce::Colour getColour(const StyleSheet::Property &p)
    {
        for (const auto &c : subClasses)
        {
            if (style()->hasColour(c, p))
                return style()->getColour(c, p);
        }
        if (style()->hasColour(getStyleClass(), p))
            return style()->getColour(getStyleClass(), p);
        return style()->getColour(superClass,
                                  p); // this stil isnt' right since it doesn't recurse up chains
    }

    // these don't belong on instances they belong on stylesheets
    std::vector<StyleSheet::Class> subClasses;
    void addStyleSubclass(const StyleSheet::Class &sc) { subClasses.push_back(sc); }

    StyleSheet::Class superClass{""};
    void setStyleSuperclass(const StyleSheet::Class &sc) { superClass = sc; }

    const StyleSheet::Class &getStyleClass() { return styleClass; }

    void setStyle(const StyleSheet::ptr_t &s);
    inline StyleSheet::ptr_t style()
    {
        if (!stylep)
            stylep = StyleSheet::getBuiltInStyleSheet(StyleSheet::DARK);
        return stylep;
    }
    virtual void onStyleChanged() {}

    void setSettings(const Settings::ptr_t &s);
    inline Settings::ptr_t settings()
    {
        if (!settingsp)
            settingsp = std::make_shared<Settings>();
        return settingsp;
    }
    virtual void onSettingsChanged() {}

  private:
    StyleSheet::ptr_t stylep;
    Settings::ptr_t settingsp;
    const StyleSheet::Class &styleClass;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLEANDSETTINGSCONSUMER_H
