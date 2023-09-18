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

#ifndef INCLUDE_SST_JUCEGUI_LAYOUTS_LABELEDGRID_H
#define INCLUDE_SST_JUCEGUI_LAYOUTS_LABELEDGRID_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/components/Label.h>

namespace sst::jucegui::layouts
{
template <int nCols, int nRows> struct LabeledGrid
{
    struct GridComp
    {
        GridComp(juce::Component &c) : comp(c) {}
        int x{0}, y{0};
        int xPush{0}, yPush{0};
        int rowSpan{0}, colspan{0};
        int reduction{2};
        juce::Component &comp;
    };

    float colGapSize{14};
    std::vector<int32_t> colGapsAfter;
    void addColGapAfter(int col) { colGapsAfter.push_back(col); }

    float labelHeight{18};
    void setLabelHeight(float lh) { labelHeight = lh; }

    GridComp *addComponent(juce::Component &comp, int x, int y)
    {
        auto g = std::make_unique<GridComp>(comp);
        g->x = x;
        g->y = y;
        gridComps.push_back(std::move(g));
        return gridComps.back().get();
    }

    GridComp *addLabel(juce::Component &comp, int x, int y)
    {
        auto g = std::make_unique<GridComp>(comp);
        g->x = x;
        g->y = y;
        gridLabels.push_back(std::move(g));
        return gridLabels.back().get();
    }

    std::unique_ptr<sst::jucegui::components::Label> addLabel(const std::string &str, int x, int y)
    {
        auto lb = std::make_unique<sst::jucegui::components::Label>();
        lb->setText(str);
        addLabel(*lb, x, y);
        return lb;
    }

    void resize(const juce::Rectangle<int> &into)
    {
        // OK lets get some sizing
        auto bxh = 1.f * (into.getHeight() - nRows * labelHeight) / nRows;
        auto bxw = 1.f * (into.getWidth() - colGapsAfter.size() * colGapSize) / nCols;
        auto bxs = std::min(bxw, bxh);

        auto box0 = into.withHeight(bxs).withWidth(bxs);
        auto boxLab0 = box0.translated(0, bxs).withHeight(labelHeight);
        for (const auto &g : gridComps)
        {
            auto xtran = g->x * bxs;
            for (const auto &c : colGapsAfter)
            {
                if (g->x > c)
                    xtran += colGapSize;
            }
            auto cbx = box0.translated(xtran + g->xPush, g->y * (bxs + labelHeight) + g->yPush);
            g->comp.setBounds(cbx.reduced(g->reduction));
        }

        for (const auto &g : gridLabels)
        {
            auto xtran = g->x * bxs;
            for (const auto &c : colGapsAfter)
            {
                if (g->x > c)
                    xtran += colGapSize;
            }
            auto cbx = boxLab0.translated(xtran, g->y * (bxs + labelHeight));
            g->comp.setBounds(cbx);
        }
    }

    std::vector<std::unique_ptr<GridComp>> gridComps, gridLabels;
};
} // namespace sst::jucegui::layouts

#endif // CONDUIT_LABELEDGRID_H
