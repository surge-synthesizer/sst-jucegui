//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_DRAGGABLETEXTDEMO_H
#define SST_JUCEGUI_DRAGGABLETEXTDEMO_H

#include <sst/jucegui/components/DraggableTextEditableValue.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct DraggableTextDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Drag Text Edit";

    struct SomeEditors : juce::Component
    {
        SomeEditors()
        {
            for (int i = 0; i < 18; ++i)
            {
                auto k = std::make_unique<sst::jucegui::components::DraggableTextEditableValue>();
                auto d = std::make_unique<ConcreteCM>();

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
        ~SomeEditors()
        {
            for (const auto &k : knobs)
            {
                k->setSource(nullptr);
            }
        }
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
                r = r.translated(0, h + 5).withTrimmedRight(36);

                kb++;
                if (kb % 9 == 0)
                {
                    h = 35;
                    r = juce::Rectangle<int>(w0 + 100, 0, w0, h).translated(5, 5);
                }
                else if (kb % 3 == 0)
                {
                    h -= 7;
                    r = r.translated(0, 10)
                            .withWidth(170 - (kb % 9) * 10)
                            .withHeight(r.getHeight() - 7);
                }
            }
        }
        std::vector<std::unique_ptr<sst::jucegui::components::DraggableTextEditableValue>> knobs;
        std::vector<std::unique_ptr<sst::jucegui::data::ContinunousModulatable>> sources;
    };

    DraggableTextDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Typeins with Drag");
        panelOne->setContentAreaComponent(std::make_unique<SomeEditors>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_KNOBDEMO_H
