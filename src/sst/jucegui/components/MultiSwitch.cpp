/*
 * sst-juce-guil - an open source library of juce widgets
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

#include <sst/jucegui/components/MultiSwitch.h>
#include <sst/jucegui/util/DebugHelpers.h>

namespace sst::jucegui::components
{
MultiSwitch::MultiSwitch(Direction d) : direction(d), style::StyleConsumer(Styles::styleClass) {}

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
    if (data->isHidden())
        return;

    int rectCorner = 3;
    float nItems = data->getMax() - data->getMin();

    auto b = getLocalBounds().reduced(1).toFloat();

    if (nItems > 0)
    {
        float h = std::min(b.getHeight() * 1.f / nItems, elementSize * 1.f);
        if (direction == HORIZONTAL)
            h = std::min(b.getWidth() * 1.f / nItems, elementSize * 1.f);

        auto bgb = b.withHeight(h * nItems);
        if (direction == HORIZONTAL)
            bgb = b.withWidth(h * nItems);
        auto bg = getColour(Styles::offbgcol);
        auto fg = getColour(Styles::textoffcol);

        g.setColour(bg);
        g.fillRoundedRectangle(bgb, rectCorner);

        g.setColour(getColour(Styles::bordercol));
        g.drawRoundedRectangle(bgb, rectCorner, 1);

        for (int i = 0; i < nItems; ++i)
        {
            juce::Rectangle<float> rule;
            if (direction == VERTICAL)
                rule = b.toFloat().reduced(5, 0).withY(h * i - 0.5).withHeight(1);
            else
                rule = b.toFloat().reduced(0, 2).withX(h * i - 0.5).withWidth(1);

            g.setColour(getColour(Styles::bordercol));
            g.fillRect(rule);
        }

        for (int i = 0; i < nItems; ++i)
        {
            juce::Rectangle<float> txt, txtbg;

            if (direction == VERTICAL)
            {
                txt = b.toFloat().withHeight(h).translated(0, h * i);
                txtbg = txt.reduced(3, 2);
            }
            else
            {
                txt = b.toFloat().withWidth(h).translated(h * i, 0);
                txtbg = txt.reduced(3, 3);
            }

            bool isH;
            if (direction == VERTICAL)
                isH = isHovered && txt.contains(getWidth() / 2, hoverY);
            else
                isH = isHovered && txt.contains(hoverX, getHeight() / 2);

            // Draw the background
            if (i == data->getValue())
            {
                if (isH)
                    g.setColour(getColour(Styles::hoveronbgcol));
                else
                    g.setColour(getColour(Styles::onbgcol));
                g.fillRoundedRectangle(txtbg, 3);
            }
            else if (isH)
            {
                g.setColour(getColour(Styles::hoveroffbgcol));
                g.fillRoundedRectangle(txtbg, 3);
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
}

void MultiSwitch::setValueFromMouse(const juce::MouseEvent &e)
{
    int val = data->getValue();
    if (direction == VERTICAL)
    {
        float nItems = data->getMax() - data->getMin();
        float h = std::min(getHeight() / nItems, elementSize * 1.f);
        val = (int)(e.y / h);
    }
    else
    {
        float nItems = data->getMax() - data->getMin();
        float h = std::min(getWidth() / nItems, elementSize * 1.f);
        val = (int)(e.x / h);
    }
    if (val != data->getValue())
        data->setValueFromGUI(val);
}
void MultiSwitch::mouseDown(const juce::MouseEvent &e)
{
    if (data && data->isHidden())
        return;

    didPopup = false;
    if (e.mods.isPopupMenu())
    {
        didPopup = true;
        onPopupMenu(e.mods);
        return;
    }

    onBeginEdit();
    setValueFromMouse(e);
}

void MultiSwitch::mouseUp(const juce::MouseEvent &e)
{
    if (data && data->isHidden())
        return;
    if (!didPopup)
        onEndEdit();
    repaint();
}

void MultiSwitch::dataChanged() { repaint(); }

void MultiSwitch::mouseMove(const juce::MouseEvent &e)
{
    if (data && data->isHidden())
        return;
    hoverY = e.y;
    hoverX = e.x;
    repaint();
}
void MultiSwitch::mouseDrag(const juce::MouseEvent &e)
{
    if (data && data->isHidden())
        return;
    hoverY = e.y;
    hoverX = e.x;

    if (!didPopup)
        setValueFromMouse(e);
    repaint();
}

} // namespace sst::jucegui::components