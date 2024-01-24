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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_TOGGLEDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_TOGGLEDEMO_H

#include <sst/jucegui/components/ToggleButton.h>
#include <sst/jucegui/components/ToggleButtonRadioGroup.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct ToggleDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Toggle Buttons";

    struct SomeToggles : juce::Component
    {
        SomeToggles()
        {
            for (int i = 0; i < 20; ++i)
            {
                auto k = std::make_unique<sst::jucegui::components::ToggleButton>();
                auto d = std::make_unique<ConcreteBinM>();
                char v[2];
                v[1] = 0;
                v[0] = (char)('A' + i);
                k->setLabel(v);

                d->setValueFromModel((rand() % 10 > 5) ? true : false);

                if (i > 10)
                {
                    k->setGlyph(sst::jucegui::components::GlyphPainter::VOLUME);
                }

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
                toggles.push_back(std::move(k));
                sources.push_back(std::move(d));
            }

            int nc{4};
            toggleSource = std::make_unique<ConcreteMultiM>(nc);
            toggleSource->setValueFromModel(rand() % nc);

            toggleGroup = std::make_unique<sst::jucegui::components::ToggleButtonRadioGroup>();
            toggleGroup->setSource(toggleSource.get());
            addAndMakeVisible(*toggleGroup);
        }
        ~SomeToggles()
        {
            for (const auto &k : toggles)
            {
                k->setSource(nullptr);
            }
        }
        void resized() override
        {
            auto r = juce::Rectangle<int>(0, 0, 30, 15).translated(3, 3);
            int kidx = 0;
            for (const auto &k : toggles)
            {
                k->setBounds(r);
                r = r.expanded(10, 3);
                r = r.translated(r.getWidth() + 3, 3);
                kidx++;
                if (kidx % 5 == 0)
                {
                    r = r.withX(3).translated(0, r.getHeight() + 5).withHeight(15).withWidth(30);
                }
            }

            auto q = juce::Rectangle<int>(0, 0, 300, 25).translated(3, 200);
            toggleGroup->setBounds(q);
        }
        std::vector<std::unique_ptr<sst::jucegui::components::ToggleButton>> toggles;
        std::vector<std::unique_ptr<sst::jucegui::data::BinaryDiscrete>> sources;

        std::unique_ptr<sst::jucegui::data::Discrete> toggleSource;
        std::unique_ptr<sst::jucegui::components::ToggleButtonRadioGroup> toggleGroup;
    };

    ToggleDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Toggle Buttons");
        panelOne->setContentAreaComponent(std::make_unique<SomeToggles>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_TOGGLEDEMO_H
