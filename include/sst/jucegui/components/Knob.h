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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_KNOB_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_KNOB_H

#include "ContinuousParamEditor.h"
#include "DiscreteParamEditor.h"

namespace sst::jucegui::components
{
template <typename T> struct KnobFor : public T, public style::StyleConsumer
{
    struct Styles : ContinuousParamEditor::Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"knob"};

        static constexpr sprop knobbase{"knobbase.color"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(ContinuousParamEditor::Styles::styleClass)
                .withProperty(knobbase);
        }
    };

    KnobFor();
    ~KnobFor();

    enum PathDrawMode
    {
        FOLLOW_BIPOLAR,
        ALWAYS_FROM_MIN,
        ALWAYS_FROM_MAX,
        ALWAYS_FROM_DEFAULT
    } pathDrawMode{FOLLOW_BIPOLAR};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(KnobFor)
};

struct Knob : KnobFor<ContinuousParamEditor>
{
    bool drawLabel{true};
    void setDrawLabel(bool b)
    {
        drawLabel = b;
        this->repaint();
    }

    void paint(juce::Graphics &g);
};

struct DiscreteKnob : KnobFor<DiscreteParamEditor>
{
    void paint(juce::Graphics &g) override;
    void showPopup(const juce::ModifierKeys &m) override;

    bool isEditing{false};
    float dragFromY{0.f};
    void mouseDown(const juce::MouseEvent &event) override;
    void mouseDrag(const juce::MouseEvent &event) override;
    void mouseUp(const juce::MouseEvent &event) override;
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_KNOBS_H
