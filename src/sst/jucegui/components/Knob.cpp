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

#include <sst/jucegui/components/Knob.h>

namespace sst::jucegui::components
{
Knob::Knob() : style::StyleConsumer(Styles::styleClass), ContinuousParamEditor(Direction::VERTICAL)
{
}
Knob::~Knob() = default;

void Knob::paint(juce::Graphics &g)
{
    if (!source)
    {
        g.fillAll(juce::Colours::red);
        return;
    }
    auto b = getLocalBounds();

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

    auto pathWithReduction = [this, knobarea](int r, float v) -> juce::Path {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float start = dAng * (2 * v - 1); // 1 -> dAng; 0 -> -dAng so dAng * 2 * v - dAng
        float end = -dAng;
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
        auto region = knobarea.reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(), start, end);
        p.closeSubPath();
        return p;
    };

    auto handlePath = [this, knobarea](int r, float v) -> juce::Path {
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

    auto handleCenter = [this, knobarea](int r, float v) -> juce::Point<float> {
        float dPath = 0.2;
        float dAng = juce::MathConstants<float>::pi * (1 - dPath);
        float pt = dAng * (2 * v - 1);
        auto start = pt;

        auto region = knobarea.reduced(r);

        auto cp = region.getCentre().toFloat();
        auto rad = std::min(region.getWidth()/2, region.getHeight()/2);

        return {cp.getX() + rad * sin(pt), cp.getY() - rad * cos(pt)};
    };

    auto modPath = [this, knobarea](int r, float v, float m, int direction) -> juce::Path {
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
    g.setColour(getColour(Styles::backgroundcol));
    g.fillPath(pOut);

    // inner gutter
    auto pIn = pacman(1);
    g.setColour(getColour(Styles::guttercol));
    g.fillPath(pIn);

    // value gutter
    pIn = pathWithReduction(2, source->getValue01());
    g.setColour(getColour(Styles::valcol));
    g.fillPath(pIn);

    // modulation arcs
    if (isEditingMod)
    {
        pIn = modPath(5, source->getValue01(), source->getModulationValuePM1(), 1);
        g.setColour(getColour(Styles::modvalcol));
        g.fillPath(pIn);
        if (source->isModulationBipolar())
        {
            pIn = modPath(5, source->getValue01(), source->getModulationValuePM1(), -1);
            g.setColour(getColour(Styles::modvalnegcol));
            g.fillPath(pIn);
        }
    }

    // Fill over the mess in the middle
    pIn = circle(knobarea.getWidth() / 2 - 8);
    g.setColour(juce::Colours::black);
    g.fillPath(pIn);

    auto c = getColour(Styles::knobbase);

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

    auto hc = handleCenter(11, source->getValue01());

    auto er = 2;
    if (isHovered)
    {
        g.setColour(getColour(Styles::handlecol));
        g.fillEllipse(hc.getX() - er, hc.getY() - er, er * 2 + 1, er * 2 + 1);
    }
    else
    {
        g.setColour(getColour(Styles::handlecol).withAlpha(0.4f));
        g.fillEllipse(hc.getX() - er, hc.getY() - er, er * 2 + 1, er * 2 + 1);
        g.setColour(getColour(Styles::handlecol).withAlpha(0.7f));
        g.drawEllipse(hc.getX() - er, hc.getY() - er, er * 2 + 1, er * 2 + 1, 1);
    }
    if (modulationDisplay == FROM_ACTIVE)
    {
        pOut = circle(8);
        g.setColour(getColour(Styles::modactivecol));
        g.fillPath(pOut);
    }
    if (modulationDisplay == FROM_OTHER)
    {
        pOut = circle(8);
        g.setColour(getColour(Styles::modothercol));
        g.fillPath(pOut);
    }

    auto textarea = b.withTrimmedTop(b.getWidth());
    g.setColour(getColour(Styles::labeltextcol));
    g.setFont(getFont(Styles::labeltextfont));
    g.drawText(source->getLabel(), textarea, juce::Justification::centred);
}

} // namespace sst::jucegui::components