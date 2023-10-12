//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_CustomStyleDEMO_H
#define SST_JUCEGUI_CustomStyleDEMO_H

#include <sst/jucegui/components/HSlider.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/style/StyleSheet.h>
#include "ExampleUtils.h"

struct CustomStyleDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Custom Stylesheets";

    static constexpr sst::jucegui::style::StyleSheet::Class greenclass{"greenslider"};
    static constexpr sst::jucegui::style::StyleSheet::Class redclass{"redslider"};
    static constexpr sst::jucegui::style::StyleSheet::Class pinkclass{"pinkslider"};

    struct SomeSliders : juce::Component
    {
        SomeSliders()
        {
            sst::jucegui::style::StyleSheet::addClass(CustomStyleDemo::greenclass)
                .withBaseClass(sst::jucegui::components::HSlider::Styles::styleClass);
            sst::jucegui::style::StyleSheet::addClass(CustomStyleDemo::redclass)
                .withBaseClass(sst::jucegui::components::HSlider::Styles::styleClass);
            sst::jucegui::style::StyleSheet::addClass(CustomStyleDemo::pinkclass)
                .withBaseClass(sst::jucegui::components::HSlider::Styles::styleClass);
            for (int i = 0; i < 24; ++i)
            {
                auto k = std::make_unique<sst::jucegui::components::HSlider>();
                auto d = std::make_unique<ConcreteCM>();

                if (i % 3 == 2)
                    k->setShowValue(false);
                if (i % 6 == 2)
                    k->setShowLabel(false);
                // every other row of 3 is bipolar
                if ((i / 3) % 2 == 1)
                    d->min = -1;

                int grp = (i % 6) / 3;
                switch (grp)
                {
                case 1:
                    k->setModulationDisplay(
                        sst::jucegui::components::HSlider::Modulatable::FROM_ACTIVE);
                    k->setEditingModulation(true);
                    break;
                default:
                case 0:
                    break;
                }

                int styleGrp = i / 6;
                bool first = (i / 6 == 0);

                switch (styleGrp)
                {
                case 0:
                    break;
                case 1:
                {
                    k->setCustomClass(CustomStyleDemo::pinkclass);
                    break;
                }
                case 2:
                {
                    k->setCustomClass(CustomStyleDemo::redclass);
                    break;
                }
                case 3:
                {
                    k->setCustomClass(CustomStyleDemo::greenclass);
                    break;
                }
                }

                d->setValueFromGUI(1.0 * (rand() % 18502) / 18502.f);
                k->setSource(d.get());
                k->onBeginEdit = []() {};
                k->onEndEdit = []() {};
                k->onPopupMenu = [](const auto &m) {};
                addAndMakeVisible(*k);
                knobs.push_back(std::move(k));
                sources.push_back(std::move(d));
            }
        }
        ~SomeSliders() {}
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
                r = r.translated(0, h + 5);

                kb++;
                if (kb % 12 == 0)
                {
                    r = juce::Rectangle<int>(w0 + 100, 0, w0, h).translated(5, 5);
                }
            }
        }
        std::vector<std::unique_ptr<sst::jucegui::components::HSlider>> knobs;
        std::vector<std::unique_ptr<sst::jucegui::data::ContinuousModulatable>> sources;
    };

    CustomStyleDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Custom Styles");
        panelOne->setContentAreaComponent(std::make_unique<SomeSliders>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    void onStyleChanged() override
    {
        const auto &sk = style();

        sk->setColour(greenclass, sst::jucegui::components::HSlider::Styles::handlecol,
                      juce::Colour(0x00, 0xFF, 0x00));

        sk->setFont(redclass, sst::jucegui::components::HSlider::Styles::labeltextfont,
                    juce::Font("Comic Sans MS", 14, juce::Font::plain));
        sk->setColour(redclass, sst::jucegui::components::HSlider::Styles::labeltextcol,
                      juce::Colours::pink);
        sk->setFont(redclass, sst::jucegui::components::HSlider::Styles::valuetextfont,
                    juce::Font("Times New Roman", 16, juce::Font::bold));
        sk->setColour(redclass, sst::jucegui::components::HSlider::Styles::valuetextcol,
                      juce::Colours::red);

        sk->setColour(pinkclass, sst::jucegui::components::GraphicalControlStyles::guttercol,
                      juce::Colours::pink);

        sst::jucegui::components::WindowPanel::onStyleChanged();
    }
    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_KNOBDEMO_H
