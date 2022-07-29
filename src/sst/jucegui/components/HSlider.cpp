//
// Created by Paul Walker on 5/25/22.
//

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
    if (!source)
    {
        g.fillAll(juce::Colours::red);
        return;
    }

    bool vCenter = !showLabel && !showValue;

    if (showLabel)
    {
        g.setColour(getColour(Styles::labeltextcol));
        g.setFont(getFont(Styles::textfont));
        g.drawText(source->getLabel(), getLocalBounds().reduced(2, 1),
                   juce::Justification::bottomLeft);
    }
    if (showValue)
    {
        g.setColour(getColour(Styles::valuetextcol));
        g.setFont(getFont(Styles::textfont));
        g.drawText(source->getValueAsString(), getLocalBounds().reduced(2, 1),
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
    g.setColour(getColour(Styles::backgroundcol));
    g.fillRoundedRectangle(r.toFloat(), gutterheight * 0.25);

    g.setColour(getColour(Styles::guttercol));
    auto gutter = r.reduced(1).toFloat();
    g.fillRoundedRectangle(gutter, gutterheight * 0.25);

    if (modulationDisplay == FROM_ACTIVE)
    {
        g.setColour(getColour(Styles::modactivecol));
        g.fillRoundedRectangle(gutter.reduced(2), gutterheight * 0.25);
    }
    else if (modulationDisplay == FROM_OTHER)
    {
        g.setColour(getColour(Styles::modothercol));
        g.fillRoundedRectangle(gutter.reduced(2), gutterheight * 0.25);
    }

    auto v = source->getValue01();
    auto w = (1 - v) * gutter.getWidth();
    auto hc = gutter.withTrimmedLeft(gutter.getWidth() - w).withWidth(1).expanded(0, 4).getCentre();

    if (source->isBipolar())
    {
        auto t = hc.getX();
        auto b = gutter.getWidth() / 2 + gutter.getX();
        if (t > b)
            std::swap(t, b);
        auto val = gutter.withLeft(t).withRight(b);
        g.setColour(getColour(Styles::valcol));
        g.fillRoundedRectangle(val, gutterheight * 0.25);
    }
    else
    {
        auto val = gutter.withTrimmedRight(w);
        g.setColour(getColour(Styles::valcol));
        g.fillRoundedRectangle(val, gutterheight * 0.25);
    }

    auto hr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(hc);

    auto mvplus = std::clamp(v + source->getModulationValuePM1(), 0.f, 1.f);
    auto mvminus = std::clamp(v - source->getModulationValuePM1(), 0.f, 1.f);
    auto hm = (1.0 - mvplus) * gutter.getWidth();
    auto mpc =
        gutter.withTrimmedLeft(gutter.getWidth() - hm).withWidth(1).expanded(0, 4).getCentre();
    auto mpr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(mpc);

    if (isEditingMod)
    {
        // draw rules
        {
            auto t = hc.getX();
            auto b = (mvplus)*gutter.getWidth() + gutter.getX();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withLeft(t).withRight(b);
            g.setColour(getColour(Styles::modvalcol));
            g.fillRoundedRectangle(val, gutterheight * 0.25);
        }

        if (source->isModulationBipolar())
        {
            auto t = hc.getX();
            auto b = (mvminus)*gutter.getWidth() + gutter.getX();
            if (t > b)
                std::swap(t, b);
            auto val = gutter.withLeft(t).withRight(b);
            g.setColour(getColour(Styles::modvalnegcol));
            g.fillRoundedRectangle(val, gutterheight * 0.25);
        }
    }
    g.setColour(getColour(Styles::handlecol));
    g.fillEllipse(hr);
    if (isEditingMod)
    {
        g.setColour(getColour(Styles::modhandlecol));
        g.fillEllipse(mpr);
    }
}

} // namespace sst::jucegui::components