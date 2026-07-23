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

#include <sst/jucegui/components/DraggableTextEditableValue.h>
#include <sst/jucegui/util/DebugHelpers.h>

namespace sst::jucegui::components
{

DraggableTextEditableValue::DraggableTextEditableValue()
    : ContinuousParamEditor(Direction::VERTICAL), style::StyleConsumer(Styles::styleClass)
{
    initTextEditor();
}

DraggableTextEditableValue::~DraggableTextEditableValue() = default;

void DraggableTextEditableValue::paint(juce::Graphics &g) { paintFor(this, g); }

void DraggableTextEditableValue::mouseDown(const juce::MouseEvent &e)
{
    if (!mouseDownFor(this, e))
        return;

    valueOnMouseDown = continuous()->getValue();
}
void DraggableTextEditableValue::mouseUp(const juce::MouseEvent &e) { mouseUpFor(this, e); }
void DraggableTextEditableValue::mouseDrag(const juce::MouseEvent &e)
{
    if (!mouseDragFor(this, e))
        return;

    auto d = e.getDistanceFromDragStartY();
    auto fac = dragScale * (e.mods.isShiftDown() ? dragShiftRatio : 1.f);
    auto nv = valueOnMouseDown - fac * d * continuous()->getMinMaxRange() * 0.01f;
    nv = std::clamp(nv, continuous()->getMin(), continuous()->getMax());
    {
        auto eidg = EditIsDragGuard(*this);
        if (e.mods.isCommandDown())
            continuous()->setValueFromGUIQuantized(nv);
        else
            continuous()->setValueFromGUI(nv);
    }
    repaint();
}

void DraggableTextEditableValue::mouseDoubleClick(const juce::MouseEvent &e) { activateEditor(); }

void DraggableTextEditableValue::onStyleChanged() { restyleTextEditor(getFont(Styles::labelfont)); }
void DraggableTextEditableValue::resized() { layoutTextEditor(getLocalBounds()); }
} // namespace sst::jucegui::components
