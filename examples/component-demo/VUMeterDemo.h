/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_VUMETERDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_VUMETERDEMO_H

#include <sst/jucegui/components/VUMeter.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct VUMeterDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "VU Meters";

    struct SomeVU : juce::Component, juce::Timer
    {
        SomeVU()
        {
            for (int i = 0; i < nVU; ++i)
            {
                auto v = std::make_unique<sst::jucegui::components::VUMeter>();
                v->direction = sst::jucegui::components::VUMeter::VERTICAL;
                addAndMakeVisible(*v);
                vus[i] = std::move(v);
                values[0][i] = (rand() % 1000) * 0.0017f;
                values[1][i] = values[0][i] * (1.1 - (rand() % 1000) * 0.001f * .2);
                vus[i]->setLevels(values[0][1], values[1][i]);

                auto vh = std::make_unique<sst::jucegui::components::VUMeter>();
                vh->direction = sst::jucegui::components::VUMeter::HORIZONTAL;
                addAndMakeVisible(*vh);
                vusH[i] = std::move(vh);
                vusH[i]->setLevels(values[0][1], values[1][i]);
            }
            startTimerHz(30);
        }
        ~SomeVU() { stopTimer(); }

        void timerCallback() override
        {
            for (int i = 0; i < nVU; ++i)
            {
                if (rand() % 100 > 96)
                {
                    values[0][i] = (rand() % 1000) * 0.0017f;
                    auto mv = (1.1 - (rand() % 1000) * 0.001f * .2);
                    values[1][i] = values[0][i] * mv;
                }
                else
                {
                    values[0][i] = 0.9 * values[0][i];
                    values[1][i] = 0.9 * values[1][i];
                }

                vusH[i]->setLevels(values[0][i], values[1][i]);
                vusH[i]->repaint();
                vus[i]->setLevels(values[0][i], values[1][i]);
                vus[i]->repaint();
            }
        }
        void resized() override
        {
            auto r = juce::Rectangle<int>(0, 0, 200, 15).translated(3, 3);
            int kidx = 0;
            for (const auto &k : vusH)
            {
                k->setBounds(r);
                r = r.translated(0, r.getHeight() + 3)
                        .withHeight(r.getHeight() + 3)
                        .withWidth(r.getWidth() + 10);
                kidx++;
            }

            r = juce::Rectangle<int>(300, 0, 15, 200).translated(3, 3);
            for (const auto &k : vus)
            {
                k->setBounds(r);
                r = r.translated(r.getWidth() + 3, 0)
                        .withHeight(r.getHeight() + 20)
                        .withWidth(r.getWidth() + 3);
            }
        }

        static constexpr int nVU{5};

        std::array<std::array<float, nVU>, 2> values;
        std::array<std::unique_ptr<sst::jucegui::components::VUMeter>, nVU> vus, vusH;
    };

    VUMeterDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Toggle Buttons");
        panelOne->setContentAreaComponent(std::make_unique<SomeVU>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_TOGGLEDEMO_H
