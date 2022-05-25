//
// Created by Paul Walker on 5/24/22.
//

#ifndef SST_JUCEGUI_WINDOWPANEL_H
#define SST_JUCEGUI_WINDOWPANEL_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components
{
struct WindowPanel : public juce::Component, public style::StyleConsumer
{
    struct Styles
    {
        static constexpr const char *className = "windowpanel";
        using sprop = style::StyleSheet::Property;
        static constexpr sprop backgroundcol{"background.color"};
    };

    WindowPanel() : style::StyleConsumer(Styles::className){};
    ~WindowPanel() = default;

    void paint(juce::Graphics &g) override
    {
        auto c = style()->getColour(this, Styles::backgroundcol);
        g.fillAll(c);
    }
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_WINDOWPANEL_H
