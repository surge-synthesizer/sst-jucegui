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
    auto col = juce::Colours::red;
    if (isHovered)
        col = getColour(Styles::labelcolor_hover);
    else
        col = getColour(Styles::labelcolor);
    GlyphPainter::paintGlyph(g, getLocalBounds().reduced(glyphButtonPad), glyph, col);
}
} // namespace sst::jucegui::components