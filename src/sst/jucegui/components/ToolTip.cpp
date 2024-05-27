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

#include "sst/jucegui/components/ToolTip.h"

namespace sst::jucegui::components
{

ToolTip::ToolTip() : sst::jucegui::style::StyleConsumer(Styles::styleClass) { setSize(100, 100); }

static constexpr int margin{5}, rowPad{2}, rowTitlePad{2};

// TODO
// - Colors
// - Fixed Width Font for the data itself
// - Small rule under title

void ToolTip::paint(juce::Graphics &g)
{
    auto bg = style()->getColour(Styles::styleClass, Styles::background);
    auto bord = style()->getColour(Styles::styleClass, Styles::brightoutline);
    auto lbord = style()->getColour(Styles::styleClass, Styles::outline);
    auto txt = style()->getColour(Styles::styleClass, Styles::labelcolor);

    g.setColour(bg);
    g.fillRect(getLocalBounds());
    g.setColour(bord);
    g.drawRect(getLocalBounds(), 1);

    auto f = style()->getFont(Styles::styleClass, Styles::labelfont);
    auto rowHeight = f.getHeight() + rowPad;

    g.setColour(lbord);
    g.drawLine(3, rowHeight + margin - rowPad / 2, getWidth() - 3, rowHeight + margin - rowPad / 2,
               1);

    g.setColour(txt);
    auto bx = juce::Rectangle<int>(margin, margin, getWidth() - 2 * margin, rowHeight);
    g.setFont(f);
    g.drawText(tooltipTitle, bx, juce::Justification::topLeft);

    auto df = style()->getFont(Styles::styleClass, Styles::datafont);
    rowHeight = df.getHeight() + rowPad;
    g.setFont(df);

    bx = bx.translated(0, rowTitlePad);
    g.setColour(txt);
    for (const auto &d : tooltipData)
    {
        bx = bx.translated(0, rowHeight);
        g.drawText(d, bx, juce::Justification::centredLeft);
    }
}

void ToolTip::resetSizeFromData()
{
    auto f = style()->getFont(Styles::styleClass, Styles::labelfont);
    auto rowHeight = f.getHeight() + rowPad;
    auto maxw = std::max(f.getStringWidthFloat(tooltipTitle), 60.f);

    auto df = style()->getFont(Styles::styleClass, Styles::datafont);
    auto drowHeight = df.getHeight() + rowPad;

    for (const auto &d : tooltipData)
        maxw = std::max(maxw, df.getStringWidthFloat(d));

    // round to nearest 20 to avoid jitters
    maxw = std::ceil(maxw / 20.f) * 20;

    setSize(maxw + 2 * margin,
            2 * margin + rowHeight + drowHeight * tooltipData.size() + rowTitlePad);
}
} // namespace sst::jucegui::components