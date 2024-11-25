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
    /*
     * GlyphPainter provides a funciton which draws a glpyh into
     * a box in a color. It's both a component and a static function
     * to do the draw you can use in other contexts.
     *
     * The glyphs are enumerated below. To add a glyph you add to
     * that enum then draw it in the switch in GlyphPainter.cpp.
     *
     * Theres two strategies for the glyphs
     *
     * 1. Just paint them. Write code in GlpyhPainter.cpp with draw and
     *    stuff.
     * 2. Use an SVG. To add one with an SVG
     *    a. Add the svg to `res/glyphs/blah.svg`
     *    b. Add that svg to the CMakeLists in the cmrc_add_resource_librar
     *    c. In the GlpyhPainter handle the switch case to delegate by
     *       path.
     * FOr now you have to specify the base color and size of the SVG,
     * since the JUCE APIs to get these aren't super reliable. But really
     * they are all 24x24 0xFFAFAFAF in the Figma so it's just copy and paste
     */
    enum GlyphType
    {
        JOG_UP,
        JOG_DOWN,
        JOG_LEFT,
        JOG_RIGHT,

        ELLIPSIS_H,
        ELLIPSIS_V,

        UP_DOWN,
        LEFT_RIGHT,

        PLUS,
        PLUS_MINUS,

        REVERSE,
        FORWARD_BACKWARD,

        PAN,
        VOLUME,
        TUNING,
        METRONOME,
        KEYBOARD,

        ARROW_L_TO_R,

        MODULATION_ADDITIVE,
        MODULATION_MULTIPLICATIVE,

        STEP_COUNT,
        SMALL_POWER_LIGHT,
        SMALL_POWER_LIGHT_OFF, // a special case of an off power light which isn't the
        // grayed out filled power light

        // Channel Strip routing icons
        ROUTING_PRE_FX,
        ROUTING_PRE_FADER,
        ROUTING_POST_FADER,

        // FileSystem
        FOLDER,
        FILE_MUSIC,

        FAVORITE,
        SAVE,
        SEARCH,
        SETTINGS,
        CLOSE,
        EDIT,
        SHOW_INFO,

        LINK,
        LOCK,
        PIN,
        UNPIN,
        POWER,

        MEMORY, // a chip-like glyph
        MIDI,

        SPEAKER,
        PORTAMENTO,
        CURVE,
        POLYPHONY,

        FREEZE,
        NOTE_PRIORITY,

        MONO,
        STEREO,

        SURGE_LOGO,
        SHORTCIRCUIT_LOGO
        // the order doesn't matter but we iterate in the demo so
        // lets leave shortcircuit logo last
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
    static void paintGlyph(juce::Graphics &, const juce::Rectangle<int> &, GlyphType,
                           const juce::Colour &as);
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_GLYPHPAINTER_H
