//
// Created by Paul Walker on 8/7/22.
//

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
    source->setValueAsString(t.toStdString());
    underlyingEditor->setVisible(false);
    repaint();
}

void DraggableTextEditableValue::paint(juce::Graphics &g)
{
    if (underlyingEditor->isVisible())
    {
        g.setColour(getColour(Styles::bgedcol));
    }
    else if (isHovered)
    {
        g.setColour(getColour(Styles::bghovcol));
    }
    else
    {
        g.setColour(getColour(Styles::bgcol));
    }
    g.fillRoundedRectangle(getLocalBounds().toFloat(), 3.f);
    g.setColour(getColour(Styles::outlinecol));
    g.drawRoundedRectangle(getLocalBounds().toFloat(), 3.f, 1.f);
    if (source && !underlyingEditor->isVisible())
    {
        g.setFont(getFont(Styles::textfont));
        g.setColour(getColour(Styles::textcol));
        g.drawText(source->getValueAsString(), getLocalBounds(), juce::Justification::centred);
    }
}

void DraggableTextEditableValue::mouseDown(const juce::MouseEvent &e)
{
    onBeginEdit();
    valueOnMouseDown = source->getValue();
}
void DraggableTextEditableValue::mouseUp(const juce::MouseEvent &e) { onEndEdit(); }
void DraggableTextEditableValue::mouseDrag(const juce::MouseEvent &e)
{
    auto d = e.getDistanceFromDragStartY();
    auto fac = 0.5f * (e.mods.isShiftDown() ? 0.1f : 1.f);
    auto nv = valueOnMouseDown - fac * d * source->getFineQuantizedStepSize();
    nv = std::clamp(nv, source->getMin(), source->getMax());
    source->setValueFromGUI(nv);
    repaint();
}
void DraggableTextEditableValue::mouseWheelMove(const juce::MouseEvent &event,
                                                const juce::MouseWheelDetails &wheel)
{
    DBGOUT("IMPLEMENT MOUSE WHEEL");
};

void DraggableTextEditableValue::mouseDoubleClick(const juce::MouseEvent &e)
{
    underlyingEditor->setText(source->getValueAsString());
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
    underlyingEditor->setFont(getFont(Styles::textfont));
    underlyingEditor->setIndents(0, 0);
    underlyingEditor->setJustification(juce::Justification::centred);
}
void DraggableTextEditableValue::resized() { underlyingEditor->setBounds(getLocalBounds()); }
} // namespace sst::jucegui::components