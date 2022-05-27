//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_COUPLEDDEMO_H
#define SST_JUCEGUI_COUPLEDDEMO_H

#include <sst/jucegui/components/VSlider.h>
#include <sst/jucegui/components/Knob.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct CoupledControlsDemo : public sst::jucegui::components::WindowPanel
{
    struct MixedControls : juce::Component
    {
        MixedControls()
        {
            source0 = std::make_unique<ConcreteCM>();
            source1 = std::make_unique<ConcreteCM>();
            source1->min = -1;
            source0->setValueFromGUI(0.9);
            source1->setValueFromGUI(0.44);
            for (int i = 0; i < 4; ++i)
            {
                auto s = std::make_unique<sst::jucegui::components::VSlider>();
                auto k = std::make_unique<sst::jucegui::components::Knob>();

                if (i % 2 == 0)
                {
                    k->setSource(source0.get());
                    s->setSource(source0.get());
                }
                else
                {
                    k->setSource(source1.get());
                    s->setSource(source1.get());
                }
                addAndMakeVisible(*k);
                addAndMakeVisible(*s);
                knobs.push_back(std::move(k));
                sliders.push_back(std::move(s));
            }
        }
        ~MixedControls()
        {
            for (const auto &k : knobs)
            {
                k->setSource(nullptr);
            }
            for (const auto &k : sliders)
            {
                k->setSource(nullptr);
            }
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
        std::unique_ptr<ConcreteCM> source0, source1;
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
