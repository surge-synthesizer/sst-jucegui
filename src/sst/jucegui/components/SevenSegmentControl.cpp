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
#include <cassert>

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
}

void SevenSegmentControl::paint(juce::Graphics &g)
{
    int val{0};
    if (data)
        val = data->getValue();

    auto bd = getLocalBounds().toFloat();
    auto bq = bd.withWidth(bd.getWidth() * 1.f / numDigits);
    auto pv = val;
    auto lcol = isHovered ? getColour(Styles::labelcolor_hover) : getColour(Styles::labelcolor);
    auto ocol = lcol.withAlpha(isHovered ? 0.15f : 0.1f);

    if (numDigits > 16)
    {
        assert(false);
        numDigits = 16;
    }
    int digits[16];
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
}
} // namespace sst::jucegui::components