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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_NAMEDPANELDIVIDER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_NAMEDPANELDIVIDER_H

#include <vector>
#include <string>
#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include "BaseStyles.h"

namespace sst::jucegui::components
{
struct NamedPanelDivider : public juce::Component,
                           public style::StyleConsumer,
                           public style::SettingsConsumer
{
    static constexpr int outerMargin = 2, cornerRadius = 2, headerHeight = 20, togglePad = 3;

    struct Styles : base_styles::Outlined
    {
        SCLASS(namedpaneldivider);
        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::Outlined::styleClass);
        }
    };

    bool isHorizontal{true};
    NamedPanelDivider(bool isHorizontal = true);
    ~NamedPanelDivider();

    void paint(juce::Graphics &g) override;
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_NAMEDPANELDIVIDER_H
