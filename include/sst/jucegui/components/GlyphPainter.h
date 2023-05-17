/*
 * sst-juce-guil - an open source library of juce widgets
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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_GLYPHPAINTER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_GLYPHPAINTER_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include "BaseStyles.h"

namespace sst::jucegui::components
{
struct GlyphPainter : public juce::Component,
                      public style::StyleConsumer,
                      public style::SettingsConsumer
{
    enum GlyphType
    {
        PAN,
        VOLUME,
        TUNING,
        CROSS,
        ARROW_L_TO_R,

        JOG_UP,
        JOG_DOWN,
        JOG_LEFT,
        JOG_RIGHT,

        BIG_PLUS
    } glyph;

    struct Styles : ControlStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"glyph"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(ControlStyles::styleClass);
        }
    };

    GlyphPainter(GlyphType g) : glyph(g), style::StyleConsumer(Styles::styleClass) {}
    void paint(juce::Graphics &) override;

    /*
     * This class is both a component which can paint glyphs bit
     * also provides a static method to paint from other clients.
     * This metnod assumes the color is set *before* you call it
     * since it doesn't have a style sheet, just geometry.
     */
    static void paintGlyph(juce::Graphics &, const juce::Rectangle<int> &, GlyphType);
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_GLYPHPAINTER_H
