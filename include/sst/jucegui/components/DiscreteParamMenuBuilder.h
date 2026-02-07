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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_DISCRETEPARAMMENUBUILDER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_DISCRETEPARAMMENUBUILDER_H

#include <optional>
#include <vector>
#include <utility>

#include <juce_gui_basics/juce_gui_basics.h>
#include <sst/jucegui/data/Discrete.h>

namespace sst::jucegui::components
{
struct DiscreteParamEditor;

struct DiscreteParamMenuBuilder
{
    enum struct Mode : uint32_t
    {
        LINEAR, // flat list of items min to max
        GROUP_LIST,
    } mode{Mode::LINEAR};

    // if this is set the menu section header is this string, otherwise its the data label
    std::optional<std::string> menuHeader{std::nullopt};

    data::Discrete *data{nullptr};
    void setData(data::Discrete *d) { data = d; };

    std::function<juce::PopupMenu::Options()> createMenuOptions = nullptr;

    juce::PopupMenu::Options menuOptionsFor(juce::Component *) const;

    /*
     * The component here is used to assume the lifetime fo the data object. As long
     * as component is still in a component hierarchy, data should be valid
     */
    void showMenu(DiscreteParamEditor *c);

    void setGroupList(const std::vector<std::pair<int, std::string>> &gl) { groupList = gl; }

    void populateLinearMenu(juce::PopupMenu &, DiscreteParamEditor *c);
    void populateGroupListMenu(juce::PopupMenu &, DiscreteParamEditor *c);

    int jogFromValue(int fromThis, int jog);

  protected:
    std::vector<std::pair<int, std::string>> groupList;
};

struct HasDiscreteParamMenuBuilder
{
    std::unique_ptr<DiscreteParamMenuBuilder> popupMenuBuilder;

    void guaranteeMenuBuilder()
    {
        if (!popupMenuBuilder)
            popupMenuBuilder = std::make_unique<DiscreteParamMenuBuilder>();
    }
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_DISCRETEPARAMMENUBUILDER_H
