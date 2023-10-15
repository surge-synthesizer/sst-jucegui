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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_NAMEDPANEL_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_NAMEDPANEL_H

#include <vector>
#include <string>
#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include "BaseStyles.h"
#include <sst/jucegui/data/Discrete.h>

namespace sst::jucegui::components
{
struct ToggleButton;

struct NamedPanel : public juce::Component,
                    public style::StyleConsumer,
                    public style::SettingsConsumer
{
    static constexpr int outerMargin = 2, cornerRadius = 2, headerHeight = 20, togglePad = 3;

    struct Styles : base_styles::Base, base_styles::Outlined, base_styles::BaseLabel
    {
        SCLASS(namedpanel);

        PROP(labelrule);
        PROP(selectedtab);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::Base::styleClass)
                .withBaseClass(base_styles::Outlined::styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass)
                .withProperty(labelrule)
                .withProperty(selectedtab);
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
        if (isVisible())
            resized();
    }

    const std::unique_ptr<juce::Component> &getContentAreaComponent() const
    {
        return contentAreaComp;
    }

    virtual void setName(const juce::String &n) override
    {
        juce::Component::setName(n);
        name = n.toStdString();
        repaint();
    }

    virtual void enablementChanged() override { repaint(); }

    /*
     * Named Panels can have toggle buttons which attach to a
     * discrete data source.
     */
    bool isTogglable{false};
    void setTogglable(bool b);
    std::unique_ptr<ToggleButton> toggleButton;
    void setToggleDataSource(data::Discrete *d);

    bool centeredHeader{false};
    bool selectable{false}, selected{false};
    void setSelected(bool b)
    {
        selected = b;
        repaint();
    }

    /*
     * Named panels can have tab selections as their named
     * bar.
     */
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

    void onStyleChanged() override
    {
        resetTabState();
        resized();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NamedPanel)

  protected:
    std::string name;
    std::unique_ptr<juce::Component> contentAreaComp;
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_NAMEDPANEL_H
