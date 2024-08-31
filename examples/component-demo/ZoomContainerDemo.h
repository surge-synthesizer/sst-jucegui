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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_ZOOMCONTAINERDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_ZOOMCONTAINERDEMO_H

#include <sst/jucegui/components/ZoomContainer.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/util/DebugHelpers.h>
#include "ExampleUtils.h"
#include <cassert>

struct ZoomContainerDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "ZoomContainer";

    template <bool DO_H, bool DO_V>
    struct PaintGrid : juce::Component, sst::jucegui::components::ZoomContainerClient
    {
        juce::Component *associatedComponent() override { return this; }
        bool supportsVerticalZoom() const override { return DO_V; }
        bool supportsHorizontalZoom() const override { return DO_H; }
        float hZoomStart{0.f}, hZoomScale{1.f};
        float vZoomStart{0.f}, vZoomScale{1.f};
        void setHorizontalZoom(float pctStart, float zoomFactor) override
        {
            assert(DO_H);
            hZoomStart = pctStart;
            hZoomScale = zoomFactor;
            repaint();
        }
        void setVerticalZoom(float pctStart, float zoomFactor) override
        {
            assert(DO_V);
            vZoomStart = pctStart;
            vZoomScale = zoomFactor;
            repaint();
        }

        void paint(juce::Graphics &g) override
        {
            g.fillAll(juce::Colour(40, 40, 70));

            if (DO_H)
            {
                // q = (i-zoomStart) * scale
                // q == 0 ->  i = zoomStart
                // q = 1 -> i = 1/scale + zoomStart
                // but q is stepping in 0.25 so if we start at q=0 we
                // quantize. So we want to start at the 0.025 below
                // zoomstart
                auto start = std::floor(hZoomStart * 20) / 20;
                auto end = 1.0 / hZoomScale + hZoomStart + 0.025;
                bool first{true};
                for (float i = start; i < end; i += 0.025)
                {
                    auto q = (i - hZoomStart) * hZoomScale;
                    g.setColour(juce::Colours::red);
                    g.drawVerticalLine(q * getWidth(), 0, getHeight());
                    g.setFont(12);
                    g.setColour(juce::Colours::white);
                    g.drawText(std::to_string((int)(i / 0.025)), q * getWidth(), 0, 60, 20,
                               juce::Justification::centredLeft);
                }
            }

            if (DO_V)
            {
                // q = (i-zoomStart) * scale
                // q == 0 ->  i = zoomStart
                // q = 1 -> i = 1/scale + zoomStart
                // but q is stepping in 0.25 so if we start at q=0 we
                // quantize. So we want to start at the 0.025 below
                // zoomstart
                auto start = std::floor(vZoomStart * 20) / 20;
                auto end = 1.0 / vZoomScale + vZoomStart + 0.025;
                bool first{true};
                for (float i = start; i < end; i += 0.025)
                {
                    auto q = (i - vZoomStart) * vZoomScale;
                    g.setColour(juce::Colours::green);
                    g.drawHorizontalLine(q * getHeight(), 0, getWidth());
                    g.setFont(12);
                    g.setColour(juce::Colours::white);
                    g.drawText(std::to_string((int)(i / 0.025)), 0, q * getHeight(), 60, 20,
                               juce::Justification::centredLeft);
                }
            }
            g.setColour(juce::Colours::yellow);
            g.drawRect(getLocalBounds());
        }
    };

    ZoomContainerDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("ZoomThis");
        panelContents = std::make_unique<sst::jucegui::components::ZoomContainer>(
            std::make_unique<PaintGrid<true, false>>());
        lowerPanelContents = std::make_unique<sst::jucegui::components::ZoomContainer>(
            std::make_unique<PaintGrid<true, true>>());
        addAndMakeVisible(*panelOne);
        panelOne->addAndMakeVisible(*panelContents);
        panelOne->addAndMakeVisible(*lowerPanelContents);
    }

    void resized() override
    {
        panelOne->setBounds(getLocalBounds().reduced(10));
        auto plb = panelOne->getLocalBounds().reduced(20);

        panelContents->setBounds(plb.withHeight(plb.getHeight() / 2).reduced(10));
        lowerPanelContents->setBounds(
            plb.withHeight(plb.getHeight() / 2).translated(0, plb.getHeight() / 2).reduced(10));
    }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
    std::unique_ptr<sst::jucegui::components::ZoomContainer> panelContents;
    std::unique_ptr<sst::jucegui::components::ZoomContainer> lowerPanelContents;
};

#endif // SST_JUCEGUI_ZOOMCONTAINER_H
