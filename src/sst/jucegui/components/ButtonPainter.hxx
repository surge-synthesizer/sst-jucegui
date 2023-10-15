//
// Created by Paul Walker on 10/15/23.
//

#ifndef SST_JUCEGUI_BUTTONPAINTER_HXX
#define SST_JUCEGUI_BUTTONPAINTER_HXX

#include <juce_graphics/juce_graphics.h>

namespace sst::jucegui::components
{
template <typename T> void paintButtonBG(T *that, juce::Graphics &g)
{
    float rectCorner = 1.5;

    auto b = that->getLocalBounds().reduced(1).toFloat();

    auto bg = that->getColour(T::Styles::fill);

    if (that->isPressed)
    {
        bg = that->getColour(T::Styles::fill_pressed);
    }
    else if (that->isHovered)
    {
        bg = that->getColour(T::Styles::fill_hover);
    }

    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    auto ip = that->isPressed;
    auto gr =
        juce::ColourGradient::vertical(bg.withAlpha(0.f), that->getHeight() * 0.6,
                                       ip ? bg.brighter(0.1) : bg.darker(0.3), that->getHeight());
    g.setGradientFill(gr);
    g.fillRoundedRectangle(b, rectCorner);

    gr = juce::ColourGradient::vertical(ip ? bg.darker(0.2) : bg.brighter(0.1), 0,
                                        bg.withAlpha(0.f), that->getHeight() * 0.1);
    g.setGradientFill(gr);
    g.fillRoundedRectangle(b, rectCorner);

    g.setColour(that->getColour(T::Styles::outline));
    g.drawRoundedRectangle(b, rectCorner, 1);
}

// Only call this in the 'on' state
template <typename T> void paintButtonOnValueBG(T *that, juce::Graphics &g)
{
    float rectCorner = 1.5;

    auto b = that->getLocalBounds().reduced(1).toFloat();
    auto bg = that->getColour(T::Styles::value);

    if (that->isHovered)
    {
        bg = that->getColour(T::Styles::value_hover);
    }

    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    auto gr = juce::ColourGradient::vertical(bg.withAlpha(0.f), that->getHeight() * 0.6,
                                             bg.brighter(0.3), that->getHeight());
    g.setGradientFill(gr);
    g.fillRoundedRectangle(b, rectCorner);

    gr = juce::ColourGradient::vertical(bg.darker(0.5), 0, bg.withAlpha(0.f),
                                        that->getHeight() * 0.2);
    g.setGradientFill(gr);
    g.fillRoundedRectangle(b, rectCorner);

    g.setColour(that->getColour(T::Styles::outline));
    g.drawRoundedRectangle(b, rectCorner, 1);
}
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_BUTTONPAINTER_HXX
