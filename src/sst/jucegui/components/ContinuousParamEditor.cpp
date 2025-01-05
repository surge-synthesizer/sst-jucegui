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

#include <sst/jucegui/components/ContinuousParamEditor.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/TypeInOverlay.h>
#include <algorithm>

namespace sst::jucegui::components
{
ContinuousParamEditor::ContinuousParamEditor(Direction dir) : direction(dir)
{
    setAccessible(true);
    setWantsKeyboardFocus(true);
    setTitle("UnNamed Continuous");
}
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
    notifyAccessibleChange();
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
        if (e.mods.isCommandDown())
        {
            continuous()->setValueFromGUIQuantized(vn);
        }
        else
        {
            continuous()->setValueFromGUI(vn);
        }
        notifyAccessibleChange();
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
#if JUCEGUI_WIN || JUCEGUI_LIN
        d *= 0.025;
#endif

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
#if JUCEGUI_WIN || JUCEGUI_LIN
        d *= 0.025;
#endif

        if (e.mods.isShiftDown())
            d = d * 0.1;

        auto vn = std::clamp(continuous()->getValue() + d, continuous()->getMin(),
                             continuous()->getMax());
        continuous()->setValueFromGUI(vn);
        notifyAccessibleChange();
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

    return true;
}

bool ContinuousParamEditor::keyPressed(const juce::KeyPress &k)
{
    auto a = this->accessibleEdit(k);

    using act =
        sst::jucegui::accessibility::AccessibilityKeyboardEditSupport<ContinuousParamEditor>;

    if (!a.isNone())
    {
        if (!continuous())
            return false;
        switch (a.action)
        {
        case act::Action::ToMax:
            onBeginEdit();
            continuous()->setValueFromGUI(continuous()->getMax());
            notifyAccessibleChange();
            repaint();
            onEndEdit();
            return true;
        case act::Action::ToMin:
            onBeginEdit();
            continuous()->setValueFromGUI(continuous()->getMin());
            notifyAccessibleChange();
            repaint();
            onEndEdit();
            return true;
        case act::Action::ToDefault:
            onBeginEdit();
            continuous()->setValueFromGUI(continuous()->getDefaultValue());
            notifyAccessibleChange();
            repaint();
            onEndEdit();
            return true;

        case act::Action::Increase:
        case act::Action::Decrease:
        {
            auto range = (continuous()->getMax() - continuous()->getMin());
            auto sgn = a.action == act::Action::Increase ? 1.f : -1.f;
            auto delt = sgn * range * 0.025f;
            if (a.mod == act::Action::Fine)
                delt *= 0.1;

            auto vn = std::clamp(continuous()->getValue() + delt, continuous()->getMin(),
                                 continuous()->getMax());
            if (a.mod == act::Action::Quantized)
            {
                vn = std::clamp(continuous()->getValue() +
                                    continuous()->getQuantizedStepSize() * sgn,
                                continuous()->getMin(), continuous()->getMax());
                vn = continuous()->quantizeValue(vn);
            }
            onBeginEdit();
            continuous()->setValueFromGUI(vn);
            repaint();
            notifyAccessibleChange();
            onEndEdit();
        }
        break;
        case act::Action::OpenEditor:
            initiateTypeIn();
            break;
        case act::Action::OpenMenu:
            if (onPopupMenu)
                onPopupMenu(juce::ModifierKeys());
            break;
        default:
            std::cout << __FILE__ << ":" << __LINE__ << " Unused Accessible Action" << std::endl;
            break;
        }
    }
    return false;
}

// Accessibility
struct ContinuousParamEditorAH : public juce::AccessibilityHandler
{
    struct CPEValue : public juce::AccessibilityValueInterface
    {
        explicit CPEValue(ContinuousParamEditor *s) : slider(s) {}

        ContinuousParamEditor *slider;

        bool isReadOnly() const override { return false; }
        double getCurrentValue() const override { return slider->continuous()->getValue(); }
        void setValue(double newValue) override
        {
            slider->onBeginEdit();
            slider->continuous()->setValueFromGUI(newValue);
            slider->notifyAccessibleChange();
            slider->onEndEdit();
        }
        juce::String getCurrentValueAsString() const override
        {
            return slider->continuous()->getValueAsString();
        }
        void setValueAsString(const juce::String &newValue) override
        {
            slider->onBeginEdit();
            slider->continuous()->setValueAsString(newValue.toStdString());
            slider->notifyAccessibleChange();
            slider->onEndEdit();
        }
        AccessibleValueRange getRange() const override
        {
            return {{slider->continuous()->getMin(), slider->continuous()->getMax()},
                    slider->continuous()->getMinMaxRange() * 0.01};
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CPEValue);
    };

    explicit ContinuousParamEditorAH(ContinuousParamEditor *s)
        : slider(s),
          juce::AccessibilityHandler(
              *s, juce::AccessibilityRole::slider,
              juce::AccessibilityActions().addAction(juce::AccessibilityActionType::showMenu,
                                                     [this]() { this->showMenu(); }),
              AccessibilityHandler::Interfaces{std::make_unique<CPEValue>(s)})
    {
    }

    void resetToDefault()
    {
        slider->continuous()->setValueFromGUI(slider->continuous()->getDefaultValue());
        slider->notifyAccessibleChange();
    }

    void showMenu() {}

    ContinuousParamEditor *slider;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContinuousParamEditorAH);
};

std::unique_ptr<juce::AccessibilityHandler> ContinuousParamEditor::createAccessibilityHandler()
{
    return std::make_unique<ContinuousParamEditorAH>(this);
}

void ContinuousParamEditor::notifyAccessibleChange()
{
    if (auto h = getAccessibilityHandler())
    {
        h->notifyAccessibilityEvent(juce::AccessibilityEvent::valueChanged);
    }
}

void ContinuousParamEditor::initiateTypeIn()
{
    auto *c = getParentComponent();
    auto b = getBounds();

    while (c && !dynamic_cast<sst::jucegui::components::NamedPanel *>(c))
    {
        b = b.translated(c->getBounds().getX(), c->getBounds().getY());
        c = c->getParentComponent();
    }

    if (c)
    {
        auto np = dynamic_cast<sst::jucegui::components::NamedPanel *>(c);
        if (!np)
        {
            return;
        }

        auto ti = std::make_unique<TypeInOverlay>(this);
        typeInComponent = std::move(ti);

        auto ticB = b.withWidth(90).withHeight(42);
        if (!np->getLocalBounds().contains(ticB))
        {
            // since we are at the upper corner of the thing we only need to move
            // left and up
            if (ticB.getRight() > np->getLocalBounds().getWidth())
            {
                ticB = ticB.translated(-(ticB.getRight() - np->getLocalBounds().getWidth() + 2), 0);
            }

            if (ticB.getBottom() > np->getLocalBounds().getHeight())
            {
                ticB =
                    ticB.translated(0, -(ticB.getBottom() - np->getLocalBounds().getHeight() + 2));
            }
        }
        typeInComponent->setBounds(ticB);
        np->addAndMakeVisible(*typeInComponent);
        typeInComponent->grabKeyboardFocus();
    }
}

void ContinuousParamEditor::dismissTypeIn()
{
    removeChildComponent(typeInComponent.get());
    grabKeyboardFocus();
    juce::Timer::callAfterDelay(0, [w = juce::Component::SafePointer(this)]() {
        if (w)
            w->typeInComponent.reset();
    });
}
// end
} // namespace sst::jucegui::components