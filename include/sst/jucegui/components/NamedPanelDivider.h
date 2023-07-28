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

    struct Styles : BaseStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;

        static constexpr sclass styleClass{"namedpaneldivider"};
        static constexpr sprop dividercol{"divider.color"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(BaseStyles::styleClass)
                .withProperty(dividercol);
        }
    };

    NamedPanelDivider();
    ~NamedPanelDivider();

    void paint(juce::Graphics &g) override;
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_NAMEDPANELDIVIDER_H
