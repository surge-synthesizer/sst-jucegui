//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/components/VSlider.h>

namespace sst::jucegui::components
{
VSlider::VSlider() : style::StyleConsumer(Styles::styleClass) {}
VSlider::~VSlider() = default;

void VSlider::paint(juce::Graphics &g)
{
    // Gutter
    auto b = getLocalBounds();
    auto o = b.getWidth() - gutterwidth;
    auto r = b.withTrimmedRight(o * 0.5)
                 .withTrimmedLeft(o * 0.5)
                 .withTrimmedTop(hanRadius + 2)
                 .withTrimmedBottom(hanRadius + 2)
                 .toFloat();
    g.setColour(getColour(Styles::backgroundcol));
    g.fillRoundedRectangle(r.toFloat(), gutterwidth * 0.25);

    g.setColour(getColour(Styles::guttercol));
    auto gutter = r.reduced(1).toFloat();
    g.fillRoundedRectangle(gutter, gutterwidth * 0.25);

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
        auto b = gutter.getHeight() / 2;
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

    if (isEditingMod)
    {
    }
    else
    {
        auto hr = juce::Rectangle<float>(2 * hanRadius, 2 * hanRadius).withCentre(hc);

        g.setColour(getColour(Styles::handlecol));
        g.fillEllipse(hr);
    }
}

} // namespace sst::jucegui::components