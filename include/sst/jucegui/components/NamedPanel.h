//
// Created by Paul Walker on 5/24/22.
//

#ifndef SST_JUCEGUI_NAMEDPANEL_H
#define SST_JUCEGUI_NAMEDPANEL_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components
{
struct NamedPanel : public juce::Component, public style::StyleAndSettingsConsumer
{
    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;

        static constexpr sclass styleClass{"namedpanel"};
        static constexpr sprop backgroundcol{"background.color"};
        static constexpr sprop bordercol{"border.color"};
        static constexpr sprop labelcol{"label.color"};
        static constexpr sprop labelrulecol{"labelrule.color"};
    };

    NamedPanel(const std::string &name);
    ~NamedPanel();

    void paint(juce::Graphics &g) override;
    void resized() override;

    void setContentAreaComponent(std::unique_ptr<juce::Component> &&c)
    {
        contentAreaComp = std::move(c);
        addAndMakeVisible(*contentAreaComp);
        resized();
    }

  protected:
    std::string name;
    std::unique_ptr<juce::Component> contentAreaComp;
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_NAMEDPANEL_H
