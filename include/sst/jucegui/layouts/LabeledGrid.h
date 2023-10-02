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
        bool hasAssociatedLabel{false};
        juce::Component &comp;
    };

    LabeledGrid() { std::fill(colSpan.begin(), colSpan.end(), 1); }

    int32_t cellWidth{-1}, cellHeight{-1};
    void setControlCellSize(int cw, int ch)
    {
        cellWidth = cw;
        cellHeight = ch;
    }

    float colGapSize{14};
    std::array<int32_t, nCols> colGapsAfter{};
    void addColGapAfter(int col) { addColGapAfter(col, colGapSize); }
    void addColGapAfter(int col, float cgs) { colGapsAfter[col] = cgs; }

    std::array<int32_t, nCols> colSpan;
    void setColspanAt(int col, int span) { colSpan[col] = span; }

    float labelHeight{18};
    void setLabelHeight(float lh) { labelHeight = lh; }

    GridComp *addComponent(juce::Component &comp, int x, int y)
    {
        auto g = std::make_unique<GridComp>(comp);
        g->x = x;
        g->y = y;

        for (auto &gl : gridLabels)
            if (gl->x == x && gl->y == y)
                g->hasAssociatedLabel = true;

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
        if (str.empty())
            return nullptr;

        auto lb = std::make_unique<sst::jucegui::components::Label>();
        lb->setText(str);
        addLabel(*lb, x, y);

        for (auto &gc : gridComps)
            if (gc->x == x && gc->y == y && !str.empty())
                gc->hasAssociatedLabel = true;

        return lb;
    }

    void resize(const juce::Rectangle<int> &into)
    {
        // OK lets get some sizing
        auto ctW = cellWidth;
        auto ctH = cellHeight;

        if (cellWidth == -1 || cellHeight == -1)
        {
            auto bxh = 1.f * (into.getHeight() - nRows * labelHeight) / nRows;
            auto bxw = 1.f * (into.getWidth() - colGapsAfter.size() * colGapSize) / nCols;
            auto bxs = std::min(bxw, bxh);

            ctW = bxs;
            ctH = bxs;
        }

        auto box0 = into.withHeight(ctH).withWidth(ctW);
        auto boxLab0 = box0.translated(0, ctH).withHeight(labelHeight);
        for (const auto &g : gridComps)
        {
            auto xtran = g->x * ctW;
            int idx{0};
            for (const auto &c : colGapsAfter)
            {
                if (g->x > idx)
                    xtran += c;
                idx++;
            }
            auto cbx = box0.translated(xtran + g->xPush, g->y * (ctH + labelHeight) + g->yPush);

            if (!g->hasAssociatedLabel)
            {
                cbx = cbx.withHeight(cbx.getHeight() + labelHeight);
            }
            cbx = cbx.withWidth(cbx.getWidth() * colSpan[g->x]);

            g->comp.setBounds(cbx.reduced(g->reduction));
        }

        for (const auto &g : gridLabels)
        {
            auto xtran = g->x * ctW;
            int idx{0};
            for (const auto &c : colGapsAfter)
            {
                if (g->x > idx)
                    xtran += c;
                idx++;
            }
            auto cbx = boxLab0.translated(xtran, g->y * (ctH + labelHeight));
            cbx = cbx.withWidth(cbx.getWidth() * colSpan[g->x]);

            g->comp.setBounds(cbx);
        }
    }

    std::vector<std::unique_ptr<GridComp>> gridComps, gridLabels;
};
} // namespace sst::jucegui::layouts

#endif // CONDUIT_LABELEDGRID_H
