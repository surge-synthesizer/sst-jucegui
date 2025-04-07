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
DraggableTextEditableValue::DraggableTextEditableValue() : style::StyleConsumer(Styles::styleClass)
{
    underlyingEditor = std::make_unique<juce::TextEditor>();

    underlyingEditor->onEscapeKey = [sp = juce::Component::SafePointer(this)] {
        if (sp)
            sp->underlyingEditor->setVisible(false);
    };
    underlyingEditor->onFocusLost = [sp = juce::Component::SafePointer(this)] {
        if (sp && sp->underlyingEditor->isVisible())
            sp->setFromEditor();
    };
    underlyingEditor->onReturnKey = [sp = juce::Component::SafePointer(this)] {
        if (sp)
            sp->setFromEditor();
    };

    addChildComponent(*underlyingEditor);
}

void DraggableTextEditableValue::setFromEditor()
{
    jassert(underlyingEditor->isVisible());
    auto t = underlyingEditor->getText();
    if (t.isEmpty())
    {
        continuous()->setValueFromGUI(continuous()->getDefaultValue());
    }
    else
    {
        continuous()->setValueAsString(t.toStdString());
    }
    underlyingEditor->setVisible(false);
    repaint();
}

void DraggableTextEditableValue::paint(juce::Graphics &g)
{
    g.setColour(getColour(Styles::background));
    if (isHovered)
        g.setColour(getColour(Styles::background_hover));
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.f);

    if (continuous() && !underlyingEditor->isVisible())
    {
        g.setFont(getFont(Styles::labelfont));
        g.setColour(
            getColour(Styles::value)); // on Hover, the text colour is intensionally the same.
        if (displayUnits)
        {
            g.drawText(continuous()->getValueAsString(), getLocalBounds(),
                       juce::Justification::centred);
        }
        else
        {
            g.drawText(continuous()->getValueAsStringWithoutUnits(), getLocalBounds(),
                       juce::Justification::centred);
        }
    }
}

void DraggableTextEditableValue::mouseDown(const juce::MouseEvent &e)
{
    onBeginEdit();
    valueOnMouseDown = continuous()->getValue();
}
void DraggableTextEditableValue::mouseUp(const juce::MouseEvent &e) { onEndEdit(); }
void DraggableTextEditableValue::mouseDrag(const juce::MouseEvent &e)
{
    auto d = e.getDistanceFromDragStartY();
    auto fac = 0.5f * (e.mods.isShiftDown() ? 0.1f : 1.f);
    auto nv = valueOnMouseDown - fac * d * continuous()->getMinMaxRange() * 0.01f;
    nv = std::clamp(nv, continuous()->getMin(), continuous()->getMax());
    continuous()->setValueFromGUI(nv);
    repaint();
}
void DraggableTextEditableValue::mouseWheelMove(const juce::MouseEvent &event,
                                                const juce::MouseWheelDetails &wheel)
{
    DBGOUT("IMPLEMENT MOUSE WHEEL");
};

void DraggableTextEditableValue::mouseDoubleClick(const juce::MouseEvent &e)
{
    if (displayUnits)
    {
        underlyingEditor->setText(continuous()->getValueAsString());
    }
    else
    {
        underlyingEditor->setText(continuous()->getValueAsStringWithoutUnits());
    }

    underlyingEditor->setVisible(true);
    underlyingEditor->selectAll();
    underlyingEditor->grabKeyboardFocus();
}

void DraggableTextEditableValue::onStyleChanged()
{
    underlyingEditor->setColour(juce::TextEditor::ColourIds::outlineColourId,
                                juce::Colours::black.withAlpha(0.f));
    underlyingEditor->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
                                juce::Colours::black.withAlpha(0.f));
    underlyingEditor->setColour(juce::TextEditor::ColourIds::backgroundColourId,
                                juce::Colours::black.withAlpha(0.f));
    underlyingEditor->setFont(getFont(Styles::labelfont));
    underlyingEditor->setIndents(0, 0);
    underlyingEditor->setJustification(juce::Justification::centred);
}
void DraggableTextEditableValue::resized() { underlyingEditor->setBounds(getLocalBounds()); }
} // namespace sst::jucegui::components