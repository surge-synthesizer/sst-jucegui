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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_MULTISWITCHDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_MULTISWITCHDEMO_H

#include <sst/jucegui/components/MultiSwitch.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct MultiSwitchDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "MultiSwitch";

    struct SomeMultiSwitchs : juce::Component
    {
        SomeMultiSwitchs()
        {
            for (int i = 0; i < 20; ++i)
            {
                auto dir = sst::jucegui::components::MultiSwitch::VERTICAL;
                if (i >= 15)
                    dir = sst::jucegui::components::MultiSwitch::HORIZONTAL;
                auto k = std::make_unique<sst::jucegui::components::MultiSwitch>(dir);
                int nc = (i / 5) + 5;
                if (i >= 15)
                    nc = 5;
                auto d = std::make_unique<ConcreteMultiM>(nc);

                d->setValueFromModel(rand() % nc);

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
                MultiSwitchs.push_back(std::move(k));
                sources.push_back(std::move(d));
            }
        }
        ~SomeMultiSwitchs()
        {
            for (const auto &k : MultiSwitchs)
            {
                k->setSource(nullptr);
            }
        }
        void resized() override
        {
            auto ridx = 6;
            auto dh = 0;
            auto r = juce::Rectangle<int>(0, 0, 50, 15 * ridx).translated(3, 3);
            int kidx = 0;
            for (const auto &k : MultiSwitchs)
            {
                if (kidx < 15)
                {
                    k->setBounds(r);
                    r = r.expanded(10, 0);
                    r = r.translated(r.getWidth() + 3, 0);
                    kidx++;
                    if (kidx % 5 == 0)
                    {
                        ridx++;
                        r = r.withX(3)
                                .translated(0, r.getHeight() + 5)
                                .withHeight(15 * ridx)
                                .withWidth(50);
                    }
                }
                else
                {
                    if (kidx == 15)
                    {
                        r = r.withX(3).withHeight(20).withWidth(400);
                    }
                    else
                    {
                        r = r.translated(0, r.getHeight() + 3);
                    }
                    r = r.withWidth((kidx - 15) * 60 + 250);
                    k->setBounds(r);

                    kidx++;
                }
            }
        }
        std::vector<std::unique_ptr<sst::jucegui::components::MultiSwitch>> MultiSwitchs;
        std::vector<std::unique_ptr<sst::jucegui::data::Discrete>> sources;
    };

    MultiSwitchDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("MultiSwitch Buttons");
        panelOne->setContentAreaComponent(std::make_unique<SomeMultiSwitchs>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_MultiSwitchDEMO_H
