//
// Created by Paul Walker on 5/30/22.
//

#ifndef SST_JUCEGUI_TOGGLEBUTTON_H
#define SST_JUCEGUI_TOGGLEBUTTON_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Discrete.h>

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

    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"togglebutton"};

        static constexpr sprop bordercol{"border.color"};
        static constexpr sprop onbgcol{"onbg.color"};
        static constexpr sprop offbgcol{"offbg.color"};
        static constexpr sprop hoveronbgcol{"hoveronbg.color"};
        static constexpr sprop hoveroffbgcol{"hoveroffbg.color"};
        static constexpr sprop textoncol{"texton.color"};
        static constexpr sprop textoffcol{"textoff.color"};
        static constexpr sprop texthoveroncol{"texton.color"};
        static constexpr sprop texthoveroffcol{"textoff.color"};
    };

    void dataChanged() override;
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

  private:
    std::string label;
    data::Discrete *data{nullptr};
};

} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_TOGGLEBUTTON_H
