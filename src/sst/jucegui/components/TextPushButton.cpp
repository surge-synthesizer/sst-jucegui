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

#include "sst/jucegui/components/TextPushButton.h"

namespace sst::jucegui::components
{
TextPushButton::TextPushButton() : style::StyleConsumer(Styles::styleClass) {}

TextPushButton::~TextPushButton() {}

void TextPushButton::paint(juce::Graphics &g)
{
    float rectCorner = 1.5;

    auto b = getLocalBounds().reduced(1).toFloat();

    auto bg = getColour(Styles::offbgcol);
    auto fg = getColour(Styles::textoffcol);

    if (isHovered)
    {
        bg = getColour(Styles::hoveronbgcol);
        fg = getColour(Styles::texthoveroncol);
    }

    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    g.setColour(getColour(Styles::bordercol));
    g.drawRoundedRectangle(b, rectCorner, 1);

    g.setFont(getFont(Styles::labelfont));
    g.setColour(fg);
    g.drawText(label, b.withTrimmedLeft(2), juce::Justification::centred);
}
} // namespace sst::jucegui::components