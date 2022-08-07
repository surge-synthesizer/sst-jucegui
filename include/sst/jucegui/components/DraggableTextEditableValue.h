//
// Created by Paul Walker on 8/7/22.
//

#ifndef SST_JUCEGUI_DRAGGABLETEXTEDITABLEVALUE_H
#define SST_JUCEGUI_DRAGGABLETEXTEDITABLEVALUE_H

#include <string>

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>

#include "ComponentBase.h"

namespace sst::jucegui::components
{
struct DraggableTextEditableValue : public juce::Component,
                                    public EditableComponentBase<DraggableTextEditableValue>,
                                    public Modulatable<DraggableTextEditableValue>,
                                    public style::SettingsConsumer,
                                    public style::StyleConsumer
{
    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"draggabletexteditor"};
       
        static constexpr sprop bgcol{"bg.color"};
        static constexpr sprop bghovcol{"bghover.color"};
        static constexpr sprop bgedcol{"bgedit.color"};
        static constexpr sprop outlinecol{"outline.color"};
        static constexpr sprop textcol{"text.color"};
        static constexpr sprop textfont{"text.font", sprop::FONT};
    };

    DraggableTextEditableValue();

    void paint(juce::Graphics &g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;
    void mouseWheelMove(const juce::MouseEvent &event,
                        const juce::MouseWheelDetails &wheel) override;

    void mouseEnter(const juce::MouseEvent &e) override { startHover(); }
    void mouseExit(const juce::MouseEvent &e) override { endHover(); }

    void mouseDoubleClick(const juce::MouseEvent &e) override;

    void setFromEditor();
    void onStyleChanged() override;

  private:
    float valueOnMouseDown{0.f};
    std::unique_ptr<juce::TextEditor> underlyingEditor;
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_DRAGGABLETEXTEDITABLEVALUE_H
