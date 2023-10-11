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

#include <sst/jucegui/components/HSliderFilled.h>

namespace sst::jucegui::components
{
HSliderFilled::HSliderFilled()
{
    setShowLabel(false);
    setShowValue(false);
}

void HSliderFilled::paint(juce::Graphics &g)
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

    // Gutter
    auto b = getLocalBounds();
    auto o = b.getHeight() - gutterheight;

    auto r = getLocalBounds().toFloat();
    auto rectRad = 2;

    g.setColour(getColour(Styles::backgroundcol));
    g.fillRoundedRectangle(r.toFloat(), rectRad);

    if (isHovered)
        g.setColour(getColour(Styles::gutterhovcol));
    else
        g.setColour(getColour(Styles::guttercol));
    auto gutter = r.reduced(2).toFloat();
    g.fillRoundedRectangle(gutter, rectRad);

    if (modulationDisplay == FROM_ACTIVE)
    {
        g.setColour(getColour(Styles::modactivecol));
        g.fillRoundedRectangle(gutter.reduced(2), rectRad);
    }
    else if (modulationDisplay == FROM_OTHER)
    {
        g.setColour(getColour(Styles::modothercol));
        g.fillRoundedRectangle(gutter.reduced(2), rectRad);
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
        g.setColour(getColour(Styles::valcol));
        g.fillRoundedRectangle(val, rectRad);
    }
    else
    {
        auto val = gutter.withTrimmedRight(w);
        g.setColour(getColour(Styles::valcol));
        g.fillRoundedRectangle(val, rectRad);
    }

    auto hr = juce::Rectangle<float>(2, gutter.getHeight()).withCentre(hc);

    juce::Point<float> mpc;
    juce::Rectangle<float> mpr;
    if (continuousModulatable() && isEditingMod)
    {
        auto mvplus = std::clamp(v + continuousModulatable()->getModulationValuePM1(), 0.f, 1.f);
        auto mvminus = std::clamp(v - continuousModulatable()->getModulationValuePM1(), 0.f, 1.f);
        auto hm = (1.0 - mvplus) * gutter.getWidth();
        mpc =
            gutter.withTrimmedLeft(gutter.getWidth() - hm).withWidth(1).expanded(0, 4).getCentre();
        mpr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(mpc);

        // draw rules
        {
            auto t = hc.getX();
            auto b = (mvplus)*gutter.getWidth() + gutter.getX();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withLeft(t).withRight(b);
            g.setColour(getColour(Styles::modvalcol));
            g.fillRoundedRectangle(val, rectRad);
        }

        if (continuousModulatable()->isModulationBipolar())
        {
            auto t = hc.getX();
            auto b = (mvminus)*gutter.getWidth() + gutter.getX();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withLeft(t).withRight(b);
            g.setColour(getColour(Styles::modvalnegcol));
            g.fillRoundedRectangle(val, rectRad);
        }
    }

    if (isHovered)
        g.setColour(getColour(Styles::handlehovcol));
    else
        g.setColour(getColour(Styles::handlecol));
    g.fillRect(hr);
    if (isEditingMod)
    {
        if (isHovered)
            g.setColour(getColour(Styles::modhandlehovcol));
        else
            g.setColour(getColour(Styles::modhandlecol));
        g.fillEllipse(mpr);
    }
}

} // namespace sst::jucegui::components