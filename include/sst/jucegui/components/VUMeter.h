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
            float zerodb = (0.7937 * getHeight());
            auto scale = [](float x) {
                x = std::clamp(0.5f * x, 0.f, 1.f);
                return powf(x, 0.3333333333f);
            };

            auto vl = getHeight() - scale(L) * getHeight();
            auto vr = getHeight() - scale(R) * getHeight();

            int gap{0};
            auto rLeft =
                getLocalBounds().toFloat().withWidth(getWidth() / 2.f).withTrimmedRight(gap);
            auto rRight = getLocalBounds().toFloat().withTrimmedLeft(getWidth() / 2.f + gap);

            g.setColour(getColour(Styles::vu_gutter));
            g.fillRect(rLeft);
            g.fillRect(rRight);

            auto fg =
                juce::ColourGradient::vertical(getColour(Styles::vu_gradend), getHeight() - zerodb,
                                               getColour(Styles::vu_gradstart), getHeight());
            g.setGradientFill(fg);
            g.fillRect(rLeft.withTrimmedTop(vl));
            g.fillRect(rRight.withTrimmedTop(vr));

            if (vl < getHeight() - zerodb)
            {
                g.setColour(getColour(Styles::vu_overload));
                g.fillRect(rLeft.withTrimmedTop(vl));
            }

            if (vr < getHeight() - zerodb)
            {
                g.setColour(getColour(Styles::vu_overload));
                g.fillRect(rRight.withTrimmedTop(vr));
            }

            g.setColour(getColour(Styles::vu_gutter));

            // Dont' draw the top line hence the offset
            for (int i = getHeight() - 2; i > vl + 1; i -= 3)
            {
                g.drawHorizontalLine(i, rLeft.getX(), rLeft.getX() + rLeft.getWidth());
            }
            for (int i = getHeight() - 2; i > vr + 1; i -= 3)
            {
                g.drawHorizontalLine(i, rRight.getX(), rRight.getX() + rRight.getWidth());
            }

            g.setColour(getColour(Styles::vu_gutter));
            if (gap == 0)
            {
                g.drawRect(getLocalBounds(), 1);
                g.drawVerticalLine(getWidth() / 2.f, 0, getHeight());
            }
            else
            {
                g.drawRect(rLeft, 1);
                g.drawRect(rRight, 1);
            }
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

            auto fg = juce::ColourGradient::horizontal(getColour(Styles::vu_gradstart), 0,
                                                       getColour(Styles::vu_gradend), zerodb);
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

            g.setColour(getColour(Styles::vu_gutter));

            for (int i = 0; i < vl; i += 3)
            {
                g.drawVerticalLine(i, rLeft.getY(), rLeft.getY() + rLeft.getHeight());
            }
            for (int i = 0; i < vr; i += 3)
            {
                g.drawVerticalLine(i, rRight.getY(), rRight.getY() + rRight.getHeight());
            }

            g.setColour(getColour(Styles::vu_gutter));
            g.drawRect(rLeft, 1);
            g.drawRect(rRight, 1);
        }
    }
};
} // namespace sst::jucegui::components
#endif // CONDUIT_VUMETER_H
