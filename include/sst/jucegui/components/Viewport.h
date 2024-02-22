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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_VIEWPORT_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_VIEWPORT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

#include "BaseStyles.h"
#include "ScrollBar.h"

namespace sst::jucegui::components
{
struct Viewport : public juce::Viewport, public style::StyleConsumer, public style::SettingsConsumer
{
    struct Styles : base_styles::Base, base_styles::Outlined
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"Viewport"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(base_styles::Base::styleClass);
        }
    };

    Viewport(const juce::String &cn = juce::String())
        : style::StyleConsumer(Styles::styleClass), juce::Viewport(cn)
    {
        setScrollBarThickness(6);
        recreateScrollbars();
    }

  protected:
    juce::ScrollBar *createScrollBarComponent(bool isVertical) override
    {
        return new sst::jucegui::components::ScrollBar(isVertical);
    }
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_VIEWPORT_H
