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

#include <unordered_set>
#include "sst/jucegui/components/ListView.h"

namespace sst::jucegui::components
{
struct ListView::Innards : juce::Component
{
    std::vector<std::unique_ptr<juce::Component>> components;
    std::unordered_set<uint32_t> selectedRows;
    int32_t contiguousStart{-1};

    void pruneSelectionsAfterRefresh()
    {
        auto numrow = components.size();
        auto it = selectedRows.begin();
        while (it != selectedRows.end())
        {
            if (*it > numrow)
            {
                it = selectedRows.erase(it);
            }
            else
            {
                it++;
            }
        }
        contiguousStart = -1;
    }
};
ListView::ListView(const juce::String &cn) : StyleConsumer(Styles::styleClass)
{
    viewPort = std::make_unique<Viewport>(cn);

    innards = std::make_unique<Innards>();
    viewPort->setViewedComponent(innards.get(), false);

    addAndMakeVisible(*viewPort);
}
ListView::~ListView() {}
void ListView::refresh()
{
    if (!getRowCount || !getRowHeight || !makeRowComponent || !assignComponentToRow)
        return;

    auto rh = getRowHeight();
    auto rc = getRowCount();
    auto ht = rc * rh;

    innards->setBounds(0, 0, getWidth() - viewPort->getScrollBarThickness(), ht);

    // Brute force strategy
    innards->removeAllChildren();
    auto ics = innards->components.size();
    if (ics < rc)
    {
        for (int i = innards->components.size(); i < rc; ++i)
        {
            auto c = makeRowComponent();
            innards->components.emplace_back(std::move(c));
        }
    }
    else if (ics > rc)
    {
        auto b = innards->components.begin();
        innards->components.erase(b + rc, innards->components.end());
    }

    uint32_t idx{0};
    for (const auto &c : innards->components)
    {
        assignComponentToRow(c, idx);
        innards->addAndMakeVisible(*c);
        c->setBounds(0, idx * rh, innards->getWidth(), rh);
        idx++;
    }

    innards->pruneSelectionsAfterRefresh();

    repaint();
}

void ListView::rowSelected(uint32_t r, bool b, const juce::ModifierKeys &mods)
{
    if (mods.isShiftDown())
        rowSelected(r, b, SelectionAddAction::ADD_CONTIGUOUS);
    else if (mods.isCommandDown() || mods.isCtrlDown())
        rowSelected(r, b, SelectionAddAction::ADD_NON_CONTIGUOUS);
    else
        rowSelected(r, b, SelectionAddAction::SINGLE);
}
void ListView::rowSelected(uint32_t r, bool b, SelectionAddAction addMode)
{
    if (selectionMode == SINGLE_SELECTION ||
        (selectionMode == MULTI_SELECTION && addMode == SINGLE))
    {
        assert(setRowSelection);
        if (b)
        {
            for (auto &rs : innards->selectedRows)
            {
                if (rs != r)
                {
                    setRowSelection(innards->components[rs], false);
                }
            }
            innards->selectedRows.clear();
            innards->selectedRows.insert(r);
            setRowSelection(innards->components[r], b);
        }
        else
        {
            innards->selectedRows.clear();
            setRowSelection(innards->components[r], b);
        }
        if (b)
            innards->contiguousStart = r;
    }
    else if (addMode == ADD_CONTIGUOUS)
    {
        if (innards->contiguousStart < 0)
        {
            if (innards->selectedRows.empty())
            {
                innards->contiguousStart = r;
            }
            else
            {
                innards->contiguousStart = *innards->selectedRows.begin();
            }
        }
        auto start = (uint32_t)innards->contiguousStart;
        auto end = r;
        if (start > end)
            std::swap(start, end);

        auto it = innards->selectedRows.begin();
        while (it != innards->selectedRows.end())
        {
            auto sr = *it;
            if (sr < start || sr > end)
            {
                setRowSelection(innards->components[sr], false);
                it = innards->selectedRows.erase(it);
            }
            else
            {
                it++;
            }
        }
        for (int i = start; i <= end; ++i)
        {
            if (innards->selectedRows.find(i) == innards->selectedRows.end())
            {
                setRowSelection(innards->components[i], true);
                innards->selectedRows.insert(i);
            }
        }
    }
    else if (addMode == ADD_NON_CONTIGUOUS)
    {
        if (b)
        {
            setRowSelection(innards->components[r], true);
            innards->selectedRows.insert(r);
        }
        else
        {
            setRowSelection(innards->components[r], true);
            innards->selectedRows.erase(r);
        }
    }
}

void ListView::setSelectionMode(SelectionMode s)
{
    selectionMode = s;
    for (auto &rs : innards->selectedRows)
    {
        setRowSelection(innards->components[rs], false);
    }
    innards->selectedRows.clear();
    // TODO - cleanup selections
}

ListView::SelectionAddAction ListView::selectionAddActionForModifier(const juce::ModifierKeys &mods)
{
    if (mods.isShiftDown())
        return ADD_CONTIGUOUS;
    if (mods.isCommandDown())
        return ADD_NON_CONTIGUOUS;
    return SINGLE;
}

} // namespace sst::jucegui::components