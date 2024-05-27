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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_HSLIDERDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_HSLIDERDEMO_H

#include <sst/jucegui/components/HSlider.h>
#include <sst/jucegui/components/HSliderFilled.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct HSliderDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "H-Sliders";

    struct SomeSliders : juce::Component
    {
        SomeSliders()
        {
            for (int i = 0; i < 24; ++i)
            {
                auto k = std::make_unique<sst::jucegui::components::HSlider>();
                if (rand() % 10 > 7)
                    k = std::make_unique<sst::jucegui::components::HSliderFilled>();
                auto d = std::make_unique<ConcreteCM>();

                if (i % 3 == 2)
                    k->setShowValue(false);
                if (i % 6 == 2)
                    k->setShowLabel(false);
                // every other row of 3 is bipolar
                if ((i / 3) % 2 == 1)
                    d->min = -1;

                int grp = i / 8;
                switch (grp)
                {
                case 1:
                    k->setModulationDisplay(
                        sst::jucegui::components::Knob::Modulatable::FROM_ACTIVE);
                    k->setEditingModulation(true);
                    break;
                case 2:
                {
                    auto r = rand() % 10;
                    if (r < 3)
                        k->setModulationDisplay(
                            sst::jucegui::components::Knob::Modulatable::FROM_ACTIVE);
                    else if (r > 7)
                        k->setModulationDisplay(
                            sst::jucegui::components::Knob::Modulatable::FROM_OTHER);
                    break;
                }
                default:
                case 0:
                    break;
                }

                d->setValueFromGUI(1.0 * (rand() % 18502) / 18502.f);
                k->setSource(d.get());
                k->onBeginEdit = []() {
                    std::cout << __FILE__ << ":" << __LINE__ << " beginEdit" << std::endl;
                };
                k->onEndEdit = []() {
                    std::cout << __FILE__ << ":" << __LINE__ << " endEdit" << std::endl;
                };
                k->onPopupMenu = [](const auto &m) {
                    std::cout << __FILE__ << ":" << __LINE__ << " popupMenu" << std::endl;
                };
                addAndMakeVisible(*k);
                knobs.push_back(std::move(k));
                sources.push_back(std::move(d));
            }
        }
        ~SomeSliders() {}
        void resized() override
        {
            auto b = getLocalBounds();
            int w0 = 200;
            auto h = 35;
            auto r = juce::Rectangle<int>(0, 0, w0, h).translated(5, 5);
            int kb = 0;
            for (auto &k : knobs)
            {
                k->setBounds(r);
                r = r.translated(18, h + 5).withTrimmedRight(36);

                kb++;
                if (kb % 12 == 0)
                {
                    r = juce::Rectangle<int>(w0 + 100, 0, w0, h).translated(5, 5);
                }
                if (kb % 3 == 0)
                {
                    r = r.withWidth(w0).translated(-3 * 18, 0);
                }
            }
        }
        std::vector<std::unique_ptr<sst::jucegui::components::HSlider>> knobs;
        std::vector<std::unique_ptr<sst::jucegui::data::ContinuousModulatable>> sources;
    };

    HSliderDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Modulatable Sliders");
        panelOne->setContentAreaComponent(std::make_unique<SomeSliders>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_KNOBDEMO_H
