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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_LISTVIEW_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_LISTVIEW_H

#include <memory>
#include <string>
#include <optional>
#include <juce_gui_basics/juce_gui_basics.h>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

#include "BaseStyles.h"
#include "Viewport.h"

namespace sst::jucegui::components
{
struct ListView : public juce::Component,
                  public style::StyleConsumer,
                  public style::SettingsConsumer
{
    struct Styles : base_styles::Base
    {
        SCLASS(listview);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(base_styles::Base::styleClass);
        }
    };

    enum ComponentStrategy
    {
        BRUTE_FORCE, // just make a component per row.
        BRUTE_FORCE_NO_REUSE
    } strategy{BRUTE_FORCE};

    enum SelectionMode
    {
        NO_SELECTION,
        SINGLE_SELECTION,
        MULTI_SELECTION
    } selectionMode{SINGLE_SELECTION};

    ListView(const juce::String &cn = juce::String());
    ~ListView();

    void refresh();

    void resized() override
    {
        viewPort->setBounds(getLocalBounds());
        refresh();
    }

    void setSelectionMode(SelectionMode s);

    enum SelectionAddAction
    {
        SINGLE,
        ADD_NON_CONTIGUOUS,
        ADD_CONTIGUOUS
    };
    void rowSelected(uint32_t r, bool select, SelectionAddAction addMode = SINGLE);
    static SelectionAddAction selectionAddActionForModifier(const juce::ModifierKeys &);

    std::function<uint32_t()> getRowCount{nullptr};
    std::function<uint32_t()> getRowHeight{nullptr};
    std::function<std::unique_ptr<juce::Component>()> makeRowComponent{nullptr};
    std::function<void(const std::unique_ptr<juce::Component> &, uint32_t)> assignComponentToRow{
        nullptr};
    std::function<void(const std::unique_ptr<juce::Component> &, bool)> setRowSelection{nullptr};

    std::unique_ptr<Viewport> viewPort;

    struct Innards;
    std::unique_ptr<Innards> innards;
};
} // namespace sst::jucegui::components

#endif // LISTVIEW_H
