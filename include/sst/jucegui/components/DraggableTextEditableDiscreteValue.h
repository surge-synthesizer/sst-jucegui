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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_DRAGGABLETEXTEDITABLEDISCRETEVALUE_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_DRAGGABLETEXTEDITABLEDISCRETEVALUE_H

#include <string>

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/components/BaseStyles.h>
#include <sst/jucegui/components/DiscreteParamEditor.h>
#include <sst/jucegui/components/TextEditableValueSupport.h>

namespace sst::jucegui::components
{

/*
 * A draggable, type-in text value for discrete (integer-backed) parameters. It
 * is the discrete-model sibling of DraggableTextEditableValue: the wheel and
 * arrow keys jog by whole steps for free from DiscreteParamEditor, drag jogs on
 * fixed pixels, and right-click / double-click / keyboard open inline type-in.
 */
struct DraggableTextEditableDiscreteValue
    : public DiscreteParamEditor,
      public style::StyleConsumer,
      public TextEditableValueSupport<DraggableTextEditableDiscreteValue>
{
    // Same style class as the continuous draggable text value, so a theme that
    // colours one colours both.
    using Styles = TextEditableValueSupport<DraggableTextEditableDiscreteValue>::Styles;

    DraggableTextEditableDiscreteValue();
    ~DraggableTextEditableDiscreteValue() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void mouseDoubleClick(const juce::MouseEvent &e) override;

    void onStyleChanged() override;

    // keyboard "open editor" action from DiscreteParamEditor
    void openTypeInEditor() override { activateEditor(); }

    /*
     * A numeric text value is an ordered range, so wheel/keyboard must not wrap
     * past the end. This is set here rather than on the attachment because the
     * same discrete attachments also drive enumerable widgets (menus, switches),
     * where wrapping is the desired behaviour.
     */
    void setSource(data::Discrete *d) override
    {
        DiscreteParamEditor::setSource(d);
        if (d)
            d->setJogWrapsAtEnd(false);
    }

    void setDisplayUnits(bool b)
    {
        displayUnits = b;
        repaint();
    }

    // hooks required by TextEditableValueSupport
    bool hasSource() { return data != nullptr; }
    std::string displayString()
    {
        if (!data)
            return "";
        return displayUnits ? data->getValueAsString() : data->getValueAsStringWithoutUnits();
    }
    void applyString(const std::string &s)
    {
        if (data)
            data->setValueAsString(s);
    }
    void applyDefault()
    {
        if (data)
            data->setValueFromGUI(data->getDefaultValue());
    }

    // Pixels of vertical drag per single jog. Per-platform starting values;
    // calibrate by feel once landed. Uses JUCE's platform macros (not the
    // target-private JUCEGUI_* ones, which are invisible to header consumers).
#if JUCE_WINDOWS
    static constexpr int pixelsPerJog{3};
#elif JUCE_LINUX
    static constexpr int pixelsPerJog{3};
#else
    static constexpr int pixelsPerJog{3};
#endif
    // shift widens the pixels-per-jog and switches to a single raw step, so a
    // stepped param still gets fine positioning where one jog is more than 1.
    static constexpr int shiftJogFactor{10};

  protected:
    int dragStepsEmitted{0};
};
} // namespace sst::jucegui::components

#endif // INCLUDE_SST_JUCEGUI_COMPONENTS_DRAGGABLETEXTEDITABLEDISCRETEVALUE_H
