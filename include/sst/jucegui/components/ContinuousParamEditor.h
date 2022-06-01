//
// Created by Paul Walker on 5/26/22.
//

#ifndef SST_JUCEGUI_CONTINUOUSPARAMEDITOR_H
#define SST_JUCEGUI_CONTINUOUSPARAMEDITOR_H

#include <string>

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

#include "ComponentBase.h"

namespace sst::jucegui::components
{
struct ContinuousParamEditor : public juce::Component,
                               public Modulatable<ContinuousParamEditor>,
                               public EditableComponentBase<ContinuousParamEditor>,
                               public style::SettingsConsumer
{
    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"continuousParamEditor"};

        static constexpr sprop backgroundcol{"background.color"};
        static constexpr sprop valcol{"value.color"};
        static constexpr sprop handlecol{"handle.color"};
        static constexpr sprop guttercol{"gutter.color"};
        static constexpr sprop modvalcol{"modulationvalue.color"};
        static constexpr sprop modvalnegcol{"modulationnegativevalue.color"};
        static constexpr sprop modactivecol{"modulationactive.color"};
        static constexpr sprop modothercol{"modulationother.color"};
        static constexpr sprop labelcol{"label.color"};
    };

    enum Direction
    {
        VERTICAL,
        HORIZONTAL
    };
    ContinuousParamEditor(Direction primaryDirection);
    ~ContinuousParamEditor();

    void paint(juce::Graphics &g) override { g.fillAll(juce::Colours::red); }

    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    void mouseEnter(const juce::MouseEvent &e) override { startHover(); }
    void mouseExit(const juce::MouseEvent &e) override { endHover(); }

  protected:
    float mouseDownV0, mouseDownX0, mouseDownY0;

    enum MouseMode
    {
        NONE,
        POPUP,
        DRAG
    } mouseMode{NONE};
    Direction direction;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContinuousParamEditor);
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_CONTINUOUSPARAMEDITOR_H
