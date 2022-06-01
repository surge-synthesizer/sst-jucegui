//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_KNOB_H
#define SST_JUCEGUI_KNOB_H

#include "ContinuousParamEditor.h"

namespace sst::jucegui::components
{
struct Knob : public ContinuousParamEditor, public style::StyleConsumer
{
    struct Styles : ContinuousParamEditor::Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"knob"};

        static constexpr sprop gradientcenter{"gradientcenter.color"};
    };

    Knob();
    ~Knob();

    void paint(juce::Graphics &g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Knob);
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_KNOBS_H
