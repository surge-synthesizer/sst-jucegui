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
        METRONOME,

        JOG_UP,
        JOG_DOWN,
        JOG_LEFT,
        JOG_RIGHT,

        BIG_PLUS,

        HAMBURGER,

        KEYBOARD,

        MONO,
        STEREO
    } glyph;

    // The glyph acts like a label so uses hte label color
    struct Styles : base_styles::BaseLabel
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"glyph"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass);
        }
    };

    GlyphPainter(GlyphType g) : style::StyleConsumer(Styles::styleClass), glyph(g) {}
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
