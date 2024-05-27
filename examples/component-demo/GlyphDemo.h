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

#ifndef SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_GLYPHDEMO_H
#define SSTJUCEGUI_EXAMPLES_COMPONENT_DEMO_GLYPHDEMO_H

#include <sst/jucegui/components/Knob.h>
#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/WindowPanel.h>
#include <sst/jucegui/util/DebugHelpers.h>
#include "ExampleUtils.h"

#include "sst/jucegui/components/GlyphPainter.h"

struct GlyphDemo : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "Glpyhs";

    struct AllGlyphs : juce::Component
    {
        juce::Colour bg{juce::Colours::black};
        void paint(juce::Graphics &g) override
        {
            g.fillAll(bg);
            g.setColour(bg.contrasting());
            g.drawRect(getLocalBounds(), 1);

            int w{30};
            auto r = juce::Rectangle<int>(5, 5, w, w);
            for (const auto &col :
                 {juce::Colours::white, juce::Colours::orange, juce::Colours::red})
            {
                for (int i = sst::jucegui::components::GlyphPainter::PAN;
                     i <= sst::jucegui::components::GlyphPainter::STEREO; ++i)
                {
                    sst::jucegui::components::GlyphPainter::paintGlyph(
                        g, r, (sst::jucegui::components::GlyphPainter::GlyphType)i, col);
                    g.setColour(juce::Colours::darkgrey);
                    g.drawRect(r);

                    r = r.translated(w + 4, 0);
                    if (!getLocalBounds().contains(r))
                    {
                        r = r.withX(5).translated(0, w + 4);
                    }
                }
            }
        }
    };

    GlyphDemo()
    {
        panelOne = std::make_unique<sst::jucegui::components::NamedPanel>("Modulatable Knobs");
        panelOne->setContentAreaComponent(std::make_unique<AllGlyphs>());

        addAndMakeVisible(*panelOne);
    }

    void resized() override { panelOne->setBounds(getLocalBounds().reduced(10)); }

    std::unique_ptr<sst::jucegui::components::NamedPanel> panelOne;
};

#endif // SST_JUCEGUI_GLYPHDEMO_H
