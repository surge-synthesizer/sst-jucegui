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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_VUMETER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_VUMETER_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

namespace sst::jucegui::components
{
struct VUMeter : public juce::Component, public style::StyleConsumer, public style::SettingsConsumer
{
    enum Direction
    {
        VERTICAL,
        HORIZONTAL
    } direction{VERTICAL};

    struct Styles : base_styles::Outlined
    {
        SCLASS(vumeter);

        PROP(vu_gutter);
        PROP(vu_gradstart);
        PROP(vu_gradend);
        PROP(vu_overload);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::Outlined::styleClass)
                .withProperty(vu_gutter)
                .withProperty(vu_gradstart)
                .withProperty(vu_gradend)
                .withProperty(vu_overload);
        }
    };

    VUMeter(Direction d = VERTICAL) : style::StyleConsumer(Styles::styleClass), direction(d) {}

    float L{0}, R{0};
    void setLevels(float iL, float iR)
    {
        L = iL;
        R = iR;
        repaint();
    }
    void paint(juce::Graphics &g)
    {
        if (direction == VERTICAL)
        {
            g.setColour(getColour(Styles::vu_gutter));
            g.fillRect(getLocalBounds().reduced(1));

            float zerodb = (0.7937 * getHeight());
            auto scale = [](float x) {
                x = std::clamp(0.5f * x, 0.f, 1.f);
                return powf(x, 0.3333333333f);
            };

            auto vl = getHeight() - scale(L) * getHeight();
            auto vr = getHeight() - scale(R) * getHeight();

            auto fg = juce::ColourGradient::vertical(getColour(Styles::vu_gradstart), zerodb,
                                                     getColour(Styles::vu_gradend), getHeight());
            g.setGradientFill(fg);
            g.fillRect(0.f, vl, getWidth() / 2.f - 0.5, 1.f * getHeight() - vl);
            g.fillRect(getWidth() / 2.f + 0.5, vr, getWidth() / 2.f - 0.5, 1.f * getHeight() - vr);

            if (vl < getHeight() - zerodb)
            {
                g.setColour(getColour(Styles::vu_overload));
                g.fillRect(0.f, vl, getWidth() / 2.f - 0.5, getHeight() - zerodb - vl);
            }

            if (vr < getHeight() - zerodb)
            {
                g.setColour(getColour(Styles::vu_overload));
                g.fillRect(getWidth() / 2.f + 0.5, vr, getWidth() / 2.f - 0.5,
                           getHeight() - zerodb - vr);
            }

            g.setColour(getColour(Styles::outline));
            g.drawRect(getLocalBounds(), 1);
        }
        else
        {
            float zerodb = (0.7937 * getWidth());
            auto scale = [](float x) {
                x = std::clamp(0.5f * x, 0.f, 1.f);
                return powf(x, 0.3333333333f);
            };

            auto vl = scale(L) * getWidth();
            auto vr = scale(R) * getWidth();

            auto rLeft = getLocalBounds().withHeight(getHeight() / 2).reduced(0, 2);
            auto rRight = getLocalBounds().withTrimmedTop(getHeight() / 2).reduced(0, 2);

            g.setColour(getColour(Styles::vu_gutter));

            g.fillRect(rLeft);
            g.fillRect(rRight);

            auto fg = juce::ColourGradient::horizontal(getColour(Styles::vu_gradend), 0,
                                                       getColour(Styles::vu_gradstart), zerodb);
            g.setGradientFill(fg);
            g.fillRect(rLeft.withWidth(vl));
            g.fillRect(rRight.withWidth(vr));

            if (vl > zerodb)
            {
                g.setColour(getColour(Styles::vu_overload));
                g.fillRect(rLeft.withWidth(vl).withTrimmedLeft(zerodb));
            }

            if (vr > zerodb)
            {
                g.setColour(getColour(Styles::vu_overload));
                g.fillRect(rRight.withWidth(vr).withTrimmedLeft(zerodb));
            }

            g.setColour(getColour(Styles::outline));
            g.drawRect(getLocalBounds(), 1);
        }
    }
};
} // namespace sst::jucegui::components
#endif // CONDUIT_VUMETER_H
