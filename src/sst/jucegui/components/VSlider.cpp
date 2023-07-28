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

#include <sst/jucegui/components/VSlider.h>

namespace sst::jucegui::components
{
VSlider::VSlider()
    : style::StyleConsumer(Styles::styleClass), ContinuousParamEditor(Direction::VERTICAL)
{
}
VSlider::~VSlider() = default;

void VSlider::paint(juce::Graphics &g)
{
    if (!source)
    {
        g.fillAll(juce::Colours::red);
        return;
    }

    if (source->isHidden())
        return;

    // Gutter
    auto b = getLocalBounds();
    auto o = b.getWidth() - gutterwidth;
    auto r = b.withTrimmedRight(o * 0.5)
                 .withTrimmedLeft(o * 0.5)
                 .withTrimmedTop(hanRadius + 2)
                 .withTrimmedBottom(hanRadius + 2)
                 .toFloat();
    g.setColour(getColour(Styles::backgroundcol));
    g.fillRoundedRectangle(r.reduced(1).toFloat(), gutterwidth * 0.25);

    g.setColour(getColour(Styles::guttercol));
    auto gutter = r.reduced(1).toFloat();
    g.fillRoundedRectangle(gutter.reduced(1), gutterwidth * 0.25);

    if (!isEnabled())
        return;

    if (modulationDisplay == FROM_ACTIVE)
    {
        g.setColour(getColour(Styles::modactivecol));
        g.fillRoundedRectangle(gutter.reduced(2), gutterwidth * 0.25);
    }
    else if (modulationDisplay == FROM_OTHER)
    {
        g.setColour(getColour(Styles::modothercol));
        g.fillRoundedRectangle(gutter.reduced(2), gutterwidth * 0.25);
    }

    auto v = source->getValue01();
    auto h = (1.0 - v) * gutter.getHeight();
    auto hc = gutter.withTrimmedTop(h).withHeight(1).expanded(0, 4).getCentre();

    if (source->isBipolar())
    {
        auto t = hc.getY();
        auto b = gutter.getHeight() / 2 + gutter.getY();
        if (t > b)
            std::swap(t, b);
        auto val = gutter.withTop(t).withBottom(b);
        g.setColour(getColour(Styles::valcol));
        g.fillRoundedRectangle(val, gutterwidth * 0.25);
    }
    else
    {
        auto val = gutter.withTrimmedTop(h);
        g.setColour(getColour(Styles::valcol));
        g.fillRoundedRectangle(val, gutterwidth * 0.25);
    }

    auto hr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(hc);

    auto mvplus = std::clamp(v + source->getModulationValuePM1(), 0.f, 1.f);
    auto mvminus = std::clamp(v - source->getModulationValuePM1(), 0.f, 1.f);
    auto hm = (1.0 - mvplus) * gutter.getHeight();
    auto mpc = gutter.withTrimmedTop(hm).withHeight(1).expanded(0, 4).getCentre();
    auto mpr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(mpc);

    if (isEditingMod)
    {
        // draw rules
        {
            auto t = hc.getY();
            auto b = (1 - mvplus) * gutter.getHeight() + gutter.getY();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withTop(t).withBottom(b).reduced(1, 0);
            g.setColour(getColour(Styles::modvalcol));
            g.fillRoundedRectangle(val, gutterwidth * 0.25);
        }

        if (source->isModulationBipolar())
        {
            auto t = hc.getY();
            auto b = (1 - mvminus) * gutter.getHeight() + gutter.getY();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withTop(t).withBottom(b).reduced(1, 0);
            g.setColour(getColour(Styles::modvalnegcol));
            g.fillRoundedRectangle(val, gutterwidth * 0.25);
        }
    }
    g.setColour(getColour(Styles::handlecol));
    g.fillEllipse(hr);
    g.setColour(getColour(Styles::handlebordercol));
    g.drawEllipse(hr, 0.5);
    if (isEditingMod)
    {
        g.setColour(getColour(Styles::modhandlecol));
        g.fillEllipse(mpr);
    }
}

} // namespace sst::jucegui::components