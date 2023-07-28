/*
 * sst-juce-gui - an open source library of juce widgets
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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TOGGLEBUTTON_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TOGGLEBUTTON_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Discrete.h>
#include <sst/jucegui/components/BaseStyles.h>

#include <string>

#include "ComponentBase.h"

namespace sst::jucegui::components
{
struct ToggleButton : public juce::Component,
                      public style::StyleConsumer,
                      public style::SettingsConsumer,
                      public EditableComponentBase<ToggleButton>,
                      public data::Discrete::DataListener
{
    ToggleButton();
    ~ToggleButton();

    struct Styles : TextualControlStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"togglebutton"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(TextualControlStyles::styleClass);
        }
    };

    enum struct DrawMode
    {
        LABELED,
        FILLED
    } drawMode{DrawMode::LABELED};

    void dataChanged() override;
    void setDrawMode(DrawMode m)
    {
        drawMode = m;
        repaint();
    }
    void setLabel(const std::string &l) { label = l; }
    void setSource(data::Discrete *d)
    {
        if (data)
            data->removeGUIDataListener(this);
        data = d;
        if (data)
            data->addGUIDataListener(this);
        repaint();
    }

    void mouseEnter(const juce::MouseEvent &e) override
    {
        startHover();
        repaint();
    }
    void mouseExit(const juce::MouseEvent &e) override
    {
        endHover();
        repaint();
    }

    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;

    void paint(juce::Graphics &g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ToggleButton);

  private:
    std::string label;
    data::Discrete *data{nullptr};
};

} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_TOGGLEBUTTON_H
