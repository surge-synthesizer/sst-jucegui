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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_SCROLLBAR_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_SCROLLBAR_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

#include "BaseStyles.h"

namespace sst::jucegui::components
{
struct ScrollBar : public juce::ScrollBar,
                   public style::StyleConsumer,
                   public style::SettingsConsumer
{
    struct Styles : base_styles::Base, base_styles::Outlined
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"ScrollBar"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::Base::styleClass)
                .withBaseClass(base_styles::Outlined::styleClass);
        }
    };

    ScrollBar(bool isVertical)
        : juce::ScrollBar(isVertical), style::StyleConsumer(Styles::styleClass)
    {
        setAccessible(true);
        setColour(juce::ScrollBar::ColourIds::thumbColourId, getColour(Styles::outline));
    };
    ~ScrollBar() = default;

    void onStyleChanged() override
    {
        setColour(juce::ScrollBar::ColourIds::thumbColourId, getColour(Styles::outline));
    }
};
} // namespace sst::jucegui::components

#endif // SHORTCIRCUITXT_SCROLLBAR_H
