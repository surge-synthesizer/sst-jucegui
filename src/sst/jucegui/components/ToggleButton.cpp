//
// Created by Paul Walker on 5/30/22.
//

#include <sst/jucegui/components/ToggleButton.h>

namespace sst::jucegui::components
{
ToggleButton::ToggleButton() : style::StyleConsumer(Styles::styleClass) {}

ToggleButton::~ToggleButton()
{
    if (data)
        data->removeGUIDataListener(this);
}

void ToggleButton::paint(juce::Graphics &g)
{
    int rectCorner = 3;

    auto b = getLocalBounds().reduced(1).toFloat();
    bool v = data ? data->getValue() : false;

    auto bg = getColour(Styles::offbgcol);
    auto fg = getColour(Styles::textoffcol);
    if (v)
    {
        if (isHovered)
        {
            bg = getColour(Styles::hoveronbgcol);
            fg = getColour(Styles::texthoveroncol);
        }
        else
        {
            bg = getColour(Styles::onbgcol);
            fg = getColour(Styles::texthoveroncol);
        }
    }
    else if (isHovered)
    {
        bg = getColour(Styles::hoveroffbgcol);
        fg = getColour(Styles::texthoveroffcol);
    }

    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    g.setFont(getFont(Styles::labelfont));
    g.setColour(fg);
    g.drawText(label, b, juce::Justification::centred);

    g.setColour(getColour(Styles::bordercol));
    g.drawRoundedRectangle(b, rectCorner, 1);
}

void ToggleButton::mouseDown(const juce::MouseEvent &e) { onBeginEdit(); }

void ToggleButton::mouseUp(const juce::MouseEvent &e)
{
    if (data)
        data->setValueFromGUI(!data->getValue());
    onEndEdit();
    repaint();
}

void ToggleButton::dataChanged() { repaint(); }
} // namespace sst::jucegui::components