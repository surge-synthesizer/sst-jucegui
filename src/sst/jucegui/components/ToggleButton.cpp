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

#include <sst/jucegui/components/ToggleButton.h>
#include "sst/jucegui/components/ButtonPainter.h"

namespace sst::jucegui::components
{
ToggleButton::ToggleButton() : style::StyleConsumer(Styles::styleClass) {}

ToggleButton::~ToggleButton()
{
    if (data)
    {
        data->removeGUIDataListener(this);
    }
}

void ToggleButton::paint(juce::Graphics &g)
{
    bool v = data ? data->getValue() : false;

    if (drawMode != DrawMode::GLYPH && drawMode != DrawMode::DUAL_GLYPH &&
        drawMode != DrawMode::FILLED)
    {
        v = v && (drawMode != DrawMode::LABELED_BY_DATA);

        if (v)
        {
            paintButtonOnValueBG(this, g);
        }
        else
        {
            paintButtonBG(this, g);
        }
    }

    if (drawMode == DrawMode::FILLED)
    {
        if (isHovered)
        {
            if (v)
                g.setColour(getColour(Styles::labelcolor_hover));
            else
                g.setColour(getColour(Styles::background_hover));
        }
        else
        {
            if (v)
                g.setColour(getColour(Styles::labelcolor));
            else
                g.setColour(getColour(Styles::background));
        }

        auto b = getLocalBounds().reduced(2).toFloat();
        g.fillRoundedRectangle(b, 1.f); // corner size should match those in ButtonPainter.h

        // Draw outline
        g.setColour(getColour(Styles::brightoutline));
        g.drawRoundedRectangle(b, 1.f, 1.f);
    }

    if (isHovered)
    {
        if (v)
            g.setColour(getColour(Styles::value_hover));
        else
            g.setColour(getColour(Styles::labelcolor_hover));
    }
    else
    {
        if (v)
            g.setColour(getColour(Styles::value));
        else
            g.setColour(getColour(Styles::labelcolor));
    }

    if (drawMode == DrawMode::GLYPH)
    {
        juce::Colour col = juce::Colours::red;
        if (v)
        {
            if (isHovered)
            {
                col = getColour(Styles::value_hover);
            }
            else
            {
                col = getColour(Styles::value);
            }
        }
        else
        {
            if (isHovered)
            {
                col = getColour(Styles::labelcolor_hover);
            }
            else
            {
                col = getColour(Styles::labelcolor);
            }
        }

        auto paintType = type;
        if (type == GlyphPainter::SMALL_POWER_LIGHT && !v && !isHovered)
        {
            paintType = GlyphPainter::SMALL_POWER_LIGHT_OFF;
        }
        GlyphPainter::paintGlyph(g, getLocalBounds(), paintType, col);
        return;
    }

    if (drawMode == DrawMode::DUAL_GLYPH)
    {
        auto col = juce::Colours::red;
        if (isHovered)
        {
            col = getColour(Styles::value_hover);
        }
        else
        {
            col = getColour(Styles::value);
        }

        GlyphPainter::paintGlyph(g, getLocalBounds(), v ? type : offType, col);
        return;
    }

    if (drawMode == DrawMode::LABELED)
    {
        g.setFont(getFont(Styles::labelfont));
        g.drawText(label, getLocalBounds(), juce::Justification::centred);
    }

    if (drawMode == DrawMode::LABELED_BY_DATA)
    {
        auto dlab = data ? data->getValueAsString() : "ERROR";
        g.setFont(getFont(Styles::labelfont));
        g.drawText(dlab, getLocalBounds(), juce::Justification::centred);
    }
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