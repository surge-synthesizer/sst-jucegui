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
    float rectCorner = 1.5;

    if (label.empty() && data)
        label = data->getLabel();

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
            fg = getColour(Styles::textoncol);
        }
    }
    else if (isHovered)
    {
        bg = getColour(Styles::hoveroffbgcol);
        fg = getColour(Styles::texthoveroffcol);
    }

    if (drawMode == DrawMode::GLYPH)
    {
        if (v)
            g.setColour(getColour(Styles::onbgcol));
        else
            g.setColour(getColour(Styles::onbgcol).withSaturation(0.2).withAlpha(0.5f));
        GlyphPainter::paintGlyph(g, getLocalBounds(), type);
        return;
    }

    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    if (drawMode == DrawMode::LABELED)
    {
        g.setFont(getFont(Styles::labelfont));
        g.setColour(fg);
        g.drawText(label, b, juce::Justification::centred);
    }
    if (v)
        g.setColour(getColour(Styles::borderoncol));
    else
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

} // namespace sst::jucegui::components