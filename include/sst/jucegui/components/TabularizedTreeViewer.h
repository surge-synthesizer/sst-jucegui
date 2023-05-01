/*
 * sst-juce-guil - an open source library of juce widgets
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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TABULARIZEDTREEVIEWER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TABULARIZEDTREEVIEWER_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/TreeTable.h>
#include <sst/jucegui/components/BaseStyles.h>

#include <string>

#include "ComponentBase.h"

namespace sst::jucegui::components
{
struct TabularizedTreeViewer : public juce::Component,
                               public style::StyleConsumer,
                               public style::SettingsConsumer,
                               public EditableComponentBase<TabularizedTreeViewer>,
                               public data::TabularizedTreeView::Listener
{
    TabularizedTreeViewer();
    ~TabularizedTreeViewer() = default;

    struct Styles : ControlStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"tabluarizedtreeviewer"};

        static constexpr sprop toggleboxcol{"togglebox.color"};
        static constexpr sprop toggleglyphcol{"toggleglyph.color"};
        static constexpr sprop toggleglyphhovercol{"toggleglyphhover.color"};

        static constexpr sprop connectorcol{"connector.color"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(ControlStyles::styleClass)
                .withProperty(toggleboxcol)
                .withProperty(toggleglyphcol)
                .withProperty(toggleglyphhovercol)
                .withProperty(connectorcol);
        }
    };
    // TODO set up listeners

    void paint(juce::Graphics &g) override;
    void setSource(data::TabularizedTreeView *d)
    {
        data = d;
        recalcRowDisplayCache();
        repaint();
    }

    void mouseMove(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;

  private:
    struct RowDisplayData
    {
        juce::Rectangle<int> openCloseZone;
        data::TabularizedTreeView::TabularizedRow::DisplayType type;
        uint32_t row;

        // If this becomes projibitive replace it with a bitmask
        std::vector<bool> showLine;
    };

    void recalcRowDisplayCache();
    std::vector<RowDisplayData> rowDisplayCache;
    int hoveredOpenCloseZone{-1};
    void rowClick(uint32_t row);

    static constexpr int rowHeight = 18, rowIndent = 20, hotzoneSize = 16;

    data::TabularizedTreeView *data{nullptr};
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_TABULARIZEDTREEVIEWER_H
