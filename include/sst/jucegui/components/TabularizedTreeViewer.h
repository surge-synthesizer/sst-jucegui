//
// Created by Paul Walker on 8/2/22.
//

#ifndef SST_JUCEGUI_TABULARIZEDTREEVIEWER_H
#define SST_JUCEGUI_TABULARIZEDTREEVIEWER_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/TreeTable.h>

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

    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"tabluarizedtreeviewer"};

        static constexpr sprop toggleboxcol{"togglebox.color"};
        static constexpr sprop toggleglyphcol{"toggleglyph.color"};
        static constexpr sprop toggleglyphhovercol{"toggleglyphhover.color"};

        static constexpr sprop connectorcol{"connector.color"};

        static constexpr sprop labelcol{"label.color"};

        static constexpr sprop labelfont{"label.font", sprop::FONT};
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
