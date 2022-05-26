//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/components/Knob.h>

namespace sst::jucegui::components
{
Knob::Knob() : style::StyleAndSettingsConsumer(Styles::styleClass) {}
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
        g.setColour(getColour(Styles::modringactivecol));
        g.fillPath(pOut);
    }
    if (modulationDisplay == FROM_OTHER)
    {
        pOut = pacman(0);
        g.setColour(getColour(Styles::modringothercol));
        g.fillPath(pOut);
    }

    auto pIn = pacman(3);
    g.setColour(getColour(Styles::guttercol));
    g.fillPath(pIn);

    pIn = pacman(8);
    g.setColour(getColour(Styles::backgroundcol));
    g.fillPath(pIn);

    pIn = pathWithReduction(3, source->getValue01());
    g.setColour(getColour(Styles::ringcol));
    g.fillPath(pIn);

    if (isEditingMod)
    {
        pIn = modPath(5, source->getValue01(), source->getModulationValuePM1(), 1);
        g.setColour(getColour(Styles::modringcol));
        g.fillPath(pIn);
        if (source->isModulationBipolar())
        {
            pIn = modPath(5, source->getValue01(), source->getModulationValuePM1(), -1);
            g.setColour(getColour(Styles::modringnegcol));
            g.fillPath(pIn);
        }
    }

    pIn = handlePath(3, source->getValue01());
    if (isHovered)
    {
        g.setColour(getColour(Styles::valcol));
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

void Knob::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isPopupMenu())
    {
        mouseMode = POPUP;
        onPopupMenu(e.mods);
        return;
    }

    jassert(settings());
    jassert(source);
    mouseMode = DRAG;
    onBeginEdit();
    if (isEditingMod)
        mouseDownV0 = source->getModulationValuePM1();
    else
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
    if (isEditingMod)
    {
        auto vn = std::clamp(mouseDownV0 + d, -1.f, 1.f);
        source->setModulationValuePM1(vn);
    }
    else
    {
        auto vn = std::clamp(mouseDownV0 + d, source->getMin(), source->getMax());
        source->setValue(vn);
    }
    repaint();
}
void Knob::mouseWheelMove(const juce::MouseEvent &e, const juce::MouseWheelDetails &wheel)
{
    if (fabs(wheel.deltaY) < 0.0001)
        return;
    onBeginEdit();

    if (isEditingMod)
    {
        // fixme - callibration and sharing
        auto d = (wheel.isReversed ? -1 : 1) * wheel.deltaY * (2);
        if (e.mods.isShiftDown())
            d = d * 0.1;

        auto vn = std::clamp(source->getModulationValuePM1() + d, -1.f, 1.f);
        source->setModulationValuePM1(vn);
    }
    else
    {
        // fixme - callibration and sharing
        auto d = (wheel.isReversed ? -1 : 1) * wheel.deltaY * (source->getMax() - source->getMin());
        if (e.mods.isShiftDown())
            d = d * 0.1;

        auto vn = std::clamp(source->getValue() + d, source->getMin(), source->getMax());
        source->setValue(vn);
    }
    onEndEdit();
    repaint();
}
} // namespace sst::jucegui::components