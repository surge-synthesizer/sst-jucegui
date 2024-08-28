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
    auto txtColour = style()->getColour(Styles::styleClass, Styles::labelcolor);

    g.setColour(bg);
    g.fillRect(getLocalBounds());
    g.setColour(bord);
    g.drawRect(getLocalBounds(), 1);

    auto f = style()->getFont(Styles::styleClass, Styles::labelfont);
    auto rowHeight = f.getHeight() + rowPad;

    g.setColour(lbord);
    g.drawLine(3, rowHeight + margin - rowPad / 2, getWidth() - 3, rowHeight + margin - rowPad / 2,
               1);

    g.setColour(txtColour);
    auto bx = juce::Rectangle<int>(margin, margin, getWidth() - 2 * margin, rowHeight);
    g.setFont(f);
    g.drawText(tooltipTitle, bx, juce::Justification::topLeft);

    auto df = style()->getFont(Styles::styleClass, Styles::datafont);
    rowHeight = df.getHeight() + rowPad;
    g.setFont(df);

    bx = bx.translated(0, rowHeight + rowTitlePad);
    g.setColour(txtColour);
    for (auto i = 0; i < tooltipData.size(); ++i)
    {
        auto &row = tooltipData[i];
        auto rh = getRowHeight(i);
        bx = bx.withHeight(rh);

        auto yp = bx.getY();
        auto txtbx = bx;
        if (row.rowLeadingGlyph.has_value())
        {
            GlyphPainter::paintGlyph(g, bx.withWidth(glyphSize), *(row.rowLeadingGlyph), txtColour);
            txtbx = bx.withTrimmedLeft(glyphSize + 2);
        }
        g.setFont(row.leftIsMonospace ? df : f);
        g.drawText(row.leftAlignText, txtbx, juce::Justification::centredLeft);

        // if (row.drawLRArrow && !row.drawRLArrow)
        //     GlyphPainter::paintGlyph(g, txtbx.withTrimmedLeft(txtbx.getWidth() / 3),
        //                              GlyphPainter::GlyphType::ARROW_L_TO_R, txtColour);
        // else if (row.drawLRArrow && row.drawRLArrow)
        //     GlyphPainter::paintGlyph(g, txtbx.withTrimmedLeft(txtbx.getWidth() / 3),
        //                              GlyphPainter::GlyphType::ARROW_L_TO_R, txtColour); // should
        //                              be R_TO_L glyph
        g.setFont(row.centerIsMonospace ? df : f);
        g.drawText(row.centerAlignText, txtbx, juce::Justification::centred);
        // if (row.drawLRArrow && row.drawRLArrow)
        //{
        //     GlyphPainter::paintGlyph(g, txtbx.withTrimmedLeft(2 * txtbx.getWidth() / 3),
        //                              GlyphPainter::GlyphType::ARROW_L_TO_R, txtColour);
        // }
        g.setFont(row.rightIsMonospace ? df : f);
        g.drawText(row.rightAlignText, txtbx, juce::Justification::centredRight);
        bx = bx.translated(0, rh + rowPad);
    }
}

void ToolTip::resetSizeFromData()
{
    auto f = style()->getFont(Styles::styleClass, Styles::labelfont);
    auto rowHeight = f.getHeight() + rowPad;
    auto maxw = std::max(f.getStringWidthFloat(tooltipTitle), 60.f);

    auto df = style()->getFont(Styles::styleClass, Styles::datafont);
    auto drowHeight = 0.f;
    for (auto i = 0; i < tooltipData.size(); ++i)
    {
        drowHeight += getRowHeight(i) + rowPad;

        maxw = std::max(maxw, (float)getRowWidth(i)); // FIX
    }
    // round to nearest 20 to avoid jitters
    maxw = std::ceil(maxw / 20.f) * 20;

    setSize(maxw + 2 * margin, 2 * margin + rowHeight + drowHeight + rowTitlePad);
}

int ToolTip::getRowHeight(int row)
{
    auto f = style()->getFont(Styles::styleClass, Styles::labelfont);
    auto df = style()->getFont(Styles::styleClass, Styles::datafont);

    if (tooltipData[row].centerIsMonospace && tooltipData[row].leftIsMonospace &&
        tooltipData[row].rightIsMonospace)
        return df.getHeight();

    if (!tooltipData[row].centerIsMonospace && !tooltipData[row].leftIsMonospace &&
        !tooltipData[row].rightIsMonospace)
        return f.getHeight();

    return std::max(f.getHeight(), df.getHeight());
}

int ToolTip::getRowWidth(int ri)
{
    auto f = style()->getFont(Styles::styleClass, Styles::labelfont);
    auto df = style()->getFont(Styles::styleClass, Styles::datafont);

    auto &row = tooltipData[ri];
    // Special case -just the text
    if (!row.rowLeadingGlyph.has_value() && row.centerAlignText.empty() &&
        row.rightAlignText.empty())
    {
        if (row.leftIsMonospace)
            return df.getStringWidthFloat(row.leftAlignText);
        else
            return f.getStringWidth(row.leftAlignText);
    }

    auto blankStringWidth{4.f};
    auto res = 0.f;
    if (row.rowLeadingGlyph.has_value())
        res += glyphSize;
    res +=
        std::min(blankStringWidth, row.leftIsMonospace ? df.getStringWidthFloat(row.leftAlignText)
                                                       : f.getStringWidthFloat(row.leftAlignText));
    res += std::min(blankStringWidth, row.centerIsMonospace
                                          ? df.getStringWidthFloat(row.centerAlignText)
                                          : f.getStringWidthFloat(row.centerAlignText));
    res += std::min(blankStringWidth, row.rightIsMonospace
                                          ? df.getStringWidthFloat(row.rightAlignText)
                                          : f.getStringWidthFloat(row.rightAlignText));

    return res + 4;
}

} // namespace sst::jucegui::components