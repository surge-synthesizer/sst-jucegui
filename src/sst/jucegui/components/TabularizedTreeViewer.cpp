//
// Created by Paul Walker on 8/2/22.
//

#include <sst/jucegui/components/TabularizedTreeViewer.h>

namespace sst::jucegui::components
{

TabularizedTreeViewer::TabularizedTreeViewer() : style::StyleConsumer(Styles::styleClass) {}
void TabularizedTreeViewer::paint(juce::Graphics &g)
{
    if (!data)
        return;

    auto dr = getLocalBounds().withHeight(rowHeight);
    for (uint32_t i = 0; i < data->getRowCount(); ++i)
    {
        const auto &row = data->getRow(i);
        auto qr = dr;
        qr = qr.withTrimmedLeft(row.depth * rowIndent);

        g.setColour(juce::Colours::black);
        qr = qr.withTrimmedLeft(hotzoneSize + 4);
        g.setFont(getFont(Styles::labelfont));
        g.setColour(getColour(Styles::labelcol));
        g.drawText(row.label, qr, juce::Justification::centredLeft);

        g.setColour(getColour(Styles::connectorcol));
        if (row.depth > 0)
        {
            uint32_t subsequentDepth =
                (i == data->getRowCount() - 1 ? 0 : data->getRow(i + 1).depth);
            auto rr = dr.withTrimmedLeft((row.depth - 1) * rowIndent).withWidth(rowIndent);
            if (subsequentDepth == row.depth)
            {
                auto vrr = rr.withTrimmedLeft(rr.getWidth() / 2).withWidth(1);
                g.fillRect(vrr);
                auto hrr = rr.withTrimmedTop(rr.getHeight() / 2)
                               .withHeight(1)
                               .withTrimmedLeft(rr.getWidth() / 2);
                g.fillRect(hrr);
            }
            else
            {
                auto vrr = rr.withTrimmedLeft(rr.getWidth() / 2)
                               .withWidth(1)
                               .withHeight(rr.getHeight() / 2);
                g.fillRect(vrr);
                auto hrr = rr.withTrimmedTop(rr.getHeight() / 2)
                               .withHeight(1)
                               .withTrimmedLeft(rr.getWidth() / 2);
                g.fillRect(hrr);
            }
        }

        g.setColour(juce::Colours::black);
        if (rowDisplayCache[i].type != data::TabularizedTreeView::TabularizedRow::NODE)
        {
            g.setColour(getColour(Styles::toggleboxcol));
            g.drawRect(rowDisplayCache[i].openCloseZone);

            if (i == hoveredOpenCloseZone)
            {
                g.setColour(getColour(Styles::toggleglyphhovercol));
            }
            else
            {
                g.setColour(getColour(Styles::toggleglyphcol));
            }
            if (rowDisplayCache[i].type == data::TabularizedTreeView::TabularizedRow::OPEN)
            {
                // I am open so draw a minus
                auto q = rowDisplayCache[i].openCloseZone;
                q = q.withTrimmedTop(q.getHeight() / 2 - 1)
                        .withTrimmedLeft(4)
                        .withTrimmedRight(4)
                        .withHeight(2);
                g.fillRect(q);
            }
            else
            {
                auto q = rowDisplayCache[i].openCloseZone;
                q = q.withTrimmedTop(q.getHeight() / 2 - 1)
                        .withTrimmedLeft(4)
                        .withTrimmedRight(4)
                        .withHeight(2);
                g.fillRect(q);
                q = rowDisplayCache[i].openCloseZone;
                q = q.withTrimmedLeft(q.getWidth() / 2 - 1)
                        .withTrimmedTop(4)
                        .withTrimmedBottom(4)
                        .withWidth(2);
                g.fillRect(q);
            }
        }
        else
        {
            auto rr = dr.withTrimmedLeft(row.depth * rowIndent)
                          .withWidth(hotzoneSize)
                          .withTrimmedTop(dr.getHeight() / 2)
                          .withHeight(1);
            g.fillRect(rr);
        }

        dr = dr.translated(0, rowHeight);
    }
}

void TabularizedTreeViewer::recalcRowDisplayCache()
{
    rowDisplayCache.clear();
    auto bhz = juce::Rectangle<int>().withHeight(hotzoneSize).withWidth(hotzoneSize);
    for (uint32_t i = 0; i < data->getRowCount(); ++i)
    {
        const auto &row = data->getRow(i);
        auto rr = bhz.translated(row.depth * rowIndent + 2,
                                 i * rowHeight + (rowHeight - hotzoneSize) / 2);
        auto newRow = RowDisplayData{rr, row.type, i};
        rowDisplayCache.push_back(newRow);
    }
}

void TabularizedTreeViewer::rowClick(uint32_t row) {}

void TabularizedTreeViewer::mouseMove(const juce::MouseEvent &e)
{
    int ohz = hoveredOpenCloseZone;
    hoveredOpenCloseZone = -1;
    for (const auto &z : rowDisplayCache)
    {
        if (z.openCloseZone.toFloat().contains(e.position))
        {
            hoveredOpenCloseZone = z.row;
        }
    }

    if (hoveredOpenCloseZone != ohz)
        repaint();
}

void TabularizedTreeViewer::mouseUp(const juce::MouseEvent &e)
{
    bool doRecalc = false;
    for (const auto &z : rowDisplayCache)
    {
        if (z.type != data::TabularizedTreeView::TabularizedRow::NODE)
        {
            if (z.openCloseZone.toFloat().contains(e.position))
            {
                if (z.type == data::TabularizedTreeView::TabularizedRow::OPEN)
                {
                    data->close(z.row);
                }
                else
                {
                    data->open(z.row);
                }
                doRecalc = true;
                break;
            }
        }
    }

    if (doRecalc)
    {
        recalcRowDisplayCache();
        repaint();
    }
}
} // namespace sst::jucegui::components