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

#ifndef INCLUDE_SST_JUCEGUI_SCREENS_MODALBASE_H
#define INCLUDE_SST_JUCEGUI_SCREENS_MODALBASE_H

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include "../components/BaseStyles.h"

namespace sst::jucegui::screens
{
struct ModalBase : juce::Component, style::StyleConsumer
{

    struct Styles : components::base_styles::BaseLabel,
                    components::base_styles::Base,
                    components::base_styles::Outlined
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"ModalBase"};

        PROP_FONT(titleFont);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withProperty(titleFont)
                .withBaseClass(components::base_styles::Base::styleClass)
                .withBaseClass(components::base_styles::Outlined::styleClass)
                .withBaseClass(components::base_styles::BaseLabel::styleClass);
        }
    };

    ModalBase() : style::StyleConsumer(Styles::styleClass) {}
    virtual void paintContents(juce::Graphics &g) {}
    virtual juce::Point<int> innerContentSize() = 0; // w,h

    juce::Rectangle<int> getContentArea()
    {
        auto sz = innerContentSize();
        sz.x = std::min(sz.x, getWidth() - 20);
        sz.y = std::min(sz.y, getWidth() - 20);
        auto r = juce::Rectangle<int>(0, 0, sz.x, sz.y);
        r.setCentre(getLocalBounds().getCentre());

        return r;
    }
    void paint(juce::Graphics &g) override
    {
        g.fillAll(getColour(Styles::background).withAlpha(0.7f));

        auto r = getContentArea();
        g.setColour(getColour(Styles::background));
        g.fillRect(r);
        g.setColour(getColour(Styles::brightoutline));
        g.drawRect(r, 1);

        paintContents(g);
    }
};
} // namespace sst::jucegui::screens
#endif // MODALBASE_H
