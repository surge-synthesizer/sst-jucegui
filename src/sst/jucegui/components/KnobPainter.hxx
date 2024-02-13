//
// Created by Paul Walker on 9/16/23.
//

#ifndef CONDUIT_KNOBPAINTER_H
#define CONDUIT_KNOBPAINTER_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <type_traits>

namespace sst::jucegui::components
{
template <typename T, typename S> void knobPainter(juce::Graphics &g, T *that, S *source)
{
    constexpr bool supportsMod = std::is_base_of_v<data::ContinuousModulatable, S>;

    if (!source)
    {
        g.fillAll(juce::Colours::red);
        return;
    }
    auto b = that->getLocalBounds();

    auto knobarea = b.withHeight(b.getWidth());

    auto pacman = [knobarea](float r, float ddPath = 0) -> juce::Path {
        float dPath = 0.2 + ddPath;
        auto region = knobarea.toFloat().reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(),
                 juce::MathConstants<float>::pi * (1 - dPath),
                 -juce::MathConstants<float>::pi * (1 - dPath));
        p.closeSubPath();
        return p;
    };

    auto circle = [knobarea](float r) -> juce::Path {
        auto region = knobarea.toFloat().reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentreX(), region.getY());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), 0,
                 2 * juce::MathConstants<float>::pi);
        p.closeSubPath();
        return p;
    };

    auto pathWithReduction = [that, source, knobarea](float r, float v) -> juce::Path {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float start = dAng * (2 * v - 1); // 1 -> dAng; 0 -> -dAng so dAng * 2 * v - dAng
        float end = -dAng;
        switch (that->pathDrawMode)
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
            float zero01 = (source->getDefaultValue() - source->getMin()) /
                           (source->getMax() - source->getMin());
            // 1 -> dAng; 0 -> -dAng again so
            start = dAng * (2 * zero01 - 1);
            end = dAng * v;
        }
        break;
        }
        auto region = knobarea.toFloat().reduced(r);
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

    auto handleAngle = [that, knobarea](float v) -> float {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float pt = dAng * (2 * v - 1);
        return pt;
    };

    auto modPath = [that, knobarea](float r, float v, float m, int direction) -> juce::Path {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float pt = dAng * (2 * v - 1);
        auto start = pt;
        auto end = pt + direction * dAng * 2 * m;
        start = std::clamp(start, -dAng, dAng);
        end = std::clamp(end, -dAng, dAng);
        auto region = knobarea.toFloat().reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), start, end);
        p.closeSubPath();
        return p;
    };

    static constexpr float gutterPad{1};
    // outer gutter edge
    auto pOut = pacman(0, -0.005);
    g.setColour(that->getColour(T::Styles::background));
    g.fillPath(pOut);

    // inner gutter
    auto pIn = pacman(gutterPad);
    g.setColour(that->getColour(T::Styles::gutter));
    g.fillPath(pIn);

    // value gutter
    pIn = pathWithReduction(gutterPad * 1.5, source->getValue01());
    if (that->isHovered)
        g.setColour(that->getColour(T::Styles::value_hover));
    else
        g.setColour(that->getColour(T::Styles::value));
    g.fillPath(pIn);

    // modulation arcs
    if constexpr (supportsMod)
    {
        if (that->isEditingMod)
        {
            pIn = modPath(gutterPad * 2, source->getValue01(), source->getModulationValuePM1(), 1);
            g.setColour(that->getColour(T::Styles::modulation_value));
            g.fillPath(pIn);
            if (source->isModulationBipolar())
            {
                pIn = modPath(gutterPad * 2, source->getValue01(), source->getModulationValuePM1(),
                              -1);
                g.setColour(that->getColour(T::Styles::modulation_opposite_value));
                g.fillPath(pIn);
            }
        }
    }

    auto knobSize = gutterPad * 5.f;
    bool smallKnob = false;
    if (knobSize > knobarea.getWidth() * .15)
    {
        knobSize = gutterPad * 3.5;
        smallKnob = true;
    }
    // Fill over the mess in the middle
    auto c = that->getColour(T::Styles::knobbase);

    auto makeGrad = [c, knobarea](auto up, auto dn) {
        return juce::ColourGradient::vertical(c.brighter(up), knobarea.getY(), c.darker(dn),
                                              knobarea.getY() + knobarea.getHeight());
    };
    g.saveState();
    g.addTransform(juce::AffineTransform()
                       .translated(-knobarea.getWidth() / 2, -knobarea.getHeight() / 2)
                       .rotated(-0.3)
                       .translated(knobarea.getWidth() / 2, knobarea.getHeight() / 2));

    pIn = circle(knobSize);
    g.setGradientFill(makeGrad(0.0, 0.5));
    g.fillPath(pIn);
    g.setColour(c.darker(0.6));
    g.strokePath(pIn, juce::PathStrokeType(smallKnob ? 0.5 : 1.0));

    pIn = circle(knobSize + gutterPad);
    g.setGradientFill(makeGrad(0.6, 0.35));
    g.fillPath(pIn);

    pIn = circle(knobSize + 3 * gutterPad);
    g.setGradientFill(makeGrad(0.1, 0.05));
    g.fillPath(pIn);
    g.restoreState();

    g.saveState();
    g.addTransform(juce::AffineTransform()
                       .translated(-knobarea.getWidth() / 2, -knobarea.getHeight() / 2)
                       .rotated(handleAngle(source->getValue01()))
                       .translated(knobarea.getWidth() / 2, knobarea.getHeight() / 2));

    auto hanWidth = 2.f;
    auto hanRect =
        juce::Rectangle<float>(knobarea.getWidth() / 2.f - hanWidth / 2.f, knobSize + gutterPad,
                               hanWidth, smallKnob ? gutterPad * 5 : gutterPad * 10);

    if (that->isHovered)
    {
        g.setColour(that->getColour(T::Styles::handle));
        g.fillRect(hanRect);
    }
    else
    {
        g.setColour(that->getColour(T::Styles::handle).withAlpha(0.2f));
        g.fillRect(hanRect);
        g.setColour(that->getColour(T::Styles::handle).withAlpha(0.4f));
        g.drawRect(hanRect, 0.5);
    }

    g.restoreState();

    if (that->modulationDisplay == ContinuousParamEditor::FROM_ACTIVE)
    {
        pOut = circle(knobarea.getWidth() / 2 - (smallKnob ? 3 : 5));
        g.setColour(that->getColour(T::Styles::modulated_by_selected));
        g.fillPath(pOut);
    }
    if (that->modulationDisplay == ContinuousParamEditor::FROM_OTHER)
    {
        pOut = circle(knobarea.getWidth() / 2 - (smallKnob ? 3 : 8));
        g.setColour(that->getColour(T::Styles::modulated_by_other));
        g.fillPath(pOut);
    }
}
} // namespace sst::jucegui::components
#endif // CONDUIT_KNOBPAINTER_H
