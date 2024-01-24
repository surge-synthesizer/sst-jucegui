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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TOGGLEBUTTON_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TOGGLEBUTTON_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Discrete.h>
#include <sst/jucegui/components/BaseStyles.h>
#include <sst/jucegui/components/DiscreteParamEditor.h>
#include <sst/jucegui/components/GlyphPainter.h>

#include <string>

#include "ComponentBase.h"

namespace sst::jucegui::components
{
struct ToggleButton : DiscreteParamEditor,
                      public style::StyleConsumer,
                      public style::SettingsConsumer
{
    ToggleButton();
    ~ToggleButton();

    struct Styles : base_styles::PushButton, base_styles::BaseLabel, base_styles::ValueBearing
    {
        SCLASS(togglebutton);
        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::PushButton::styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass)
                .withBaseClass(base_styles::ValueBearing::styleClass);
        }
    };

    enum struct DrawMode
    {
        LABELED,
        FILLED,
        GLYPH
    } drawMode{DrawMode::LABELED};

    void setDrawMode(DrawMode m)
    {
        drawMode = m;
        repaint();
    }
    void setLabel(const std::string &l) { label = l; }

    GlyphPainter::GlyphType type{GlyphPainter::CROSS};
    void setGlyph(GlyphPainter::GlyphType gt)
    {
        drawMode = DrawMode::GLYPH;
        type = gt;
        repaint();
    }

    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;

    void paint(juce::Graphics &g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleButton)

    bool isPressed{false};

  private:
    std::string label;
};

} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_TOGGLEBUTTON_H
