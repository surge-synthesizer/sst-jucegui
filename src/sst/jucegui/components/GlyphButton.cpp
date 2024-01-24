/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
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
#include "sst/jucegui/components/ButtonPainter.h"

namespace sst::jucegui::components
{
GlyphButton::GlyphButton(GlyphPainter::GlyphType t)
    : style::StyleConsumer(Styles::styleClass), glyph(t)
{
}

GlyphButton::~GlyphButton() {}

void GlyphButton::paint(juce::Graphics &g)
{
    paintButtonBG(this, g);
    if (isHovered)
        g.setColour(getColour(Styles::labelcolor_hover));
    else
        g.setColour(getColour(Styles::labelcolor));
    GlyphPainter::paintGlyph(g, getLocalBounds().reduced(glyphButtonPad), glyph);
}
} // namespace sst::jucegui::components