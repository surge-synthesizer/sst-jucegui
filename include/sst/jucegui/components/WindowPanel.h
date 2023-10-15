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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_WINDOWPANEL_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_WINDOWPANEL_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components
{
struct WindowPanel : public juce::Component,
                     public style::StyleConsumer,
                     public style::SettingsConsumer
{
    struct Styles
    {
        SCLASS(windowpanel);
        PROP(bgstart);
        PROP(bgend);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withProperty(bgstart).withProperty(bgend);
        }
    };

    WindowPanel() : style::StyleConsumer(Styles::styleClass) {}
    ~WindowPanel() = default;

    void paint(juce::Graphics &g) override
    {
        auto cg = juce::ColourGradient::vertical(getColour(Styles::bgstart), 0,
                                                 getColour(Styles::bgend), getHeight());
        g.setGradientFill(cg);
        g.fillRect(getLocalBounds());
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WindowPanel)
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_WINDOWPANEL_H
