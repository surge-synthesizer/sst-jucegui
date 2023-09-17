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

    struct Styles : ControlStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"vumeter"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(ControlStyles::styleClass);
        }
    };

    VUMeter(Direction d = VERTICAL) : direction(d), style::StyleConsumer(Styles::styleClass) {}

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

            g.setColour(juce::Colour(100, 100, 140));
            g.fillRect(0.f, vl, getWidth() / 2.f, 1.f * getHeight() - vl);
            g.fillRect(getWidth() / 2.f, vr, getWidth() / 2.f, 1.f * getHeight() - vr);
        }
        else
        {
            g.fillAll(juce::Colours::red);
        }
    }
};
} // namespace sst::jucegui::components
#endif // CONDUIT_VUMETER_H
