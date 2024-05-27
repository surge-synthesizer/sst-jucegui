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

#include <sst/jucegui/components/HSlider.h>
#include <sst/jucegui/util/DebugHelpers.h>

namespace sst::jucegui::components
{
HSlider::HSlider()
    : style::StyleConsumer(Styles::styleClass), ContinuousParamEditor(Direction::HORIZONTAL)
{
}
HSlider::~HSlider() = default;

void HSlider::paint(juce::Graphics &g)
{
    if (!continuous())
    {
        g.fillAll(juce::Colours::red);
        g.setColour(juce::Colours::white);
        g.drawText("NoSource", getLocalBounds(), juce::Justification::centred);
        return;
    }

    if (continuous()->isHidden())
        return;

    bool vCenter = !showLabel && !showValue;

    if (showLabel)
    {
        if (isHovered)
            g.setColour(getColour(Styles::labelcolor_hover));
        else
            g.setColour(getColour(Styles::labelcolor));
        g.setFont(getFont(Styles::labelfont));
        g.drawText(continuous()->getLabel(), getLocalBounds().reduced(2, 1),
                   juce::Justification::bottomLeft);
    }
    if (showValue)
    {
        if (isHovered)
            g.setColour(getColour(Styles::labelcolor_hover));
        else
            g.setColour(getColour(Styles::labelcolor));

        g.setFont(getFont(Styles::labelfont));
        g.drawText(continuous()->getValueAsString(), getLocalBounds().reduced(2, 1),
                   juce::Justification::bottomRight);
    }

    // Gutter
    auto b = getLocalBounds();
    auto o = b.getHeight() - gutterheight;

    auto r = b.withTrimmedTop(o * 0.5)
                 .withTrimmedBottom(o * 0.5)
                 .withTrimmedLeft(hanRadius + 2)
                 .withTrimmedRight(hanRadius + 2)
                 .toFloat();

    if (!vCenter)
    {
        auto newY = hanRadius + 2;
        if (r.getY() > newY)
            r = r.withY(newY);
    }
    g.setColour(getColour(Styles::outline));
    g.fillRoundedRectangle(r.toFloat(), gutterheight * 0.25);

    if (isHovered)
        g.setColour(getColour(Styles::gutter_hover));
    else
        g.setColour(getColour(Styles::gutter));
    auto gutter = r.reduced(1).toFloat();
    g.fillRoundedRectangle(gutter, gutterheight * 0.25);

    if (modulationDisplay == FROM_ACTIVE)
    {
        g.setColour(getColour(Styles::modulated_by_selected));
        g.fillRoundedRectangle(gutter.reduced(2), gutterheight * 0.25);
    }
    else if (modulationDisplay == FROM_OTHER)
    {
        g.setColour(getColour(Styles::modulated_by_other));
        g.fillRoundedRectangle(gutter.reduced(2), gutterheight * 0.25);
    }

    auto v = continuous()->getValue01();
    auto w = (1 - v) * gutter.getWidth();
    auto hc = gutter.withTrimmedLeft(gutter.getWidth() - w).withWidth(1).expanded(0, 4).getCentre();

    if (continuous()->isBipolar())
    {
        auto t = hc.getX();
        auto b = gutter.getWidth() / 2 + gutter.getX();
        if (t > b)
            std::swap(t, b);
        auto val = gutter.withLeft(t).withRight(b);
        if (isHovered)
            g.setColour(getColour(Styles::value_hover));
        else
            g.setColour(getColour(Styles::value));
        g.fillRoundedRectangle(val, gutterheight * 0.25);
    }
    else
    {
        auto val = gutter.withTrimmedRight(w);
        if (isHovered)
            g.setColour(getColour(Styles::value_hover));
        else
            g.setColour(getColour(Styles::value));
        g.fillRoundedRectangle(val, gutterheight * 0.25);
    }

    auto hr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(hc);

    juce::Point<float> mpc{};
    juce::Rectangle<float> mpr{};
    if (isEditingMod && continuousModulatable())
    {
        auto mvplus = std::clamp(v + continuousModulatable()->getModulationValuePM1(), 0.f, 1.f);
        auto mvminus = std::clamp(v - continuousModulatable()->getModulationValuePM1(), 0.f, 1.f);
        auto hm = (1.0 - mvplus) * gutter.getWidth();
        mpc =
            gutter.withTrimmedLeft(gutter.getWidth() - hm).withWidth(1).expanded(0, 4).getCentre();
        mpr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(mpc);

        auto modvalcol = getColour(Styles::modulation_value);
        if (isHovered)
            modvalcol = getColour(Styles::modulation_value_hover);

        auto modvaloppcol = getColour(Styles::modulation_opposite_value);
        if (isHovered)
            modvaloppcol = getColour(Styles::modulation_opposite_value_hover);

        // draw rules
        {
            auto t = hc.getX();
            auto b = (mvplus)*gutter.getWidth() + gutter.getX();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withLeft(t).withRight(b);
            g.setColour(modvalcol);
            g.fillRoundedRectangle(val, gutterheight * 0.25);
        }

        if (continuousModulatable()->isModulationBipolar())
        {
            auto t = hc.getX();
            auto b = (mvminus)*gutter.getWidth() + gutter.getX();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withLeft(t).withRight(b);
            g.setColour(modvaloppcol);
            g.fillRoundedRectangle(val, gutterheight * 0.25);
        }
    }

    if (isHovered)
        g.setColour(getColour(Styles::handle_hover));
    else
        g.setColour(getColour(Styles::handle));
    g.fillEllipse(hr);
    g.setColour(getColour(Styles::handle_outline));
    g.drawEllipse(hr, 1);
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