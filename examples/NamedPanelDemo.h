//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_NAMEDPANELDEMO_H
#define SST_JUCEGUI_NAMEDPANELDEMO_H

#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct NamedPanelDemo : public sst::jucegui::components::WindowPanel
{
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
        panelThree->addStyleSuperclass({"greenpanel"});
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
