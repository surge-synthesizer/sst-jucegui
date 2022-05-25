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

namespace sst::jucegui::components
{
struct Knob : public juce::Component, public style::StyleConsumer
{
    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;

        static constexpr sclass styleClass{"knob"};
    };

    Knob();
    ~Knob();

    void paint(juce::Graphics &g) override;

    data::ContinunousModulatable *source{nullptr};
    void setSource(data::ContinunousModulatable *s) { source = s; }
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_KNOBS_H
