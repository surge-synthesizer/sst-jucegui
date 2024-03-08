/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TABBEDCOMPONENT_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TABBEDCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "sst/jucegui/style/StyleAndSettingsConsumer.h"

namespace sst::jucegui::components
{
struct TabbedComponent : juce::TabbedComponent, style::StyleConsumer
{
    struct Styles : base_styles::Base
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"TabbedComponent"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(base_styles::Base::styleClass);
        }
    };

    explicit TabbedComponent(
        juce::TabbedButtonBar::Orientation o = juce::TabbedButtonBar::TabsAtTop)
        : juce::TabbedComponent(o), style::StyleConsumer(Styles::styleClass)
    {
    }

    void currentTabChanged(int newCurrentTabIndex, const juce::String &newCurrentTabName) override
    {
        // Lots of ways we could do this but for now just
        // rehammer the current style onto the newly component
        // hierarchy added components
        setStyle(style());
    }

    struct ButtonImpl : juce::TabBarButton
    {
        juce::String name;
        int idx{0};
        ButtonImpl(const juce::String &n, int id, juce::TabbedButtonBar &owner)
            : juce::TabBarButton(n, owner), name(n), idx(id)
        {
        }
        void paint(juce::Graphics &g) override { juce::TabBarButton::paint(g); }
        int getBestTabLength(int depth) override { return 20; }
    };
    juce::TabBarButton *createTabButton(const juce::String &tabName, int tabIndex) override
    {
        return new ButtonImpl(tabName, tabIndex, getTabbedButtonBar());
    }

    void resized() override
    {
        auto content = getLocalBounds();

        tabs->setBounds(content.withHeight(18));
        content = content.withTrimmedTop(18);

        // Say it with me: private is dumb
        // for (auto &c : contentComponents)
        for (int i = 0; i < getNumTabs(); ++i)
            if (auto comp = getTabContentComponent(i))
                comp->setBounds(content);
    }
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_TABBEDCOMPONENT_H
