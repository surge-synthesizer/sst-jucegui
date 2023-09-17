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

#include <sst/jucegui/components/SevenSegmentControl.h>
#include <sst/jucegui/components/SevenSegmentPainter.h>
#include <sst/jucegui/components/GlyphPainter.h>

namespace sst::jucegui::components
{
SevenSegmentControl::SevenSegmentControl(int n)
    : numDigits(n), style::StyleConsumer(Styles::styleClass)
{
}

SevenSegmentControl::~SevenSegmentControl() {}

void SevenSegmentControl::mouseDown(const juce::MouseEvent &e) { isDragGesture = false; }

void SevenSegmentControl::mouseDrag(const juce::MouseEvent &e)
{
    if (e.getDistanceFromDragStart() > 2)
    {
        isDragGesture = true;
        onBeginEdit();
    }

    if (isDragGesture && data)
    {
        auto d = e.getDistanceFromDragStartY() - lastJogDragDistance;
        if (abs(d) > 10)
        {
            if (d < 0)
            {
                data->jog(+1);
            }
            else
            {
                data->jog(-1);
            }
            lastJogDragDistance = e.getDistanceFromDragStartY();
            repaint();
        }
    }
}

void SevenSegmentControl::mouseUp(const juce::MouseEvent &e)
{
    if (!data)
        return;

    if (isDragGesture)
    {
        onEndEdit();
        isDragGesture = false;
        return;
    }

    if (hasJogButtons)
    {
        // TODO don't copy this
        auto bd = getLocalBounds().toFloat().withTrimmedRight(jogButtonWidth + 2);
        auto jb =
            getLocalBounds().toFloat().withWidth(jogButtonWidth).translated(bd.getWidth() + 1, 0);
        auto bq = bd.withWidth(bd.getWidth() * 1.f / numDigits);
        bq = bq.translated(bq.getWidth() * (numDigits - 1), 0);
        jb = jb.withHeight(bq.getWidth() / SevenSegmentPainter::aspectRatio);

        auto r1 = jb.withHeight(jb.getHeight() / 2);
        if (r1.contains(e.x, e.y))
        {
            onBeginEdit();
            data->jog(+1);
            onEndEdit();
            repaint();
        }
        r1 = r1.translated(0, r1.getHeight());
        if (r1.contains(e.x, e.y))
        {
            onBeginEdit();
            data->jog(-1);
            onEndEdit();
            repaint();
        }
    }
}

void SevenSegmentControl::paint(juce::Graphics &g)
{
    int val{0};
    if (data)
        val = data->getValue();

    auto jbw = hasJogButtons ? jogButtonWidth : 0;

    auto bd = getLocalBounds().toFloat().withTrimmedRight(jbw + 2 * hasJogButtons);
    auto jb = getLocalBounds().toFloat().withWidth(jbw).translated(bd.getWidth() + 1, 0);
    auto bq = bd.withWidth(bd.getWidth() * 1.f / numDigits);
    jb = jb.withHeight(bq.getWidth() / SevenSegmentPainter::aspectRatio);
    auto pv = val;
    auto lcol = getColour(Styles::controlLabelCol);
    auto ocol = lcol.withAlpha(isHovered ? 0.2f : 0.1f);
    int digits[numDigits];
    for (int i = 0; i < numDigits; ++i)
    {
        auto vl = pv % 10;
        digits[numDigits - 1 - i] = vl;
        pv = (int)pv / 10;
    }

    bool leadingZero = true;
    for (int i = 0; i < numDigits; ++i)
    {
        leadingZero = leadingZero && (digits[i] == 0);
        SevenSegmentPainter::paintInto(g, bq.reduced(0.5), (leadingZero ? ocol : lcol), ocol,
                                       digits[i]);
        bq = bq.translated(bq.getWidth(), 0);
    }

    if (hasJogButtons)
    {
        g.setColour(getColour(Styles::regionBorder));
        g.drawRoundedRectangle(jb, 2, 1);
        g.drawLine(jb.getX(), jb.getCentreY(), jb.getX() + jb.getWidth(), jb.getCentreY());

        auto r1 = jb.withHeight(jb.getHeight() / 2);

        if (isHovered && r1.contains(hoverX, hoverY))
        {
            g.setColour(getColour(Styles::regionBorder));
            g.fillRoundedRectangle(r1, 2);
        }
        g.setColour(getColour(Styles::controlLabelCol));
        GlyphPainter::paintGlyph(g, r1.toNearestInt(), GlyphPainter::JOG_UP);

        r1 = r1.translated(0, r1.getHeight());
        if (isHovered && r1.contains(hoverX, hoverY))
        {
            g.setColour(getColour(Styles::regionBorder));
            g.fillRoundedRectangle(r1, 2);
        }
        g.setColour(getColour(Styles::controlLabelCol));
        GlyphPainter::paintGlyph(g, r1.toNearestInt(), GlyphPainter::JOG_DOWN);
    }
}
} // namespace sst::jucegui::components