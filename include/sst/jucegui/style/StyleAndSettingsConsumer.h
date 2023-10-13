/*
 * sst-juce-gui - an open source library of juce widgets
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
        if (style())
            return style()->getColour(getStyleClass(), p);
        return juce::Colours::red;
    }

    juce::Font getFont(const StyleSheet::Property &p)
    {
        if (style())
            return style()->getFont(getStyleClass(), p);
        return juce::Font(1);
    }

    // these don't belong on instances they belong on stylesheets
    StyleSheet::Class customClass{""};
    void setCustomClass(const StyleSheet::Class &sc)
    {
        customClass.copyFrom(sc);
        StyleSheet::extendInheritanceMap(customClass, styleClass);
    }

    void removeCustomClass() { customClass.cname[0] = 0; }

    const StyleSheet::Class &getStyleClass()
    {
        if (customClass.cname[0] != 0)
            return customClass;
        return styleClass;
    }

    void setStyle(const StyleSheet::ptr_t &s);

    /*
     * Note style() can return nullptr
     */
    inline StyleSheet::ptr_t style()
    {
        if (!stylep)
        {
            auto jc = dynamic_cast<juce::Component *>(this);
            while (jc)
            {
                jc = jc->getParentComponent();
                auto sc = dynamic_cast<StyleConsumer *>(jc);
                if (sc)
                {
                    stylep = sc->style();
                    if (stylep)
                    {
                        onStyleChanged();
                        jc = nullptr;
                    }
                }
            }
            /* Please ask BP before you nuke this debug code
            if (!stylep)
            {
                std::cout << "Style Unresolved" << std::endl;

                jc = dynamic_cast<juce::Component *>(this);
                std::string pfx = "";
                while (jc)
                {
                    std::cout << pfx << jc << " " << jc->getTitle() << " "
                              << jc->getBounds().toString() << " " << typeid(*jc).name() << " "
                              << dynamic_cast<StyleConsumer *>(jc) << "\n";
                    pfx = pfx + "   ";
                    jc = jc->getParentComponent();
                }
            }
             */
        }
        return stylep;
    }
    virtual void onStyleChanged() {}

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StyleConsumer)

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

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SettingsConsumer)

  private:
    Settings::ptr_t settingsp;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLEANDSETTINGSCONSUMER_H
