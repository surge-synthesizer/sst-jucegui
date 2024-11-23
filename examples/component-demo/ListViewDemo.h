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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_LISTVIEWDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_LISTVIEWDEMO_H

#include <string>
#include <sst/jucegui/components/ListView.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/components/TextPushButton.h>
#include "ExampleUtils.h"

struct ListViewDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "ListViews";

    struct AList : juce::Component
    {
        struct RowComp : juce::Component
        {
            AList *parent{nullptr};
            RowComp(AList *parent) : parent(parent) {}
            uint32_t row{0};
            bool selected{false};
            void setRow(uint32_t r)
            {
                row = r;
                repaint();
            }
            void paint(juce::Graphics &g) override
            {
                g.fillAll(juce::Colour((row * 17) % 255, 0, 120));
                if (selected)
                    g.setColour(juce::Colours::white);
                else
                    g.setColour(juce::Colours::green);
                g.setFont(12);
                g.drawText(std::to_string(row), getLocalBounds(), juce::Justification::centred);
                g.drawRect(getLocalBounds().reduced(1));
            }
            void mouseDown(const juce::MouseEvent &e) override
            {
                parent->listView->rowSelected(row, !selected,
                    sst::jucegui::components::ListView::selectionAddActionForModifier(e.mods));
            }
        };
        int rowCount{275}, rowHeight{18};
        AList()
        {
            listView = std::make_unique<sst::jucegui::components::ListView>();
            addAndMakeVisible(*listView);

            for (int i = 0; i < 3; ++i)
            {
                auto tb = std::make_unique<sst::jucegui::components::TextPushButton>();
                auto amt = (i == 0 ? 275 : i == 1 ? 11 : 750);
                tb->setLabel(std::to_string(amt) + " rows");
                tb->setOnCallback([amt, w = juce::Component::SafePointer(this)]() {
                    if (!w)
                        return;
                    w->rowCount = amt;
                    w->listView->refresh();
                });
                addAndMakeVisible(*tb);
                buttons[i] = std::move(tb);
            }

            for (int i = 3; i < 5; ++i)
            {
                auto tb = std::make_unique<sst::jucegui::components::TextPushButton>();
                auto amt = (i == 3 ? 18 : 67);
                tb->setLabel(std::to_string(amt) + " height");
                tb->setOnCallback([amt, w = juce::Component::SafePointer(this)]() {
                    if (!w)
                        return;
                    w->rowHeight = amt;
                    w->listView->refresh();
                });
                addAndMakeVisible(*tb);
                buttons[i] = std::move(tb);
            }

            for (int i = 5; i < 7; ++i)
            {
                auto tb = std::make_unique<sst::jucegui::components::TextPushButton>();
                auto amt = (i == 5 ? sst::jucegui::components::ListView::SelectionMode::SINGLE_SELECTION :
                    sst::jucegui::components::ListView::SelectionMode::MULTI_SELECTION);
                tb->setLabel(i== 5 ? "SingSel" : "MultSel");
                tb->setOnCallback([amt, w = juce::Component::SafePointer(this)]() {
                    if (!w)
                        return;
                    w->listView->setSelectionMode(amt);
                    w->listView->refresh();
                });
                addAndMakeVisible(*tb);
                buttons[i] = std::move(tb);
            }

            listView->getRowHeight = []() { return 18; };
            listView->getRowHeight = [w = juce::Component::SafePointer(this)]() {
                if (!w)
                    return 0;
                return w->rowHeight;
            };
            listView->getRowCount = [w = juce::Component::SafePointer(this)]() {
                if (!w)
                    return 0;
                return w->rowCount;
            };
            listView->makeRowComponent = [this]() { return std::make_unique<RowComp>(this); };
            listView->assignComponentToRow = [](const auto &rc, auto r) {
                auto rcomp = dynamic_cast<RowComp *>(rc.get());
                if (rcomp)
                {
                    rcomp->setRow(r);
                }
            };
            listView->setRowSelection = [](const auto &rc, auto r) {
                auto rcomp = dynamic_cast<RowComp *>(rc.get());
                if (rcomp)
                {
                    rcomp->selected = r;
                    rcomp->repaint();
                }
            };
            listView->refresh();
        }
        ~AList() {}
        void resized() override
        {
            listView->setBounds(getLocalBounds().withTrimmedTop(25));
            auto br = getLocalBounds().withHeight(22);
            auto bw = br.getWidth() / buttons.size();
            br = br.withWidth(bw);
            for (const auto &b : buttons)
            {
                if (b)
                {
                    b->setBounds(br.reduced(1));
                }
                br = br.translated(bw, 0);
            }
        }

        std::unique_ptr<sst::jucegui::components::ListView> listView;

        std::array<std::unique_ptr<sst::jucegui::components::TextPushButton>, 7> buttons;
    };

    ListViewDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("List View");
        panelOne->setContentAreaComponent(std::make_unique<AList>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_ListViewDemo_H
