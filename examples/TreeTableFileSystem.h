//
// Created by Paul Walker on 7/31/22.
//

#ifndef SST_JUCEGUI_TREETABLEFILESYSTEM_H
#define SST_JUCEGUI_TREETABLEFILESYSTEM_H

#include "sst/jucegui/util/DebugHelpers.h"
#include "sst/jucegui/data/TreeTable.h"
#include "sst/jucegui/components/TabularizedTreeViewer.h"
#include "sst/jucegui/components/NamedPanel.h"
#include <filesystem>

struct FSTreeDataEntry : public sst::jucegui::data::TreeTableData::Entry
{
    const std::filesystem::path &path;
    bool isDir{false};
    uint32_t childCount{0};
    std::vector<std::unique_ptr<Entry>> children;
    std::vector<std::filesystem::path> childPaths;

    FSTreeDataEntry(const std::filesystem::path &p) : path(p)
    {
        if (std::filesystem::is_directory(path))
        {
            isDir = true;

            for (auto const &dir_entry : std::filesystem::directory_iterator{path})
            {
                childPaths.push_back(dir_entry.path());
                childCount++;
            }
            children.resize(childCount);
        }
    }

    bool hasChildren() const override { return isDir; }

    void buildChildren()
    {
        assert(isDir);
        if (!children.empty())
            return;
    }

    uint32_t getChildCount() const override { return children.size(); }
    const std::unique_ptr<Entry> &getChildAt(uint32_t idx) override
    {
        if (!children[idx])
            children[idx] = std::make_unique<FSTreeDataEntry>(childPaths[idx]);
        return children[idx];
    }
    std::string getLabel() const override
    {
        auto res = path.filename().u8string();
        if (res.empty())
            return "/";
        return res;
    }
};

struct FSTreeData : public sst::jucegui::data::TreeTableData
{
    std::unique_ptr<Entry> root;
    FSTreeData()
    {
#if JUCE_WINDOWS
        root = std::make_unique<FSTreeDataEntry>(std::filesystem::path{"c:"});
#else
        root = std::make_unique<FSTreeDataEntry>(std::filesystem::path{"/Users/paul/dev"});
#endif
    }
    const std::unique_ptr<Entry> &getRoot() const override { return root; }
};

struct TreeTableFileSystem : public sst::jucegui::components::WindowPanel
{
    static constexpr const char *name = "TreeTableFileSystem";

    std::unique_ptr<sst::jucegui::components::NamedPanel> panel;
    std::unique_ptr<sst::jucegui::data::TreeTableData> treedata;
    std::unique_ptr<sst::jucegui::data::TabularizedTreeView> tablularizeddata;

    TreeTableFileSystem()
    {
        panel = std::make_unique<sst::jucegui::components::NamedPanel>("FileSystem View");

        treedata = std::make_unique<FSTreeData>();
        tablularizeddata =
            std::make_unique<sst::jucegui::data::ConcreteTabularizedViewOfTree>(*treedata);

        auto tt = std::make_unique<sst::jucegui::components::TabularizedTreeViewer>();
        tt->setSource(tablularizeddata.get());
        panel->setContentAreaComponent(std::move(tt));
        addAndMakeVisible(*panel);
        runExample();
    }

    void resized() override { panel->setBounds(getLocalBounds().reduced(10)); }

    void recurseTree(const std::unique_ptr<sst::jucegui::data::TreeTableData::Entry> &e, int depth,
                     int maxDepth)
    {
        auto pfx = std::string(depth * 3, '-') + "| ";
        DBGOUT(pfx << DBGVAL(e->getLabel()) << DBGVAL(e->hasChildren())
                   << DBGVAL(e->getChildCount()));
        if (depth < maxDepth && e->hasChildren())
        {
            for (uint32_t i = 0; i < e->getChildCount(); ++i)
            {
                const auto &k = e->getChildAt(i);
                if (k)
                    recurseTree(k, depth + 1, maxDepth);
            }
        }
    }

    void runExample()
    {
#if 0
        auto e = std::make_unique<FSTreeData>();

        auto dump = [](const sst::jucegui::data::ConcreteTabularizedViewOfTree &view) {
            DBGOUT("VIEW - ROWCOUNT = " << view.getRowCount());
            for (int i = 0; i < view.getRowCount(); ++i)
            {
                auto r = view.getRow(i);
                char nm = ' ';
                switch (r.type)
                {
                case sst::jucegui::data::ConcreteTabularizedViewOfTree::TabularizedRow::OPEN:
                    nm = '-';
                    break;
                case sst::jucegui::data::ConcreteTabularizedViewOfTree::TabularizedRow::CLOSED:
                    nm = '+';
                    break;
                default:
                    nm = ' ';
                    break;
                }
                DBGOUT(std::setw(5)
                       << i << " " << std::string(r.depth * 3, '-') << "| " << nm << r.label);
            }
        };

        auto v = sst::jucegui::data::ConcreteTabularizedViewOfTree(*e);
        dump(v);

        v.open(0);
        dump(v);

        v.open(2);
        dump(v);

        v.close(2);
        dump(v);
#endif
    }
};
#endif // SST_JUCEGUI_TREETABLEFILESYSTEM_H
