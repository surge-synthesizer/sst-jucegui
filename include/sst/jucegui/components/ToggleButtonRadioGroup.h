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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TOGGLEBUTTONRADIOGROUP_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TOGGLEBUTTONRADIOGROUP_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <memory>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Discrete.h>
#include <sst/jucegui/components/BaseStyles.h>

#include <string>

#include "ComponentBase.h"
#include "ToggleButton.h"

namespace sst::jucegui::components
{
struct ToggleButtonRadioGroup : public juce::Component,
                                public style::StyleConsumer,
                                public style::SettingsConsumer,
                                public EditableComponentBase<ToggleButtonRadioGroup>,
                                public data::Discrete::DataListener
{
    ToggleButtonRadioGroup();
    ~ToggleButtonRadioGroup();

    struct Styles : TextualControlStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"togglebuttonradiogroup"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(TextualControlStyles::styleClass);
        }
    };

    void dataChanged() override;
    void setSource(data::Discrete *d)
    {
        if (data)
            data->removeGUIDataListener(this);
        data = d;
        if (data)
            data->addGUIDataListener(this);
        dataChanged();
        repaint();
    }

    void resized() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleButtonRadioGroup);

  private:
    std::string label;
    data::Discrete *data{nullptr};

    std::vector<std::unique_ptr<ToggleButton>> buttons;
    std::vector<std::unique_ptr<data::Discrete>> buttonSubData;
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_TOGGLEBUTTONRADIOGROUP_H
