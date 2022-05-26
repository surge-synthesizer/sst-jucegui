//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/components/Knob.h>

namespace sst::jucegui::components
{
Knob::Knob() : style::StyleConsumer(Styles::styleClass) {}
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

    auto pacman = [knobarea](int r) -> juce::Path {
        float dPath = 0.2;
        auto region = knobarea.reduced(r);
        auto p = juce::Path();
        p.startNewSubPath(region.getCentre().toFloat());
        p.addArc(region.getX(), region.getY(), region.getWidth(), region.getHeight(),
                 juce::MathConstants<float>::pi * (1 - dPath),
                 -juce::MathConstants<float>::pi * (1 - dPath));
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
            start = 0;
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

    // fix me - paint modulation
    auto pOut = pacman(1);
    g.setColour(getColour(Styles::backgroundcol));
    g.fillPath(pOut);

    if (modulationDisplay == FROM_ACTIVE)
    {
        pOut = pacman(0);
        g.setColour(getColour(Styles::modactivecol));
        g.fillPath(pOut);
    }
    if (modulationDisplay == FROM_OTHER)
    {
        pOut = pacman(0);
        g.setColour(getColour(Styles::modothercol));
        g.fillPath(pOut);
    }

    auto pIn = pacman(3);
    g.setColour(getColour(Styles::guttercol));
    g.fillPath(pIn);

    pIn = pacman(8);
    g.setColour(getColour(Styles::backgroundcol));
    g.fillPath(pIn);

    pIn = pathWithReduction(3, source->getValue01());
    g.setColour(getColour(Styles::valcol));
    g.fillPath(pIn);

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

    pIn = handlePath(3, source->getValue01());
    if (isHovered)
    {
        g.setColour(getColour(Styles::handlecol));
        g.fillPath(pIn);
    }

    pIn = pacman(8);
    g.setColour(getColour(Styles::backgroundcol));
    g.fillPath(pIn);

    if (isHovered)
    {
        auto pGrad = pathWithReduction(8, source->getValue01());
        auto cg = juce::ColourGradient(getColour(Styles::gradientcenter), knobarea.getCentreX(),
                                       knobarea.getCentreY(), getColour(Styles::backgroundcol),
                                       knobarea.getCentreX(), knobarea.getY() - 3, true);
        g.setGradientFill(cg);
        g.fillPath(pGrad);
    }
    else
    {
    }

    g.setColour(getColour(Styles::backgroundcol));
    g.strokePath(pOut, juce::PathStrokeType(1));

    auto textarea = b.withTrimmedTop(b.getWidth());
    g.setColour(getColour(Styles::labelcol));
    g.drawText(source->getLabel(), textarea, juce::Justification::centred);
}

} // namespace sst::jucegui::components