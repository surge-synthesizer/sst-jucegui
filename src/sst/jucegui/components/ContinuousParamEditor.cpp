//
// Created by Paul Walker on 5/26/22.
//

#include <sst/jucegui/components/ContinuousParamEditor.h>

namespace sst::jucegui::components
{
ContinuousParamEditor::ContinuousParamEditor() {}
ContinuousParamEditor::~ContinuousParamEditor() = default;

void ContinuousParamEditor::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isPopupMenu())
    {
        mouseMode = POPUP;
        onPopupMenu(e.mods);
        return;
    }

    jassert(settings());
    jassert(source);
    mouseMode = DRAG;
    onBeginEdit();
    if (isEditingMod)
        mouseDownV0 = source->getModulationValuePM1();
    else
        mouseDownV0 = source->getValue();
    mouseDownY0 = e.position.y;
}
void ContinuousParamEditor::mouseUp(const juce::MouseEvent &e)
{
    if (mouseMode == DRAG)
        onEndEdit();
    mouseMode = NONE;
}
void ContinuousParamEditor::mouseDrag(const juce::MouseEvent &e)
{
    if (mouseMode != DRAG)
        return;

    float d = -(e.position.y - mouseDownY0) / 150.0 * (source->getMax() - source->getMin());
    if (e.mods.isShiftDown())
        d = d * 0.1;
    if (isEditingMod)
    {
        if (source->isModulationBipolar())
            d = d * 0.5;
        auto vn = std::clamp(mouseDownV0 + d, -1.f, 1.f);
        source->setModulationValuePM1(vn);
    }
    else
    {
        auto vn = std::clamp(mouseDownV0 + d, source->getMin(), source->getMax());
        source->setValueFromGUI(vn);
    }
    repaint();
}
void ContinuousParamEditor::mouseWheelMove(const juce::MouseEvent &e,
                                           const juce::MouseWheelDetails &wheel)
{
    if (fabs(wheel.deltaY) < 0.0001)
        return;
    onBeginEdit();

    if (isEditingMod)
    {
        // fixme - callibration and sharing
        auto d = (wheel.isReversed ? -1 : 1) * wheel.deltaY * (2);
        if (e.mods.isShiftDown())
            d = d * 0.1;

        auto vn = std::clamp(source->getModulationValuePM1() + d, -1.f, 1.f);
        source->setModulationValuePM1(vn);
    }
    else
    {
        // fixme - callibration and sharing
        auto d = (wheel.isReversed ? -1 : 1) * wheel.deltaY * (source->getMax() - source->getMin());
        if (e.mods.isShiftDown())
            d = d * 0.1;

        auto vn = std::clamp(source->getValue() + d, source->getMin(), source->getMax());
        source->setValueFromGUI(vn);
    }
    onEndEdit();
    repaint();
}
} // namespace sst::jucegui::components