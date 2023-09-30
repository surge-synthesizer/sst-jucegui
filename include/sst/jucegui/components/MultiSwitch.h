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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_MULTISWITCH_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_MULTISWITCH_H

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/data/Discrete.h>
#include <sst/jucegui/components/BaseStyles.h>

#include <string>

#include "ComponentBase.h"
#include "DiscreteParamEditor.h"

namespace sst::jucegui::components
{
struct MultiSwitch : public DiscreteParamEditor,
                     public style::StyleConsumer,
                     public style::SettingsConsumer
{
    enum Direction
    {
        VERTICAL,
        HORIZONTAL
    } direction{VERTICAL};
    MultiSwitch(Direction d = VERTICAL);
    ~MultiSwitch();

    struct Styles : TextualControlStyles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"multiswitch"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass).withBaseClass(TextualControlStyles::styleClass);
        }
    };

    void mouseMove(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;

    void paint(juce::Graphics &g) override;

    void setElementSize(int i)
    {
        elementSize = i;
        repaint();
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MultiSwitch)

  private:
    int elementSize{std::numeric_limits<int>::max()};
    void setValueFromMouse(const juce::MouseEvent &e);
};

} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_MULTISWITCH_H
