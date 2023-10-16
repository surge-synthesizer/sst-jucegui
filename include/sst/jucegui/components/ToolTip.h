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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TOOLTIP_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TOOLTIP_H

#include <juce_gui_basics/juce_gui_basics.h>
#include "sst/jucegui/style/StyleAndSettingsConsumer.h"
#include "sst/jucegui/components/BaseStyles.h"

namespace sst::jucegui::components
{
struct ToolTip : juce::Component, sst::jucegui::style::StyleConsumer
{
    struct Styles : sst::jucegui::components::base_styles::BaseLabel,
                    sst::jucegui::components::base_styles::Outlined,
                    sst::jucegui::components::base_styles::Base
    {
        static constexpr sst::jucegui::style::StyleSheet::Class styleClass{"tooltip"};
        static constexpr sst::jucegui::style::StyleSheet::Property datafont{
            "datafont", sst::jucegui::style::StyleSheet::Property::FONT};
        static void initialize()
        {
            sst::jucegui::style::StyleSheet::addClass(styleClass)
                .withBaseClass(sst::jucegui::components::base_styles::BaseLabel::styleClass)
                .withBaseClass(sst::jucegui::components::base_styles::Outlined::styleClass)
                .withBaseClass(sst::jucegui::components::base_styles::Base::styleClass)
                .withProperty(datafont);
        }
    };

    ToolTip();
    void paint(juce::Graphics &g);

    void setTooltipTitleAndData(const std::string &t, const std::vector<std::string> &d)
    {
        tooltipTitle = t;
        tooltipData = d;
        resetSizeFromData();
        repaint();
    }

    void resetSizeFromData();
    std::string tooltipTitle{};
    std::vector<std::string> tooltipData;
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_TOOLTIP_H
