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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_COUPLEDCONTROLS_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_COUPLEDCONTROLS_H

#include <sst/jucegui/components/VSlider.h>
#include <sst/jucegui/components/Knob.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct CoupledControlsDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Coupled Controls";

    struct MixedControls : juce::Component
    {
        std::unique_ptr<juce::Timer> idleTimer;
        struct Idle : public juce::Timer
        {
            MixedControls *controls{nullptr};
            Idle(MixedControls *c) : controls(c) {}
            void timerCallback() override { controls->idle(); }
        };

        MixedControls()
        {
            source0 = std::make_unique<ConcreteCM>();
            source1 = std::make_unique<ConcreteCM>();
            source2 = std::make_unique<ConcreteCM>();
            source1->min = -1;
            source0->setValueFromGUI(0.9);
            source1->setValueFromGUI(0.44);
            source2->setValueFromGUI(0.0);
            for (int i = 0; i < 6; ++i)
            {
                auto s = std::make_unique<sst::jucegui::components::VSlider>();
                auto k = std::make_unique<sst::jucegui::components::Knob>();

                if (i % 3 == 0)
                {
                    k->setSource(source0.get());
                    s->setSource(source0.get());
                }
                else if (i % 3 == 1)
                {
                    k->setSource(source1.get());
                    s->setSource(source1.get());
                }
                else if (i % 3 == 2)
                {
                    k->setSource(source2.get());
                    s->setSource(source2.get());
                }
                addAndMakeVisible(*k);
                addAndMakeVisible(*s);
                knobs.push_back(std::move(k));
                sliders.push_back(std::move(s));
            }

            idleTimer = std::make_unique<Idle>(this);
            idleTimer->startTimer(1000.0 / 60.0);
        }
        ~MixedControls() { idleTimer->stopTimer(); }

        float ival = 0.f;
        void idle()
        {
            ival += 0.01 * source0->getValue();
            if (ival >= 1)
                ival -= 1.f;
            source2->setValueFromModel(ival);
        }
        void resized() override
        {
            auto b = getLocalBounds();
            int h0 = 200;
            auto r = juce::Rectangle<int>(0, 0, 30, h0).translated(5, 5);
            for (auto &s : sliders)
            {
                s->setBounds(r);
                r = r.translated(35, 0);
            }
            r = juce::Rectangle<int>(0, h0 + 50, 70, 90).translated(5, 5);
            for (auto &s : knobs)
            {
                s->setBounds(r);
                r = r.translated(75, 0);
            }
        }
        std::vector<std::unique_ptr<sst::jucegui::components::VSlider>> sliders;
        std::vector<std::unique_ptr<sst::jucegui::components::Knob>> knobs;
        std::unique_ptr<ConcreteCM> source0, source1, source2;
    };

    CoupledControlsDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Coupled Controls");
        panelOne->setContentAreaComponent(std::make_unique<MixedControls>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_KNOBDEMO_H
