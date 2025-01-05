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

#include <sst/jucegui/components/DiscreteParamEditor.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/TypeInOverlay.h>
#include <algorithm>

namespace sst::jucegui::components
{

bool DiscreteParamEditor::keyPressed(const juce::KeyPress &k)
{
    auto a = this->accessibleEdit(k);

    using act = sst::jucegui::accessibility::AccessibilityKeyboardEditSupport<DiscreteParamEditor>;

    if (!a.isNone())
    {
        if (!data)
            return false;
        switch (a.action)
        {
        case act::Action::ToMax:
            onBeginEdit();
            data->setValueFromGUI(data->getMax());
            repaint();
            notifyAccessibleChange();
            onEndEdit();
            return true;
        case act::Action::ToMin:
            onBeginEdit();
            data->setValueFromGUI(data->getMin());
            repaint();
            notifyAccessibleChange();
            onEndEdit();
            return true;
        case act::Action::ToDefault:
            onBeginEdit();
            data->setValueFromGUI(data->getDefaultValue());
            repaint();
            notifyAccessibleChange();
            onEndEdit();
            return true;

        case act::Action::Increase:
        case act::Action::Decrease:
        {
            auto jog = -1;
            if (a.action == act::Action::Increase)
            {
                jog = 1;
            }

            auto nv = data->getValue() + jog;
            if (nv > data->getMax())
                nv = data->getMin();
            if (nv < data->getMin())
                nv = data->getMax();

            onBeginEdit();
            data->setValueFromGUI(nv);
            repaint();
            notifyAccessibleChange();
            onEndEdit();
        }
        break;
        case act::Action::OpenMenu:
            showPopup(juce::ModifierKeys());
            break;
        default:
            std::cout << __FILE__ << ":" << __LINE__ << " Unused Accessible Action" << std::endl;
            break;
        }
    }
    return false;
}

// Accessibility
struct DiscreteParamEditorAH : public juce::AccessibilityHandler
{
    struct DPEValue : public juce::AccessibilityValueInterface
    {
        explicit DPEValue(DiscreteParamEditor *s) : slider(s) {}

        DiscreteParamEditor *slider;

        bool isReadOnly() const override { return false; }
        double getCurrentValue() const override { return slider->data->getValue(); }
        void setValue(double newValue) override
        {
            slider->onBeginEdit();
            slider->data->setValueFromGUI(newValue);
            slider->notifyAccessibleChange();
            slider->repaint();
            slider->onEndEdit();
        }
        juce::String getCurrentValueAsString() const override
        {
            return slider->data->getValueAsString();
        }
        void setValueAsString(const juce::String &newValue) override
        {
            slider->onBeginEdit();
            slider->data->setValueAsString(newValue.toStdString());
            slider->notifyAccessibleChange();
            slider->repaint();
            slider->onEndEdit();
        }
        AccessibleValueRange getRange() const override
        {
            return {{(double)slider->data->getMin(), (double)slider->data->getMax()}, 1};
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DPEValue);
    };

    explicit DiscreteParamEditorAH(DiscreteParamEditor *s)
        : slider(s),
          juce::AccessibilityHandler(
              *s, s->getAccessibleRole(),
              juce::AccessibilityActions().addAction(juce::AccessibilityActionType::showMenu,
                                                     [this]() { this->showMenu(); }),
              AccessibilityHandler::Interfaces{std::make_unique<DPEValue>(s)})
    {
    }

    void resetToDefault()
    {
        slider->data->setValueFromGUI(slider->data->getDefaultValue());
        slider->repaint();
        slider->notifyAccessibleChange();
    }

    void showMenu() {}

    DiscreteParamEditor *slider;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DiscreteParamEditorAH);
};

std::unique_ptr<juce::AccessibilityHandler> DiscreteParamEditor::createAccessibilityHandler()
{
    return std::make_unique<DiscreteParamEditorAH>(this);
}

void DiscreteParamEditor::notifyAccessibleChange()
{
    if (auto h = getAccessibilityHandler())
    {
        h->notifyAccessibilityEvent(juce::AccessibilityEvent::valueChanged);
    }
}
// end
} // namespace sst::jucegui::components