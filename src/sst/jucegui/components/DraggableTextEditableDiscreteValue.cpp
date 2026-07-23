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

#include <algorithm>

#include <sst/jucegui/components/DraggableTextEditableDiscreteValue.h>

namespace sst::jucegui::components
{

DraggableTextEditableDiscreteValue::DraggableTextEditableDiscreteValue()
    : style::StyleConsumer(Styles::styleClass)
{
    initTextEditor();
}

DraggableTextEditableDiscreteValue::~DraggableTextEditableDiscreteValue() = default;

void DraggableTextEditableDiscreteValue::paint(juce::Graphics &g) { paintFor(this, g); }

void DraggableTextEditableDiscreteValue::mouseDown(const juce::MouseEvent &e)
{
    if (!mouseDownFor(this, e))
        return;

    dragStepsEmitted = 0;
}

void DraggableTextEditableDiscreteValue::mouseUp(const juce::MouseEvent &e)
{
    mouseUpFor(this, e);
    dragStepsEmitted = 0;
}

void DraggableTextEditableDiscreteValue::mouseDrag(const juce::MouseEvent &e)
{
    if (!data)
        return;
    if (!mouseDragFor(this, e))
        return;

    // Accumulate raw drag pixels and emit one jog per pixelsPerJog crossed. The
    // residual lives in the (cumulative) drag distance itself, so no value is
    // cached across events: each jog reads the model fresh.
    auto d = e.getDistanceFromDragStartY();
    bool fine = e.mods.isShiftDown();
    auto pxPer = fine ? pixelsPerJog * shiftJogFactor : pixelsPerJog;
    int wantSteps = (int)((float)(-d) / (float)pxPer);
    int delta = wantSteps - dragStepsEmitted;
    if (delta != 0)
    {
        auto eidg = EditIsDragGuard(*this);
        if (fine)
        {
            // one raw unit per step, bypassing any coarse jog() increment
            auto nv = std::clamp(data->getValue() + delta, data->getMin(), data->getMax());
            data->setValueFromGUI(nv);
        }
        else
        {
            data->jog(delta);
        }
        dragStepsEmitted = wantSteps;
        notifyAccessibleChange();
    }
    repaint();
}

void DraggableTextEditableDiscreteValue::mouseDoubleClick(const juce::MouseEvent &e)
{
    activateEditor();
}

void DraggableTextEditableDiscreteValue::onStyleChanged()
{
    restyleTextEditor(getFont(Styles::labelfont));
}

void DraggableTextEditableDiscreteValue::resized() { layoutTextEditor(getLocalBounds()); }
} // namespace sst::jucegui::components
