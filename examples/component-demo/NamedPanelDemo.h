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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_NAMEDPANELDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_NAMEDPANELDEMO_H

#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct NamedPanelDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Named Panels";

    NamedPanelDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Basic Panel");
        panelOne->setContentAreaComponent(std::make_unique<Solid>(juce::Colours::yellow));
        addAndMakeVisible(*panelOne);

        panelTwo = std::make_unique<sst::jucegui::components::NamedPanel>("Panel Two");
        panelTwo->setContentAreaComponent(std::make_unique<Solid>(juce::Colours::cyan));
        addAndMakeVisible(*panelTwo);

        panelThree =
            std::make_unique<sst::jucegui::components::NamedPanel>("Panel Three Long Name");
        panelThree->setContentAreaComponent(std::make_unique<Solid>(juce::Colours::red));
        addAndMakeVisible(*panelThree);
    }

    void resized() override
    {
        panelOne->setBounds(10, 10, 200, 200);
        panelTwo->setBounds(10, 210, 200, 200);
        panelThree->setBounds(210, 10, 200, 400);
    }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne, panelTwo, panelThree;
};

#endif // SST_JUCEGUI_NAMEDPANELDEMO_H
