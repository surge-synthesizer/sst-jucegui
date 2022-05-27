//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_VSLIDERDEMO_H
#define SST_JUCEGUI_VSLIDERDEMO_H

#include <sst/jucegui/components/VSlider.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct VSliderDemo : public sst::jucegui::components::WindowPanel
{
    struct SomeSliders : juce::Component
    {
        SomeSliders()
        {
            for (int i = 0; i < 32; ++i)
            {
                auto k = std::make_unique<sst::jucegui::components::VSlider>();
                auto d = std::make_unique<ConcreteCM>();
                // every other row of 4 is bipolar
                if ((i / 4) % 2 == 1)
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
        ~SomeSliders()
        {
            for (const auto &k : knobs)
            {
                k->setSource(nullptr);
            }
        }
        void resized() override
        {
            auto b = getLocalBounds();
            int h0 = 200;
            auto r = juce::Rectangle<int>(0, 0, 30, h0).translated(5, 5);
            int kb = 0;
            for (auto &k : knobs)
            {
                k->setBounds(r);
                r = r.translated(35, 18).withTrimmedBottom(36);

                kb++;
                if (kb % 16 == 0)
                {
                    r = juce::Rectangle<int>(0, h0 + 100, 30, h0).translated(5, 5);
                }
                if (kb % 4 == 0)
                {
                    r = r.withHeight(h0).translated(0, -4 * 18);
                }
            }
        }
        std::vector<std::unique_ptr<sst::jucegui::components::VSlider>> knobs;
        std::vector<std::unique_ptr<sst::jucegui::data::ContinunousModulatable>> sources;
    };

    VSliderDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Modulatable Sliders");
        panelOne->setContentAreaComponent(std::make_unique<SomeSliders>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_KNOBDEMO_H
