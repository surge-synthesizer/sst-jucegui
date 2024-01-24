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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_DISCRETEPARAMMENUBUILDER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_DISCRETEPARAMMENUBUILDER_H

#include <optional>
#include <vector>
#include <utility>

#include <juce_gui_basics/juce_gui_basics.h>
#include <sst/jucegui/data/Discrete.h>

namespace sst::jucegui::components
{
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

    std::function<juce::PopupMenu::Options()> createMenuOptions = []() {
        return juce::PopupMenu::Options();
    };

    /*
     * The component here is used to assume the lifetime fo the data object. As long
     * as component is still in a component hierarchy, data should be valid
     */
    void showMenu(juce::Component *c);

    std::vector<std::pair<int, std::string>> groupList;

    void populateLinearMenu(juce::PopupMenu &, juce::Component *c);
    void populateGroupListMenu(juce::PopupMenu &, juce::Component *c);
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
