//
// Created by Paul Walker on 5/24/22.
//

#ifndef SST_JUCEGUI_WINDOWPANEL_H
#define SST_JUCEGUI_WINDOWPANEL_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components
{
struct WindowPanel : public juce::Component,
                     public style::StyleConsumer,
                     public style::SettingsConsumer
{
    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"windowpanel"};
        static constexpr sprop backgroundgradstart{"bgstart.color"};
        static constexpr sprop backgroundgradend{"bgend.color"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withProperty(backgroundgradend)
                .withProperty(backgroundgradstart);
        }
    };

    WindowPanel() : style::StyleConsumer(Styles::styleClass){};
    ~WindowPanel() = default;

    void paint(juce::Graphics &g) override
    {
        auto cg = juce::ColourGradient::vertical(getColour(Styles::backgroundgradstart), 0,
                                                 getColour(Styles::backgroundgradend), getHeight());
        g.setGradientFill(cg);
        g.fillRect(getLocalBounds());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WindowPanel);
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_WINDOWPANEL_H
