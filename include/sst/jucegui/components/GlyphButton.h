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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_GLYPHBUTTON_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_GLYPHBUTTON_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Discrete.h>
#include <sst/jucegui/components/BaseStyles.h>
#include "ToggleButton.h"

#include <string>

#include "ComponentBase.h"
#include "MenuButton.h"
#include "GlyphPainter.h"

namespace sst::jucegui::components
{
struct GlyphButton : public CallbackButtonComponent<GlyphButton>,
                     public style::StyleConsumer,
                     public style::SettingsConsumer,
                     public EditableComponentBase<GlyphButton>
{
    GlyphButton(GlyphPainter::GlyphType type);
    ~GlyphButton();

    struct Styles : TextualControlStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"glyphbutton"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(MenuButton::Styles::styleClass);
        }
    };

    int glyphButtonPad{0};
    void paint(juce::Graphics &g) override;
    GlyphPainter::GlyphType glyph;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GlyphButton)
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_MENUBUTTON_H
