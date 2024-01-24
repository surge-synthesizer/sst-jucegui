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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_KNOBDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_KNOBDEMO_H

#include <sst/jucegui/components/Knob.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/util/DebugHelpers.h>
#include "ExampleUtils.h"

struct KnobDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Knobs";

    struct SomeKnobs : juce::Component
    {
        SomeKnobs()
        {
            for (int i = 0; i < 32; ++i)
            {
                auto k = std::make_unique<sst::jucegui::components::Knob>();
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
                k->onBeginEdit = []() { DBGOUT("beginEdit"); };
                k->onEndEdit = []() { DBGOUT("endEdit"); };
                k->onPopupMenu = [](const auto &m) { DBGOUT("popupMenu"); };
                addAndMakeVisible(*k);
                knobs.push_back(std::move(k));
                sources.push_back(std::move(d));
            }
        }
        ~SomeKnobs() {}
        void resized() override
        {
            auto b = getLocalBounds();
            int sz = 70;
            int xp = 0;
            int tn = 0;
            auto r = juce::Rectangle<int>(0, 0, sz, 90).translated(5, tn + 5);
            int kb = 0;
            for (auto &k : knobs)
            {
                k->setBounds(r);
                r = r.translated(sz + 10, 0);
                r = r.withTrimmedRight(10).withTrimmedBottom(10);
                sz -= 10;
                kb++;
                if (kb % 4 == 0)
                {
                    tn = tn + 100;
                    if (kb == 16)
                    {
                        tn = 0;
                        xp = 300;
                    }
                    sz = 70;
                    r = juce::Rectangle<int>(xp, 0, sz, 90).translated(5, tn + 5);
                }
            }
        }
        std::vector<std::unique_ptr<sst::jucegui::components::Knob>> knobs;
        std::vector<std::unique_ptr<sst::jucegui::data::ContinuousModulatable>> sources;
    };

    KnobDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Modulatable Knobs");
        panelOne->setContentAreaComponent(std::make_unique<SomeKnobs>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_KNOBDEMO_H
