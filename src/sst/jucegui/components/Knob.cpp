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

#include <sst/jucegui/components/Knob.h>
#include "KnobPainter.hxx"

namespace sst::jucegui::components
{
template <typename T> KnobFor<T>::KnobFor() : style::StyleConsumer(Styles::styleClass), T() {}

template <>
KnobFor<ContinuousParamEditor>::KnobFor()
    : StyleConsumer(Styles::styleClass),
      ContinuousParamEditor(ContinuousParamEditor::Direction::VERTICAL)
{
}

template <typename T> KnobFor<T>::~KnobFor() = default;

void Knob::paint(juce::Graphics &g)
{
    auto b = getLocalBounds();
    if (this->continuousModulatable())
    {
        knobPainter(g, this, this->continuousModulatable());
    }
    else
    {
        knobPainter(g, this, this->continuous());
    }
    if (drawLabel)
    {
        auto textarea = b.withTrimmedTop(b.getWidth());
        if (isHovered)
            g.setColour(getColour(Styles::labelcolor_hover));
        else
            g.setColour(getColour(Styles::labelcolor));
        g.setFont(getFont(Styles::labelfont));
        g.drawText(continuous()->getLabel(), textarea, juce::Justification::centred);
    }
}

void DiscreteKnob::paint(juce::Graphics &g) { knobPainter(g, this, data); }

void DiscreteKnob::showPopup(const juce::ModifierKeys &m)
{
    if (onPopupMenu)
    {
        onPopupMenu(m);
    }
    else
    {
        DiscreteParamMenuBuilder builder;
        builder.setData(data);
        builder.showMenu(this);
    }
}

void DiscreteKnob::mouseDown(const juce::MouseEvent &event)
{
    if (event.mods.isPopupMenu())
    {
        showPopup(event.mods);
        return;
    }
    isEditing = true;
    dragFromY = event.position.y;
    onBeginEdit();
}

void DiscreteKnob::mouseDrag(const juce::MouseEvent &e)
{
    if (!isEditing)
        return;
    float dy = -(e.position.y - dragFromY);
    float minForScaling = data->getMin();
    float maxForScaling = data->getMax();

    auto d = (dy) / 150.0 * (maxForScaling - minForScaling);
    if (d > 1 || d < -1)
    {
        auto dd = std::round(d);
        auto v = data->getValue();
        auto nv = static_cast<int>(v + dd);
        nv = std::clamp(nv, data->getMin(), data->getMax());

        data->setValueFromGUI(nv);
        dragFromY = e.position.y;
    }
}

void DiscreteKnob::mouseUp(const juce::MouseEvent &event)
{
    if (isEditing)
    {
        onEndEdit();
    }
    isEditing = false;
}

// Explicit instantiation
template struct KnobFor<ContinuousParamEditor>;
template struct KnobFor<DiscreteParamEditor>;
} // namespace sst::jucegui::components