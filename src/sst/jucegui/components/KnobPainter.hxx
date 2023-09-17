//
// Created by Paul Walker on 9/16/23.
//

#ifndef CONDUIT_KNOBPAINTER_H
#define CONDUIT_KNOBPAINTER_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace sst::jucegui::components
{
template<typename T, typename S>
void knobPainter(juce::Graphics &g, T* that, S *source)
{
    if (!source)
    {
        g.fillAll(juce::Colours::red);
        return;
    }
    auto b = that->getLocalBounds();

    auto knobarea = b.withHeight(b.getWidth());

    auto pacman = [knobarea](int r, float ddPath = 0) -> juce::Path {
        float dPath = 0.2 + ddPath;
        auto region = knobarea.reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(),
                 juce::MathConstants<float>::pi * (1 - dPath),
                 -juce::MathConstants<float>::pi * (1 - dPath));
        p.closeSubPath();
        return p;
    };

    auto circle = [knobarea](int r) -> juce::Path {
        auto region = knobarea.reduced(knobarea.getWidth() / 2 - r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentreX(), region.getY());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), 0,
                 2 * juce::MathConstants<float>::pi);
        p.closeSubPath();
        return p;
    };

    auto pathWithReduction = [that, source, knobarea](int r, float v) -> juce::Path {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float start = dAng * (2 * v - 1); // 1 -> dAng; 0 -> -dAng so dAng * 2 * v - dAng
        float end = -dAng;
        switch(that->pathDrawMode)
        {
        case T::FOLLOW_BIPOLAR:
            if (source->isBipolar())
            {
                auto v0 = v;
                v = 2 * v - 1;
                // split between dAng and -dAnd
                float zero01 = (0 - source->getMin()) / (source->getMax() - source->getMin());
                // 1 -> dAng; 0 -> -dAng again so
                start = dAng * (2 * zero01 - 1);
                end = dAng * v;
            }
            break;
        case T::ALWAYS_FROM_MIN:
            break;
        case T::ALWAYS_FROM_MAX:
        {
            auto t = start;
            start = dAng;
            end = t;
        }
            break;
        case T::ALWAYS_FROM_DEFAULT:
            {
                auto v0 = v;
                v = 2 * v - 1;
                // split between dAng and -dAnd
                float zero01 = (source->getDefaultValue() - source->getMin()) / (source->getMax() - source->getMin());
                // 1 -> dAng; 0 -> -dAng again so
                start = dAng * (2 * zero01 - 1);
                end = dAng * v;
            }
            break;

        }
        auto region = knobarea.reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), start, end);
        p.closeSubPath();
        return p;
    };

    auto handlePath = [that, knobarea](int r, float v) -> juce::Path {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float pt = dAng * (2 * v - 1);
        auto start = pt - juce::MathConstants<float>::pi * 0.02;
        auto end = pt + juce::MathConstants<float>::pi * 0.02;
        auto region = knobarea.reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), start, end);
        p.closeSubPath();
        return p;
    };

    auto handleCenter = [that, knobarea](int r, float v) -> juce::Point<float> {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float pt = dAng * (2 * v - 1);
        auto start = pt;

        auto region = knobarea.reduced(r);

        auto cp = region.getCentre().toFloat();
        auto rad = std::min(region.getWidth()/2, region.getHeight()/2);

        return {cp.getX() + rad * sin(pt), cp.getY() - rad * cos(pt)};
    };

    auto modPath = [that, knobarea](int r, float v, float m, int direction) -> juce::Path {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float pt = dAng * (2 * v - 1);
        auto start = pt;
        auto end = pt + direction * dAng * 2 * m;
        start = std::clamp(start, -dAng, dAng);
        end = std::clamp(end, -dAng, dAng);
        auto region = knobarea.reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), start, end);
        p.closeSubPath();
        return p;
    };

    // outer gutter edge
    auto pOut = pacman(0, -0.005);
    g.setColour(that->getColour(T::Styles::backgroundcol));
    g.fillPath(pOut);

    // inner gutter
    auto pIn = pacman(1);
    g.setColour(that->getColour(T::Styles::guttercol));
    g.fillPath(pIn);

    // value gutter
    pIn = pathWithReduction(2, source->getValue01());
    g.setColour(that->getColour(T::Styles::valcol));
    g.fillPath(pIn);

    // modulation arcs
    if (that->isEditingMod)
    {
        pIn = modPath(5, source->getValue01(), source->getModulationValuePM1(), 1);
        g.setColour(that->getColour(T::Styles::modvalcol));
        g.fillPath(pIn);
        if (source->isModulationBipolar())
        {
            pIn = modPath(5, source->getValue01(), source->getModulationValuePM1(), -1);
            g.setColour(that->getColour(T::Styles::modvalnegcol));
            g.fillPath(pIn);
        }
    }

    // Fill over the mess in the middle
    pIn = circle(knobarea.getWidth() / 2 - 8);
    g.setColour(juce::Colours::black);
    g.fillPath(pIn);

    auto c = that->getColour(T::Styles::knobbase);

    auto makeGrad = [c, knobarea](auto up, auto dn) {
        return juce::ColourGradient::vertical(c.brighter(up), knobarea.getY(), c.darker(dn),
                                              knobarea.getY() + knobarea.getHeight());
    };
    pIn = circle(knobarea.getWidth() / 2 - 9);
    g.setGradientFill(makeGrad(0.0, 0.5));
    g.fillPath(pIn);

    pIn = circle(knobarea.getWidth() / 2 - 10);
    g.setGradientFill(makeGrad(0.8, 0.35));
    g.fillPath(pIn);

    pIn = circle(knobarea.getWidth() / 2 - 11);
    g.setGradientFill(makeGrad(0.1, 0.05));
    g.fillPath(pIn);

    auto hc = handleCenter(12, source->getValue01());

    auto er = 2;
    if (that->isHovered)
    {
        g.setColour(that->getColour(T::Styles::handlecol));
        g.fillEllipse(hc.getX() - er, hc.getY() - er, er * 2 + 1, er * 2 + 1);
    }
    else
    {
        g.setColour(that->getColour(T::Styles::handlecol).withAlpha(0.4f));
        g.fillEllipse(hc.getX() - er, hc.getY() - er, er * 2 + 1, er * 2 + 1);
        g.setColour(that->getColour(T::Styles::handlecol).withAlpha(0.7f));
        g.drawEllipse(hc.getX() - er, hc.getY() - er, er * 2 + 1, er * 2 + 1, 1);
    }
    if (that->modulationDisplay == ContinuousParamEditor::FROM_ACTIVE)
    {
        pOut = circle(8);
        g.setColour(that->getColour(T::Styles::modactivecol));
        g.fillPath(pOut);
    }
    if (that->modulationDisplay == ContinuousParamEditor::FROM_OTHER)
    {
        pOut = circle(8);
        g.setColour(that->getColour(T::Styles::modothercol));
        g.fillPath(pOut);
    }
}
}
#endif // CONDUIT_KNOBPAINTER_H
