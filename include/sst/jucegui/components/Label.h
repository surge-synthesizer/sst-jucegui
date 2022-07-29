//
// Created by Paul Walker on 6/1/22.
//

#ifndef SST_JUCEGUI_LABEL_H
#define SST_JUCEGUI_LABEL_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components
{
struct Label : public juce::Component, public style::StyleConsumer, public style::SettingsConsumer
{
    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"label"};
        static constexpr sprop textcol{"text.color"};
        static constexpr sprop textfont{"text.font", sprop::FONT};
    };

    Label() : style::StyleConsumer(Styles::styleClass){};
    ~Label() = default;

    void setText(const std::string &s)
    {
        text = s;
        repaint();
    }
    std::string getText() const { return text; }

    void paint(juce::Graphics &g) override
    {
        g.setColour(getColour(Styles::textcol));
        g.setFont(getFont(Styles::textfont));
        g.drawText(text, getLocalBounds(), juce::Justification::centred);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Label);

  private:
    std::string text;
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_LABEL_H
