//
// Created by Paul Walker on 8/1/22.
//

#include <sst/jucegui/data/TreeTable.h>

namespace sst::jucegui::data
{
ConcreteTabularizedViewOfTree::ConcreteTabularizedViewOfTree(const TreeTableData &d) : data(d)
{
    auto tr = TabularizedRow();
    if (d.getRoot()->hasChildren())
    {
        tr.type = TabularizedRow::CLOSED;
    }
    else
    {
        tr.type = TabularizedRow::NODE;
    }
    tr.label = d.getRoot()->getLabel();
    tr.path = {};
    tr.depth = 0;

    rows.push_back(tr);
}

uint32_t ConcreteTabularizedViewOfTree::getRowCount() const { return rows.size(); }

void ConcreteTabularizedViewOfTree::open(int r)
{
    assert(r >= 0 && r < rows.size());
    assert(rows[r].type == TabularizedRow::CLOSED);

    const auto &row = rows[r];

    // This is pretty inefficient but
    auto newRows = rows_t();
    for (int i = 0; i <= r; ++i)
        newRows.push_back(rows[i]);

    newRows[r].type = TabularizedRow::OPEN;

    // Now do the open. This is rather clumsy
    auto e = data.getRoot().get();
    for (const auto &idx : row.path)
    {
        e = e->getChildAt(idx).get();
    }
    auto p = row.path;
    for (int i = 0; i < e->getChildCount(); ++i)
    {
        const auto &q = e->getChildAt(i);
        auto tr = TabularizedRow();
        tr.label = q->getLabel();
        tr.type = q->hasChildren() ? TabularizedRow::CLOSED : TabularizedRow::NODE;
        tr.depth = row.depth + 1;
        tr.path = p;
        tr.path.push_back(i);

        newRows.push_back(tr);
    }

    for (int i = r + 1; i < rows.size(); ++i)
        newRows.push_back(rows[i]);

    rows = newRows;
}

void ConcreteTabularizedViewOfTree::close(int r)
{
    assert(r >= 0 && r < rows.size());
    assert(rows[r].type == TabularizedRow::OPEN);

    auto newRows = rows_t();

    int idx = 0;
    while (idx <= r)
    {
        newRows.push_back(rows[idx]);
        idx++;
    }
    auto d = newRows.back().depth;
    newRows.back().type = TabularizedRow::CLOSED;

    while (d < rows[idx].depth && idx < rows.size())
    {
        idx++;
    }

    while (idx < rows.size())
    {
        newRows.push_back(rows[idx]);
        idx++;
    }

    rows = newRows;
}

} // namespace sst::jucegui::data