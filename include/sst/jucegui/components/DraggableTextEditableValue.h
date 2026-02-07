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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_DRAGGABLETEXTEDITABLEVALUE_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_DRAGGABLETEXTEDITABLEVALUE_H

#include <string>

#include <juce_gui_basics/juce_gui_basics.h>

#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include <sst/jucegui/components/BaseStyles.h>
#include <sst/jucegui/components/ContinuousParamEditor.h>

#include "ComponentBase.h"

namespace sst::jucegui::components
{

struct DraggableTextEditableValue : public ContinuousParamEditor, public style::StyleConsumer
{
    struct Styles : base_styles::Base,
                    base_styles::Outlined,
                    base_styles::BaseLabel,
                    base_styles::ValueBearing
    {
        SCLASS(draggabletexteditor);
        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::Base::styleClass)
                .withBaseClass(base_styles::Outlined::styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass)
                .withBaseClass(base_styles::ValueBearing::styleClass);
        }
    };

    DraggableTextEditableValue();
    ~DraggableTextEditableValue() override;

    void paint(juce::Graphics &g) override;
    void resized() override;

    void mouseDown(const juce::MouseEvent &e) override;
    void mouseUp(const juce::MouseEvent &e) override;
    void mouseDrag(const juce::MouseEvent &e) override;

    void mouseEnter(const juce::MouseEvent &e) override { startHover(); }
    void mouseExit(const juce::MouseEvent &e) override { endHover(); }

    void mouseDoubleClick(const juce::MouseEvent &e) override;

    void setFromEditor();
    void onStyleChanged() override;

    void setDisplayUnits(bool b)
    {
        displayUnits = b;
        repaint();
    }

    void activateEditor();

    bool isSetFromDrag() const { return isEditDrag; }

  protected:
    float valueOnMouseDown{0.f};
    float displayUnits{false};
    bool everDragged{false};
    std::unique_ptr<juce::TextEditor> underlyingEditor;

    bool isEditDrag{false};
    struct EditIsDragGuard
    {
        DraggableTextEditableValue &w;
        EditIsDragGuard(DraggableTextEditableValue &w) : w(w) { w.isEditDrag = true; }
        ~EditIsDragGuard() { w.isEditDrag = false; }
    };
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_DRAGGABLETEXTEDITABLEVALUE_H
