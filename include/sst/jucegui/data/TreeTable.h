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

#ifndef INCLUDE_SST_JUCEGUI_DATA_TREETABLE_H
#define INCLUDE_SST_JUCEGUI_DATA_TREETABLE_H

#include <string>
#include <vector>
#include <memory>
#include <cassert>

namespace sst::jucegui::data
{
/*
 * A TreeTable has parent-child relationships between nodes
 */
struct TreeTableData
{
    virtual ~TreeTableData() = default;
    struct Entry
    {
        virtual ~Entry() = default;
        uint64_t id;

        virtual bool hasChildren() const = 0;
        virtual uint32_t getChildCount() const = 0;

        virtual const std::unique_ptr<Entry> &getChildAt(uint32_t idx) = 0;
        virtual std::string getLabel() const = 0;
        // virtual T getDataForColumn(int i) const = 0;
    };

    virtual const std::unique_ptr<Entry> &getRoot() const = 0;
};

struct TabularizedTreeView
{
    virtual ~TabularizedTreeView() = default;
    struct Listener
    {
    };

    struct TabularizedRow
    {
        enum DisplayType
        {
            NODE,
            OPEN,
            CLOSED
        } type;
        std::string label;
        uint32_t depth;

        typedef std::vector<uint32_t> path_t;
        path_t path;
    };

    virtual uint32_t getRowCount() const = 0;
    virtual const TabularizedRow &getRow(uint32_t r) const = 0;
    virtual void open(int displayRow) = 0;
    virtual void close(int displayRow) = 0;
};

struct ConcreteTabularizedViewOfTree : public TabularizedTreeView
{
    const TreeTableData &data;
    ConcreteTabularizedViewOfTree(const TreeTableData &d);
    virtual ~ConcreteTabularizedViewOfTree() = default;

    uint32_t getRowCount() const override;

    const TabularizedRow &getRow(uint32_t r) const override
    {
        assert(r >= 0 && r < rows.size());
        return rows[r];
    }

    void open(int displayRow) override;
    void close(int displayRow) override;

  private:
    typedef std::vector<TabularizedRow> rows_t;
    rows_t rows;
};

} // namespace sst::jucegui::data
#endif // SST_JUCEGUI_DATA_TREETABLE_H
