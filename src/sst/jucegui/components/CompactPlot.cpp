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

#include "sst/jucegui/components/CompactPlot.h"

namespace sst::jucegui::components
{
void CompactPlot::paint(juce::Graphics &g)
{
    if (!source)
    {
        g.fillAll(juce::Colours::red);
        return;
    }
    auto cl = 1.0f;
    if (!source->isYAxisFromZero())
    {
        auto yb = source->getYAxisBounds();
        cl = yb.second / (-yb.first + yb.second);
    }

    if (curvePath.isEmpty() || plotData.empty() || !source->curvePathIsValid())
    {
        plotData.clear();
        source->recalculateCurvePath(plotData);

        curvePath.clear();
        positiveCurvePath.clear();
        negativeCurvePath.clear();

        positiveCurvePath.startNewSubPath(0, cl);
        negativeCurvePath.startNewSubPath(0, cl);

        bool first{true};
        for (const auto &[x, y] : plotData)
        {
            if (first)
            {
                curvePath.startNewSubPath(x, y);
                first = false;
            }
            else
            {
                curvePath.lineTo(x, y);
            }
            positiveCurvePath.lineTo(x, std::min(y, cl));
            negativeCurvePath.lineTo(x, std::max(y, cl));
        }
        positiveCurvePath.lineTo(1, cl);
        positiveCurvePath.closeSubPath();
        negativeCurvePath.lineTo(1, cl);
        negativeCurvePath.closeSubPath();
    }

    auto axisColor = getColour(Styles::plotAxis);
    auto gradStart = getColour(Styles::plotGradStart);
    auto gradEnd = getColour(Styles::plotGradEnd);
    auto plotStroke = getColour(Styles::plotLine);

    auto lg = juce::ColourGradient::vertical(gradStart, 0, gradEnd, cl * getHeight());
    g.setGradientFill(lg);
    g.fillPath(positiveCurvePath, juce::AffineTransform().scaled(getWidth(), getHeight()));

    auto lg2 = juce::ColourGradient::vertical(gradEnd, cl * getHeight(), gradStart, getHeight());
    g.setGradientFill(lg2);
    g.fillPath(negativeCurvePath, juce::AffineTransform().scaled(getWidth(), getHeight()));

    g.setColour(axisColor);
    g.drawLine(0, 0, 0, getHeight(), 1);
    g.drawLine(0, cl * getHeight(), getWidth(), cl * getHeight(), 1);

    g.setColour(plotStroke);
    g.strokePath(curvePath, juce::PathStrokeType(1.0),
                 juce::AffineTransform().scaled(getWidth(), getHeight()));
}

} // namespace sst::jucegui::components