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
    if (!continuous())
    {
        g.fillAll(juce::Colours::red);
        return;
    }

    if (continuous()->isHidden())
        return;

    // Gutter
    auto b = getLocalBounds();
    auto o = b.getWidth() - gutterwidth;
    auto r = b.withTrimmedRight(o * 0.5)
                 .withTrimmedLeft(o * 0.5)
                 .withTrimmedTop(hanRadius + 2)
                 .withTrimmedBottom(hanRadius + 2)
                 .toFloat();

    g.setColour(getColour(Styles::gutter));
    if (isHovered)
        g.setColour(getColour(Styles::gutter_hover));
    auto gutter = r.reduced(1).toFloat();
    g.fillRoundedRectangle(gutter.reduced(1), 0);

    if (!isEnabled())
        return;

    if (modulationDisplay == FROM_ACTIVE)
    {
        g.setColour(getColour(Styles::modulated_by_selected));
        g.fillRoundedRectangle(gutter.reduced(2), 0);
    }
    else if (modulationDisplay == FROM_OTHER)
    {
        g.setColour(getColour(Styles::modulated_by_other));
        g.fillRoundedRectangle(gutter.reduced(2), 0);
    }

    auto v = continuous()->getValue01();
    auto h = (1.0 - v) * gutter.getHeight();
    auto hc = gutter.withTrimmedTop(h).withHeight(1).expanded(0, 4).getCentre();

    auto valcol = getColour(Styles::value);
    if (isHovered)
        valcol = getColour(Styles::value_hover);

    if (continuous()->isBipolar())
    {
        auto t = hc.getY();
        auto b = gutter.getHeight() / 2 + gutter.getY();
        if (t > b)
            std::swap(t, b);
        auto val = gutter.withTop(t).withBottom(b);
        g.setColour(valcol);
        g.fillRoundedRectangle(val, 0);
    }
    else
    {
        auto val = gutter.withTrimmedTop(h);
        g.setColour(valcol);
        g.fillRoundedRectangle(val, 0);
    }

    auto hr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(hc);

    juce::Point<float> mpc;
    juce::Rectangle<float> mpr;

    if (continuousModulatable() && isEditingMod)
    {
        auto mvplus = std::clamp(v + continuousModulatable()->getModulationValuePM1(), 0.f, 1.f);
        auto mvminus = std::clamp(v - continuousModulatable()->getModulationValuePM1(), 0.f, 1.f);
        auto hm = (1.0 - mvplus) * gutter.getHeight();
        mpc = gutter.withTrimmedTop(hm).withHeight(1).expanded(0, 4).getCentre();
        mpr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(mpc);

        auto modvalcol = getColour(Styles::modulation_value);
        if (isHovered)
            modvalcol = getColour(Styles::modulation_value_hover);

        auto modvaloppcol = getColour(Styles::modulation_opposite_value);
        if (isHovered)
            modvaloppcol = getColour(Styles::modulation_opposite_value_hover);

        // draw rules
        {
            auto t = hc.getY();
            auto b = (1 - mvplus) * gutter.getHeight() + gutter.getY();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withTop(t).withBottom(b).reduced(1, 0);
            g.setColour(modvalcol);
            g.fillRoundedRectangle(val, 0);
        }

        if (continuousModulatable()->isModulationBipolar())
        {
            auto t = hc.getY();
            auto b = (1 - mvminus) * gutter.getHeight() + gutter.getY();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withTop(t).withBottom(b).reduced(1, 0);
            g.setColour(modvaloppcol);
            g.fillRoundedRectangle(val, 0);
        }
    }

    if (isHovered)
        g.setColour(getColour(Styles::handle_hover));
    else
        g.setColour(getColour(Styles::handle));
    g.fillEllipse(hr);
    g.setColour(getColour(Styles::handle_outline));
    g.drawEllipse(hr, 0.5);
    if (isEditingMod)
    {
        if (isHovered)
            g.setColour(getColour(Styles::modulation_handle_hover));
        else
            g.setColour(getColour(Styles::modulation_handle));
        g.fillEllipse(mpr);
        g.setColour(getColour(Styles::handle_outline));
        g.drawEllipse(mpr, 1);
    }
}

} // namespace sst::jucegui::components