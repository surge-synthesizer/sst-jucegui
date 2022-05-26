//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_KNOBDEMO_H
#define SST_JUCEGUI_KNOBDEMO_H

#include <sst/jucegui/components/Knob.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct KnobDemo : public sst::jucegui::components::WindowPanel
{
    struct ConcreteCM : sst::jucegui::data::ContinunousModulatable
    {
        std::string label{"A Knob"};
        std::string getLabel() const override { return label; }
        float value{0};
        float getValue() const override { return value; }
        void setValue(const float &f) override
        {
            value = f;
            std::cout << __FILE__ << ":" << __LINE__ << " setValue=" << value << std::endl;
        }

        float min{0}, max{1};
        float getMin() const override { return min; }
        float getMax() const override { return max; }
        float getModulationValuePM1() const override { return 0; }
        void setModulationValuePM1(const float &f) override {}
    };
    struct SomeKnobs : juce::Component
    {
        SomeKnobs()
        {
            for (int i = 0; i < 8; ++i)
            {
                auto k = std::make_unique<sst::jucegui::components::Knob>();
                auto d = std::make_unique<ConcreteCM>();
                if (i >= 4)
                    d->min = -1;
                d->setValue(1.0 * (rand() % 18502) / 18502.f);
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
        ~SomeKnobs()
        {
            for (const auto &k : knobs)
            {
                k->setSource(nullptr);
            }
        }
        void resized() override
        {
            auto b = getLocalBounds();
            int sz = 70;
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
                    sz = 70;
                    r = juce::Rectangle<int>(0, 0, sz, 90).translated(5, tn + 5);
                }
            }
        }
        std::vector<std::unique_ptr<sst::jucegui::components::Knob>> knobs;
        std::vector<std::unique_ptr<sst::jucegui::data::ContinunousModulatable>> sources;
    };

    KnobDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Modulatable Knobs");
        panelOne->setContentAreaComponent(std::make_unique<SomeKnobs>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override {
        panelOne->setBounds(getLocalBounds().reduced(10));
    }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_KNOBDEMO_H
