//
// Created by Paul Walker on 9/16/23.
//

#ifndef CONDUIT_KNOBPAINTER_H
#define CONDUIT_KNOBPAINTER_H

#include <type_traits>
#include <algorithm>

#include <juce_gui_basics/juce_gui_basics.h>

namespace sst::jucegui::components
{

template <typename T, typename S> void knobPainterNoBody(juce::Graphics &g, T *that, S *source)
{
    auto b = that->getLocalBounds();
    auto knobarea = b.withHeight(b.getWidth());

    int strokeWidth{5};
    if (knobarea.getWidth() < 20)
        strokeWidth = 3;

    //  start and end here are 0...1
    auto arcFromTo = [knobarea, strokeWidth](float startV, float endV) {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float startA = dAng * (2 * startV - 1);
        float endA = dAng * (2 * endV - 1);

        auto region = knobarea.toFloat().reduced(strokeWidth * 0.5 + 1);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), startA, endA,
                 true);
        return p;
    };

    float dA = 1.f;
    if (!that->isEnabled())
    {
        g.setColour(that->getColour(T::Styles::gutter).withAlpha(0.5f));
        dA = 0.3f;
    }
    else if (that->isHovered)
        g.setColour(that->getColour(T::Styles::gutter_hover));
    else
        g.setColour(that->getColour(T::Styles::gutter));
    g.strokePath(arcFromTo(0, 1), juce::PathStrokeType(strokeWidth));

    auto v = source->getValue01();
    float startV{0.f}, endV{v};
    switch (that->pathDrawMode)
    {
    case T::FOLLOW_BIPOLAR:
        if (source->isBipolar())
        {
            startV = 0.5;
        }
        break;
    case T::ALWAYS_FROM_MIN:
        break;
    case T::ALWAYS_FROM_MAX:
    {
        startV = 1.f;
    }
    break;
    case T::ALWAYS_FROM_DEFAULT:
    {
        startV =
            (source->getDefaultValue() - source->getMin()) / (source->getMax() - source->getMin());
    }
    break;
    }
    if (startV > endV)
        std::swap(startV, endV);

    if (that->isHovered)
        g.setColour(that->getColour(T::Styles::value_hover).withAlpha(dA));
    else
        g.setColour(that->getColour(T::Styles::value).withAlpha(dA));
    g.strokePath(arcFromTo(startV, endV), juce::PathStrokeType(strokeWidth));

    constexpr bool supportsMod = std::is_base_of_v<data::ContinuousModulatable, S>;

    if constexpr (supportsMod)
    {
        if (that->isEditingMod)
        {
            auto mstart = source->getValue01();
            auto mend = std::clamp(mstart + source->getModulationValuePM1(), 0.f, 1.f);

            if (mstart > mend)
                std::swap(mstart, mend);
            g.setColour(that->getColour(T::Styles::modulation_value));
            g.strokePath(arcFromTo(mstart, mend), juce::PathStrokeType(strokeWidth * 0.7));

            if (source->isModulationBipolar())
            {
                auto mstart = source->getValue01();
                auto mend = std::clamp(mstart - source->getModulationValuePM1(), 0.f, 1.f);

                if (mstart > mend)
                    std::swap(mstart, mend);
                g.setColour(that->getColour(T::Styles::modulation_value));
                g.strokePath(arcFromTo(mstart, mend), juce::PathStrokeType(strokeWidth * 0.7));
            }
        }
    }

    if (that->isHovered)
    {
        auto v = source->getValue01();
        g.setColour(that->getColour(T::Styles::handle).withAlpha(dA));
        g.strokePath(arcFromTo(v - 0.005, v + 0.005), juce::PathStrokeType(strokeWidth));
    }
}
template <typename T, typename S> void knobPainter(juce::Graphics &g, T *that, S *source)
{

    if (!source)
    {
        g.fillAll(juce::Colours::red);
        return;
    }
    auto b = that->getLocalBounds();

    auto knobarea = b.withHeight(b.getWidth());

    auto kbcol = that->getColour(T::Styles::knobbase);
    knobPainterNoBody(g, that, source);

    int strokeWidth{5};
    bool smallKnob = false;
    if (knobarea.getWidth() < 20)
    {
        smallKnob = true;
        strokeWidth = 3;
    }

    auto circle = [knobarea](float r) -> juce::Path {
        auto region = knobarea.toFloat().reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentreX(), region.getY());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), 0,
                 2 * juce::MathConstants<float>::pi);
        p.closeSubPath();
        return p;
    };

    auto dA = 1.f;
    if (!that->isEnabled())
    {
        dA = 0.166f; // since we paint 3 layers this gets us =~ 0.5
    }

    if (kbcol.getAlpha() != 0)
    {
        auto handleAngle = [that, knobarea](float v) -> float {
            float dPath = 0.2;
            float dAng = juce::MathConstants<float>::pi * (1 - dPath);
            float pt = dAng * (2 * v - 1);
            return pt;
        };

        // Fill over the mess in the middle
        auto c = that->getColour(T::Styles::knobbase).withAlpha(dA);

        auto makeGrad = [c, knobarea](auto up, auto dn) {
            return juce::ColourGradient::vertical(c.brighter(up), knobarea.getY(), c.darker(dn),
                                                  knobarea.getY() + knobarea.getHeight());
        };
        g.saveState();
        g.addTransform(juce::AffineTransform()
                           .translated(-knobarea.getWidth() / 2, -knobarea.getHeight() / 2)
                           .rotated(-0.3)
                           .translated(knobarea.getWidth() / 2, knobarea.getHeight() / 2));

        auto pIn = circle(strokeWidth);
        g.setGradientFill(makeGrad(0.0, 0.5));
        g.fillPath(pIn);
        g.setColour(c.darker(0.6));
        g.strokePath(pIn, juce::PathStrokeType(smallKnob ? 0.5 : 1.0));

        pIn = circle(strokeWidth + 1);
        g.setGradientFill(makeGrad(0.6, 0.35));
        g.fillPath(pIn);

        pIn = circle(strokeWidth + 2);
        g.setGradientFill(makeGrad(0.1, 0.05));
        g.fillPath(pIn);

        g.restoreState();

        g.saveState();
        g.addTransform(juce::AffineTransform()
                           .translated(-knobarea.getWidth() / 2, -knobarea.getHeight() / 2)
                           .rotated(handleAngle(source->getValue01()))
                           .translated(knobarea.getWidth() / 2, knobarea.getHeight() / 2));

        auto hanWidth = 2.f;
        auto hanLen = smallKnob ? 3.f : 8.f;
        auto hanRect = juce::Rectangle<float>(knobarea.getWidth() / 2.f - hanWidth / 2.f,
                                              strokeWidth, hanWidth, hanLen);

        if (that->isHovered)
        {
            g.setColour(that->getColour(T::Styles::handle).withAlpha(dA));
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
    }

    if (that->modulationDisplay == ContinuousParamEditor::FROM_ACTIVE)
    {
        auto pOut = circle(knobarea.getWidth() / 2 - (smallKnob ? 3 : 5));
        g.setColour(that->getColour(T::Styles::modulated_by_selected));
        g.fillPath(pOut);
    }
    if (that->modulationDisplay == ContinuousParamEditor::FROM_OTHER)
    {
        auto pOut = circle(knobarea.getWidth() / 2 - (smallKnob ? 3 : 8));
        g.setColour(that->getColour(T::Styles::modulated_by_other));
        g.fillPath(pOut);
    }
}

} // namespace sst::jucegui::components
#endif // CONDUIT_KNOBPAINTER_H
