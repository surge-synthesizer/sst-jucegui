//
// Created by Paul Walker on 5/30/22.
//

#include <sst/jucegui/components/MultiSwitch.h>
#include <sst/jucegui/util/DebugHelpers.h>

namespace sst::jucegui::components
{
MultiSwitch::MultiSwitch() : style::StyleConsumer(Styles::styleClass) {}

MultiSwitch::~MultiSwitch()
{
    if (data)
        data->removeGUIDataListener(this);
}

void MultiSwitch::paint(juce::Graphics &g)
{
    if (!data || data->getMin() == data->getMax())
    {
        g.fillAll(juce::Colours::red);
        return;
    }
    int rectCorner = 3;
    float nItems = data->getMax() - data->getMin();

    auto b = getLocalBounds().reduced(1).toFloat();

    auto bg = getColour(Styles::offbgcol);
    auto fg = getColour(Styles::textoffcol);

    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    g.setColour(getColour(Styles::bordercol));
    g.drawRoundedRectangle(b, rectCorner, 1);

    if (nItems > 0)
    {
        float h = getHeight() / nItems;

        for (int i = 0; i < nItems; ++i)
        {
            auto rule = getLocalBounds().toFloat().reduced(5, 0).withY(h * i - 0.5).withHeight(1);
            g.setColour(getColour(Styles::bordercol));
            g.fillRect(rule);
        }

        for (int i = 0; i <= nItems; ++i)
        {
            auto txt = getLocalBounds().toFloat().withHeight(h).translated(0, h * i);
            bool isH = isHovered && txt.contains(getWidth() / 2, hoverY);

            // Draw the background
            if (i == data->getValue())
            {
                if (isH)
                    g.setColour(getColour(Styles::hoveronbgcol));
                else
                    g.setColour(getColour(Styles::onbgcol));
                g.fillRoundedRectangle(txt.reduced(3, 2), 3);
            }
            else if (isH)
            {
                g.setColour(getColour(Styles::hoveroffbgcol));
                g.fillRoundedRectangle(txt.reduced(3, 2), 3);
            }

            if (i == data->getValue())
            {
                g.setColour(getColour(Styles::textoncol));
                if (isH)
                {
                    g.setColour(getColour(Styles::texthoveroncol));
                }
            }
            else
            {
                g.setColour(getColour(Styles::textoffcol));
                if (isH)
                {
                    g.setColour(getColour(Styles::texthoveroffcol));
                }
            }
            g.setFont(getFont(Styles::labelfont));
            g.drawText(data->getValueAsStringFor(i), txt, juce::Justification::centred);
        }
    }
    /*
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

     */
}

void MultiSwitch::mouseDown(const juce::MouseEvent &e)
{
    onBeginEdit();

    float nItems = data->getMax() - data->getMin();
    float h = getHeight() / nItems;
    int val = (int)(e.y / h);
    data->setValueFromGUI(val);
}

void MultiSwitch::mouseUp(const juce::MouseEvent &e)
{
    onEndEdit();
    repaint();
}

void MultiSwitch::dataChanged() { repaint(); }

void MultiSwitch::mouseMove(const juce::MouseEvent &e)
{
    hoverY = e.y;
    repaint();
}
void MultiSwitch::mouseDrag(const juce::MouseEvent &e)
{
    hoverY = e.y;

    float nItems = data->getMax() - data->getMin();
    float h = getHeight() / nItems;
    int val = (int)(e.y / h);
    if (val != data->getValue())
        data->setValueFromGUI(val);

    repaint();
}

} // namespace sst::jucegui::components