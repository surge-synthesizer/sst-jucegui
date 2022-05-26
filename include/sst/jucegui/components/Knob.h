//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_KNOB_H
#define SST_JUCEGUI_KNOB_H

#include <string>

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Continuous.h>

#include "ComponentBase.h"

namespace sst::jucegui::components
{
struct Knob : public juce::Component,
              public style::StyleConsumer,
              public EditableComponentBase
{
    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"knob"};

        static constexpr sprop backgroundcol{"background.color"};
        static constexpr sprop ringcol{"ring.color"};
        static constexpr sprop modringcol{"modulationring.color"};
        static constexpr sprop gradientcenter{"gradientcenter.color"};
        static constexpr sprop labelcol{"label.color"};
    };

    Knob();
    ~Knob();

    void paint(juce::Graphics &g) override;

    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;

    void setSource(data::ContinunousModulatable *s) { source = s; }

  private:
    data::ContinunousModulatable *source{nullptr};
    float mouseDownV0, mouseDownY0;

    enum MouseMode
    {
        NONE,
        POPUP,
        DRAG
    } mouseMode{NONE};
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_KNOBS_H
