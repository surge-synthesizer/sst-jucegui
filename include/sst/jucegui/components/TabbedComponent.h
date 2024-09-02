/*
 * sst-jucegui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-jucegui is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-jucegui available at
 * https://github.com/surge-synthesizer/sst-jucegui
 */

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TABBEDCOMPONENT_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TABBEDCOMPONENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "sst/jucegui/style/StyleAndSettingsConsumer.h"

namespace sst::jucegui::components
{
struct TabbedComponent : juce::TabbedComponent, style::StyleConsumer
{
    struct Styles : base_styles::Base, base_styles::BaseLabel
    {
        SCLASS(tabbedcomponent);

        PROP(tabSelectedLabelColor);
        PROP(tabUnselectedLabelColor);
        PROP_HOVER(tabUnselectedLabelColor);
        PROP(tabSelectedFillColor);
        PROP_HOVER(tabSelectedFillColor);
        PROP(tabUnselectedOutlineColor);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::Base::styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass)
                .withProperty(tabSelectedLabelColor)
                .withProperty(tabUnselectedLabelColor)
                .withProperty(tabUnselectedLabelColor_hover)
                .withProperty(tabSelectedFillColor)
                .withProperty(tabSelectedFillColor_hover)
                .withProperty(tabUnselectedOutlineColor);
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
        TabbedComponent *parentComp{nullptr};
        juce::String name;
        int idx{0};
        ButtonImpl(const juce::String &n, int id, juce::TabbedButtonBar &owner, TabbedComponent *c)
            : juce::TabBarButton(n, owner), name(n), idx(id), parentComp(c)
        {
        }
        static constexpr int marginPad{3}, minTab{20};
        void paint(juce::Graphics &g) override
        {
            if (!parentComp->style())
                return;

            const auto &st = parentComp->style();

            auto b = getLocalBounds().withHeight(getHeight() + 10).reduced(1);

            if (isFrontTab())
            {
                if (hover)
                    g.setColour(
                        st->getColour(Styles::styleClass, Styles::tabSelectedFillColor_hover));
                else
                    g.setColour(st->getColour(Styles::styleClass, Styles::tabSelectedFillColor));
                g.fillRoundedRectangle(b.toFloat(), 4);
                g.setColour(st->getColour(Styles::styleClass, Styles::tabSelectedFillColor_hover));
                g.drawRoundedRectangle(b.toFloat(), 4, 1);
                g.setColour(st->getColour(Styles::styleClass, Styles::tabSelectedLabelColor));
                g.setFont(st->getFont(Styles::styleClass, Styles::labelfont));
                g.drawText(name, getLocalBounds(), juce::Justification::centred);
            }
            else
            {
                g.setColour(st->getColour(Styles::styleClass, Styles::tabUnselectedOutlineColor));
                g.drawRoundedRectangle(b.toFloat(), 4, 1);

                if (hover)
                    g.setColour(
                        st->getColour(Styles::styleClass, Styles::tabUnselectedLabelColor_hover));
                else
                    g.setColour(st->getColour(Styles::styleClass, Styles::tabUnselectedLabelColor));

                g.setFont(st->getFont(Styles::styleClass, Styles::labelfont));
                g.drawText(name, getLocalBounds(), juce::Justification::centred);
            }
        }
        bool hover{false};

      protected:
        void mouseEnter(const juce::MouseEvent &event) override
        {
            hover = true;
            repaint();
        }
        void mouseExit(const juce::MouseEvent &event) override
        {
            hover = false;
            repaint();
        }

      public:
        int getBestTabLength(int depth) override
        {
            if (!parentComp->style())
                return minTab;

            auto ft = parentComp->style()->getFont(Styles::styleClass, Styles::labelfont);
            auto fw = ft.getStringWidthFloat(name) + 2 * marginPad;
            return std::max((int)std::ceil(fw), minTab);
        }
    };
    juce::TabBarButton *createTabButton(const juce::String &tabName, int tabIndex) override
    {
        return new ButtonImpl(tabName, tabIndex, getTabbedButtonBar(), this);
    }

    void resized() override
    {
        auto content = getLocalBounds();

        tabs->setBounds(content.withHeight(18).withTrimmedLeft(4));
        content = content.withTrimmedTop(18);

        // Say it with me: private is dumb
        // for (auto &c : contentComponents)
        for (int i = 0; i < getNumTabs(); ++i)
            if (auto comp = getTabContentComponent(i))
                comp->setBounds(content);
    }

    void onStyleChanged() override { resized(); }
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_TABBEDCOMPONENT_H
