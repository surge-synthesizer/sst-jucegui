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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_TEXTPUSHBUTTONDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_TEXTPUSHBUTTONDEMO_H

#include <sst/jucegui/components/TextPushButton.h>
#include <sst/jucegui/components/GlyphButton.h>
#include <sst/jucegui/components/MenuButton.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include "ExampleUtils.h"

struct TextPushButtonDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Text Push Buttons";

    struct SomeTPB : juce::Component
    {
        SomeTPB()
        {
            for (int i = 0; i < 5; ++i)
            {
                auto k = std::make_unique<sst::jucegui::components::TextPushButton>();
                k->setLabel("Button " + std::to_string(i + 1));
                k->setOnCallback([i]() { std::cout << "You pressed button " << i << std::endl; });
                addAndMakeVisible(*k);
                tpb.push_back(std::move(k));

                auto g = std::make_unique<sst::jucegui::components::GlyphButton>(
                    sst::jucegui::components::GlyphPainter::PAN);
                g->setOnCallback([i]() { std::cout << "You pressed GB " << i << std::endl; });
                addAndMakeVisible(*g);
                gb.push_back(std::move(g));

                auto m = std::make_unique<sst::jucegui::components::MenuButton>();
                m->setLabel("Menu " + std::to_string(i + 1));
                m->setOnCallback([i]() { std::cout << "You pressed MB " << i << std::endl; });
                addAndMakeVisible(*m);
                mb.push_back(std::move(m));
            }
        }
        void resized() override
        {
            auto r = juce::Rectangle<int>(0, 0, 40, 18).translated(3, 3);
            int kidx = 0;
            for (const auto &k : tpb)
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

            r = juce::Rectangle<int>(0, 0, 40, 18).translated(3, 70);
            kidx = 0;
            for (const auto &k : gb)
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
            r = juce::Rectangle<int>(0, 0, 40, 18).translated(3, 140);
            kidx = 0;
            for (const auto &k : mb)
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
        std::vector<std::unique_ptr<sst::jucegui::components::TextPushButton>> tpb;
        std::vector<std::unique_ptr<sst::jucegui::components::GlyphButton>> gb;
        std::vector<std::unique_ptr<sst::jucegui::components::MenuButton>> mb;
        std::vector<std::unique_ptr<sst::jucegui::data::BinaryDiscrete>> sources;
    };

    TextPushButtonDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Push Buttons");
        panelOne->setContentAreaComponent(std::make_unique<SomeTPB>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_TOGGLEDEMO_H
