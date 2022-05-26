//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_VSLIDER_H
#define SST_JUCEGUI_VSLIDER_H

#include "ContinuousParamEditor.h"

namespace sst::jucegui::components
{
struct VSlider : public ContinuousParamEditor
{
    struct Styles : ContinuousParamEditor::Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"vslider"};
    };

    VSlider();
    ~VSlider();

    void paint(juce::Graphics &g) override;
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_KNOBS_H
