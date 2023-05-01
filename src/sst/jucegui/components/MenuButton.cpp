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

#include "sst/jucegui/components/MenuButton.h"

namespace sst::jucegui::components
{
MenuButton::MenuButton() : style::StyleConsumer(Styles::styleClass) {}

MenuButton::~MenuButton() {}

void MenuButton::paint(juce::Graphics &g)
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

    g.setFont(getFont(Styles::labelfont));
    g.setColour(fg);
    g.drawText(label, b.withTrimmedLeft(2), juce::Justification::centredLeft);

    auto q = b.withTrimmedRight(2);
    q = q.withLeft(q.getRight() - 10);
    auto cy = q.getCentreY();
    auto au = cy - 2;
    auto ad = cy + 2;

    auto cx = q.getCentreX();
    auto aL = cx - 3;
    auto aR = cx + 3;
    auto p = juce::Path();
    p.startNewSubPath(aL, au);
    p.lineTo(aR, au);
    p.lineTo(cx, ad);
    p.closeSubPath();

    g.fillPath(p);

    g.setColour(getColour(Styles::bordercol));
    g.drawRoundedRectangle(b, rectCorner, 1);
}
} // namespace sst::jucegui::components