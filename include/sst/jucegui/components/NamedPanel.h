//
// Created by Paul Walker on 5/24/22.
//

#ifndef SST_JUCEGUI_NAMEDPANEL_H
#define SST_JUCEGUI_NAMEDPANEL_H

#include <vector>
#include <string>
#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include "BaseStyles.h"

namespace sst::jucegui::components
{
struct NamedPanel : public juce::Component,
                    public style::StyleConsumer,
                    public style::SettingsConsumer
{
    static constexpr int outerMargin = 2, cornerRadius = 2, headerHeight = 20;

    struct Styles : BaseStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;

        static constexpr sclass styleClass{"namedpanel"};
        static constexpr sprop labelrulecol{"labelrule.color"};
        static constexpr sprop selectedtabcol{"selectedtab.color"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(BaseStyles::styleClass)
                .withProperty(labelrulecol)
                .withProperty(selectedtabcol);
        }
    };

    NamedPanel(const std::string &name);
    ~NamedPanel();

    void paint(juce::Graphics &g) override;
    void resized() override;

    juce::Rectangle<int> getContentArea();

    virtual void paintHeader(juce::Graphics &g);

    void setContentAreaComponent(std::unique_ptr<juce::Component> &&c)
    {
        contentAreaComp = std::move(c);
        addAndMakeVisible(*contentAreaComp);
        resized();
    }

    bool isTabbed{false};
    std::vector<std::string> tabNames{};
    size_t selectedTab{0};
    std::function<void(int)> onTabSelected{nullptr};
    std::vector<juce::Rectangle<int>> tabPositions;
    juce::Rectangle<int> totalTabArea;
    void resetTabState();

    bool hasHamburger{false};
    static constexpr int hamburgerSize = 22;
    void mouseDown(const juce::MouseEvent &event) override;
    juce::Rectangle<int> getHamburgerRegion();

    std::function<void()> onHamburger{nullptr};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NamedPanel);

  protected:
    std::string name;
    std::unique_ptr<juce::Component> contentAreaComp;
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_NAMEDPANEL_H
