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

#include <sst/jucegui/components/ContinuousParamEditor.h>

namespace sst::jucegui::components
{
ContinuousParamEditor::ContinuousParamEditor(Direction dir) : direction(dir) {}
ContinuousParamEditor::~ContinuousParamEditor() = default;

void ContinuousParamEditor::mouseDown(const juce::MouseEvent &e)
{
    if (!processMouseActions())
        return;

    jassert(settings());
    jassert(continuous());

    if (e.mods.isPopupMenu())
    {
        mouseMode = POPUP;
        onPopupMenu(e.mods);
        return;
    }

    mouseMode = DRAG;
    onBeginEdit();
    if (isEditingMod && continuousModulatable())
        mouseDownV0 = continuousModulatable()->getModulationValuePM1();
    else
        mouseDownV0 = continuous()->getValue();
    mouseDownY0 = e.position.y;
    mouseDownX0 = e.position.x;
}
void ContinuousParamEditor::mouseUp(const juce::MouseEvent &e)
{
    if (!processMouseActions())
        return;

    if (mouseMode == DRAG)
        onEndEdit();
    mouseMode = NONE;
}

void ContinuousParamEditor::mouseDoubleClick(const juce::MouseEvent &e)
{
    if (!processMouseActions())
        return;

    onBeginEdit();
    continuous()->setValueFromGUI(continuous()->getDefaultValue());
    onEndEdit();

    repaint();
}

void ContinuousParamEditor::mouseDrag(const juce::MouseEvent &e)
{
    if (!processMouseActions())
        return;

    if (mouseMode != DRAG)
        return;

    float dy = -(e.position.y - mouseDownY0);
    float dx = (e.position.x - mouseDownX0);
    float d = 0;
    float minForScaling = continuous()->getMin();
    float maxForScaling = continuous()->getMax();
    if (isEditingMod && continuousModulatable())
    {
        if (continuousModulatable()->isModulationBipolar())
            minForScaling = -1.0f;
        else
            minForScaling = 0.0f;
        maxForScaling = 1.0f;
    }
    if (direction == VERTICAL)
    {
        d = (dx * 0.1 + dy) / 150.0 * (maxForScaling - minForScaling);
    }
    else
    {
        // getWidth probably isn't exactly right here, but better than the constant
        d = (dx + 0.1 * dy) / (float)getWidth() * (maxForScaling - minForScaling);
    }
    if (e.mods.isShiftDown())
        d = d * 0.1;
    if (isEditingMod && continuousModulatable())
    {
        if (continuousModulatable()->isModulationBipolar())
            d = d * 0.5;
        auto vn = std::clamp(mouseDownV0 + d, -1.f, 1.f);
        continuousModulatable()->setModulationValuePM1(vn);
        mouseDownV0 = vn;
    }
    else
    {
        auto vn = std::clamp(mouseDownV0 + d, continuous()->getMin(), continuous()->getMax());
        continuous()->setValueFromGUI(vn);
        mouseDownV0 = vn;
    }
    mouseDownX0 = e.position.x;
    mouseDownY0 = e.position.y;

    repaint();
}
void ContinuousParamEditor::mouseWheelMove(const juce::MouseEvent &e,
                                           const juce::MouseWheelDetails &wheel)
{
    if (!processMouseActions())
        return;

    if (fabs(wheel.deltaY) < 0.0001)
        return;
    onBeginEdit();

    if (isEditingMod && continuousModulatable())
    {
        // fixme - callibration and sharing
        auto d = (wheel.isReversed ? -1 : 1) * wheel.deltaY * (2);
        if (e.mods.isShiftDown())
            d = d * 0.1;

        auto vn = std::clamp(continuousModulatable()->getModulationValuePM1() + d, -1.f, 1.f);
        continuousModulatable()->setModulationValuePM1(vn);
    }
    else
    {
        // fixme - callibration and sharing
        auto d = (wheel.isReversed ? -1 : 1) * wheel.deltaY *
                 (continuous()->getMax() - continuous()->getMin());
        if (e.mods.isShiftDown())
            d = d * 0.1;

        auto vn = std::clamp(continuous()->getValue() + d, continuous()->getMin(),
                             continuous()->getMax());
        continuous()->setValueFromGUI(vn);
    }
    onEndEdit();
    repaint();
}

bool ContinuousParamEditor::processMouseActions()
{
    if (!continuous())
        return false;
    if (continuous()->isHidden())
        return false;
    if (!isEnabled())
        return false;

    return true;
}
} // namespace sst::jucegui::components