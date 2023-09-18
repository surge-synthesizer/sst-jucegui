/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023, various authors, as described in the GitHub
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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_SEVENSEGMENTPAINTER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_SEVENSEGMENTPAINTER_H

#include <cstdint>
#include <utility>

namespace sst::jucegui::components
{
struct SevenSegmentPainter
{
    static constexpr int elementsByNum[16][7] = {
        {1, 1, 1, 1, 1, 1, 0}, // 0
        {0, 1, 1, 0, 0, 0, 0}, // 1
        {1, 1, 0, 1, 1, 0, 1}, // 2
        {1, 1, 1, 1, 0, 0, 1}, // 3
        {0, 1, 1, 0, 0, 1, 1}, // 4
        {1, 0, 1, 1, 0, 1, 1}, // 5
        {1, 0, 1, 1, 1, 1, 1}, // 6
        {1, 1, 1, 0, 0, 0, 0}, // 7
        {1, 1, 1, 1, 1, 1, 1}, // 8
        {1, 1, 1, 1, 0, 1, 1}, // 9

        {1, 1, 1, 0, 1, 1, 1}, // A
        {0, 0, 1, 1, 1, 1, 1}, // b
        {1, 0, 0, 1, 1, 1, 0}, // C
        {0, 1, 1, 1, 1, 0, 1}, // d
        {1, 0, 0, 1, 1, 1, 1}, // E
        {1, 0, 0, 0, 1, 1, 1}  // F
    };

    using pt = std::pair<uint32_t, uint32_t>;
    using rct = std::pair<pt, pt>;
    static constexpr rct locations[7] = {{{2, 1}, {3, 1}}, {{5, 2}, {1, 3}}, {{5, 6}, {1, 3}},
                                         {{2, 9}, {3, 1}}, {{1, 6}, {1, 3}}, {{1, 2}, {1, 3}},
                                         {{2, 5}, {3, 1}}};

    // height and width in logical units
    static constexpr int maxHeight{10}, maxWidth{6};
    static constexpr float aspectRatio{1.f * maxWidth / maxHeight};

    static void paintInto(juce::Graphics &g, const juce::Rectangle<float> &rect,
                          const juce::Colour &col, const juce::Colour &offCol, int digit)
    {
        auto &ebn = elementsByNum[digit];

        juce::Graphics::ScopedSaveState gs(g);
        g.addTransform(juce::AffineTransform().translated(rect.getX(), rect.getY()));

        auto pplw{rect.getWidth() * 1.f / maxWidth};
        auto pplh{rect.getHeight() * 1.f / maxHeight};
        auto ppl{std::min(pplw, pplh)};

        int idx{0};
        for (auto &[loc, sz] : locations)
        {
            auto [xv, yv] = loc;
            auto [w0, h0] = sz;

            float y = yv - 0.5;
            float x = xv - 0.5;
            int ew = w0;
            int eh = h0;

            // New version with corners
            float x0 = x * ppl + 1;
            float y0 = y * ppl + 1;
            float w = ew * ppl;
            float h = eh * ppl;
            float tri = ppl / 2;

            juce::Path p;

            if (eh == 1)
            {
                // This is a sideways element
                p.startNewSubPath(x0, y0);
                p.lineTo(x0 + w, y0);
                p.lineTo(x0 + w + tri, y0 + tri);
                p.lineTo(x0 + w, y0 + h);
                p.lineTo(x0, y0 + h);
                p.lineTo(x0 - tri, y0 + tri);
                p.closeSubPath();
            }
            else
            {
                p.startNewSubPath(x0, y0);
                p.lineTo(x0, y0 + h);
                p.lineTo(x0 + tri, y0 + h + tri);
                p.lineTo(x0 + w, y0 + h);
                p.lineTo(x0 + w, y0);
                p.lineTo(x0 + tri, y0 - tri);
                p.closeSubPath();
            }

            if (ebn[idx] > 0)
            {
                g.setColour(col);
                g.fillPath(p);
            }
            else
            {
                g.setColour(offCol);
                g.fillPath(p);
            }

            idx++;
        }
    }
};
} // namespace sst::jucegui::components
#endif // CONDUIT_SEVENSEGMENTPAINTER_H
