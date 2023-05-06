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

#include "sst/jucegui/components/GlyphButton.h"
#include "sst/jucegui/components/GlyphPainter.h"
#include "sst/jucegui/components/MenuButton.h"

namespace sst::jucegui::components
{
GlyphButton::GlyphButton(GlyphPainter::GlyphType t)
    : style::StyleConsumer(Styles::styleClass), glyph(t)
{
}

GlyphButton::~GlyphButton() {}

void GlyphButton::paint(juce::Graphics &g)
{
    float rectCorner = 1.5;

    auto b = getLocalBounds().reduced(1).toFloat();
    auto v = !isInactive;

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

    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    g.setColour(fg);
    GlyphPainter::paintGlyph(g, getLocalBounds(), glyph);

    g.setColour(getColour(Styles::bordercol));
    g.drawRoundedRectangle(b, rectCorner, 1);
}
} // namespace sst::jucegui::components