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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_BASESTYLES_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_BASESTYLES_H

#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components::base_styles
{
#define SCLASS(x)                                                                                  \
    static constexpr style::StyleSheet::Class styleClass { #x }
#define PROP(x)                                                                                    \
    static constexpr style::StyleSheet::Property x { #x }
#define PROP_NAMED(x, y)                                                                           \
    static constexpr style::StyleSheet::Property x { y }
#define PROP_FONT(x)                                                                               \
    static constexpr style::StyleSheet::Property x { #x, style::StyleSheet::Property::FONT }
#define PROP_OFF(x)                                                                                \
    static constexpr style::StyleSheet::Property x##_off { #x ".off" }
#define PROP_HOVER(x)                                                                              \
    static constexpr style::StyleSheet::Property x##_hover { #x ".hover" }
#define PROP_HOVER_OFF(x)                                                                          \
    static constexpr style::StyleSheet::Property x##_hover_off { #x ".hover.off" }

#define PROP_FULL_HOVER(x)                                                                         \
    PROP(x);                                                                                       \
    PROP_OFF(x);                                                                                   \
    PROP_HOVER(x);                                                                                 \
    PROP_HOVER_OFF(x);

#define WITH_PROP_FULL_HOVER(x)                                                                    \
    withProperty(x).withProperty(x##_off).withProperty(x##_hover).withProperty(x##_hover_off)

struct Base
{
    SCLASS(base);
    PROP(background);
    static void initialize() { style::StyleSheet::addClass(styleClass).withProperty(background); }
};

struct Outlined
{
    SCLASS(outlined);
    PROP(outline);
    PROP(brightoutline);
    static void initialize()
    {
        style::StyleSheet::addClass(styleClass).withProperty(outline).withProperty(brightoutline);
    }
};

struct ValueBearing
{
    SCLASS(value_bearing);
    PROP(value);
    PROP_HOVER(value);
    PROP(valuelabel);
    PROP_HOVER(valuelabel);
    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withProperty(value)
            .withProperty(value_hover)
            .withProperty(valuelabel)
            .withProperty(valuelabel_hover);
    }
};

struct ModulationValueBearing
{
    SCLASS(modulation_value_bearing);
    PROP_FULL_HOVER(modulation_value);
    PROP_FULL_HOVER(modulation_opposite_value);
    PROP(modulated_by_selected);
    PROP(modulated_by_other);
    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .WITH_PROP_FULL_HOVER(modulation_value)
            .WITH_PROP_FULL_HOVER(modulation_opposite_value)
            .withProperty(modulated_by_selected)
            .withProperty(modulated_by_other);
    }
};

struct ValueGutter
{
    SCLASS(value_gutter);
    PROP(gutter);
    PROP_HOVER(gutter);
    static void initialize()
    {
        style::StyleSheet::addClass(styleClass).withProperty(gutter).withProperty(gutter_hover);
    }
};

struct GraphicalHandle
{
    SCLASS(graphical_handle);
    PROP(handle);
    PROP_HOVER(handle);
    PROP(modulation_handle);
    PROP_HOVER(modulation_handle);
    PROP(handle_outline);
    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withProperty(handle)
            .withProperty(handle_hover)
            .withProperty(modulation_handle)
            .withProperty(modulation_handle_hover)
            .withProperty(handle_outline);
    }
};

struct BaseLabel
{
    SCLASS(baselabel);

    PROP(labelcolor);
    PROP_HOVER(labelcolor);
    PROP_FONT(labelfont);

    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withProperty(labelcolor)
            .withProperty(labelcolor_hover)
            .withProperty(labelfont);
    }
};

struct PushButton : Outlined
{
    SCLASS(pushbutton);

    PROP(fill);
    PROP_HOVER(fill);
    PROP_NAMED(fill_pressed, "fill.pressed");

    static void initialize()
    {
        style::StyleSheet::addClass(styleClass)
            .withBaseClass(Outlined::styleClass)
            .withProperty(fill)
            .withProperty(fill_hover)
            .withProperty(fill_pressed);
    }
};

inline void initialize()
{
    Base::initialize();
    Outlined::initialize();
    ValueBearing::initialize();
    ModulationValueBearing::initialize();
    ValueGutter::initialize();
    GraphicalHandle::initialize();
    BaseLabel::initialize();
    PushButton::initialize();
}

} // namespace sst::jucegui::components::base_styles

#endif // SST_JUCEGUI_BASESTYLES_H
