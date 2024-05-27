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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_HSLIDER_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_HSLIDER_H

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

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(ContinuousParamEditor::Styles::styleClass);
        }
    };

    static constexpr int hanRadius = 7;
    static constexpr int gutterheight = 4;

    HSlider();
    ~HSlider();

    void paint(juce::Graphics &g) override;

    void setShowLabel(bool b)
    {
        showLabel = b;
        repaint();
    }
    void setShowValue(bool b)
    {
        showValue = b;
        repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HSlider);

  protected:
    bool showLabel{true}, showValue{true};
};
} // namespace sst::jucegui::components
#endif // SST_JUCEGUI_KNOBS_H
