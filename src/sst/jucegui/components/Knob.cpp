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

    // fix me - paint modulation
    auto pOut = pacman(1);
    g.setColour(getColour(Styles::backgroundcol));
    g.fillPath(pOut);

    // modulation not set up yet
    auto pIn = pathWithReduction(2, 0.2);
    /*
    g.setColour(getColour(Styles::modringcol));
    g.fillPath(pIn);
    g.setColour(getColour(Styles::backgroundcol));
    g.fillPath(pacman(4));
     */

    pIn = pathWithReduction(3, source->getValue01());
    g.setColour(getColour(Styles::ringcol));
    g.fillPath(pIn);
    {
        auto pGrad = pathWithReduction(8, source->getValue01());
        auto cg = juce::ColourGradient(getColour(Styles::gradientcenter), knobarea.getCentreX(),
                                       knobarea.getCentreY(), getColour(Styles::backgroundcol),
                                       knobarea.getCentreX(), knobarea.getY() - 3, true);
        g.setGradientFill(cg);
        g.fillPath(pGrad);
    }

    g.setColour(getColour(Styles::backgroundcol));
    g.strokePath(pOut, juce::PathStrokeType(1));

    auto textarea = b.withTrimmedTop(b.getWidth());
    g.setColour(getColour(Styles::labelcol));
    g.drawText(source->getLabel(), textarea, juce::Justification::centred);
}

void Knob::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isPopupMenu())
    {
        mouseMode = POPUP;
        onPopupMenu(e.mods);
        return;
    }

    jassert(source);
    mouseMode = DRAG;
    onBeginEdit();
    mouseDownV0 = source->getValue();
    mouseDownY0 = e.position.y;
}
void Knob::mouseUp(const juce::MouseEvent &e)
{
    if (mouseMode == DRAG)
        onEndEdit();
    mouseMode = NONE;
}
void Knob::mouseDrag(const juce::MouseEvent &e)
{
    if (mouseMode != DRAG)
        return;

    float d = -(e.position.y - mouseDownY0) / 150.0 * (source->getMax() - source->getMin());
    if (e.mods.isShiftDown())
        d = d * 0.1;
    auto vn = std::clamp(mouseDownV0 + d, source->getMin(), source->getMax());
    source->setValue(vn);
    repaint();
}
void Knob::mouseWheelMove(const juce::MouseEvent &e, const juce::MouseWheelDetails &wheel)
{
    if (fabs(wheel.deltaY) < 0.0001)
        return;
    onBeginEdit();

    // fixme - callibration and sharing
    auto d = (wheel.isReversed ? -1 : 1) * wheel.deltaY * (source->getMax() - source->getMin());
    if (e.mods.isShiftDown())
        d = d * 0.1;
    auto vn = std::clamp(source->getValue() + d, source->getMin(), source->getMax());
    source->setValue(vn);

    onEndEdit();
    repaint();
}
} // namespace sst::jucegui::components