//
// Created by Paul Walker on 5/30/22.
//

#ifndef SST_JUCEGUI_TOGGLEDEMO_H
#define SST_JUCEGUI_TOGGLEDEMO_H

#include <sst/jucegui/components/ToggleButton.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct ToggleDemo : public sst::jucegui::components::WindowPanel
{

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
        }
        std::vector<std::unique_ptr<sst::jucegui::components::ToggleButton>> toggles;
        std::vector<std::unique_ptr<sst::jucegui::data::BinaryDiscrete>> sources;
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
