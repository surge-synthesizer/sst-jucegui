//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_HSLIDER_H
#define SST_JUCEGUI_HSLIDER_H

#include "ContinuousParamEditor.h"

namespace sst::jucegui::components
{
struct HSlider : public ContinuousParamEditor, public style::StyleConsumer
{
    struct Styles : ContinuousParamEditor::Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"hslider"};
        static constexpr sprop modhandlecol{"modhandle.color"};

        static constexpr sprop labeltextcol{"labeltext.color"};
        static constexpr sprop valuetextcol{"valuetext.color"};

        static constexpr sprop textfont{"text.font", sprop::FONT};
    };

    static constexpr int hanRadius = 7;
    static constexpr int gutterheight = 8;

    HSlider();
    ~HSlider();

    void paint(juce::Graphics &g) override;

    void setShowLabel(bool b) { showLabel = b; repaint();}
    void setShowValue(bool b) { showValue = b; repaint();}

    void setupInheritanceRelationships() override
    {
        namespace sc = sst::jucegui::components;

        style::StyleSheet::extendInheritanceMap(Styles::styleClass,
                                                sc::ContinuousParamEditor::Styles::styleClass);
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HSlider);
  protected:
    bool showLabel{true}, showValue{true};
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_KNOBS_H
