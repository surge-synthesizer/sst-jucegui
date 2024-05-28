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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TYPEINOVERLAY_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TYPEINOVERLAY_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include "ContinuousParamEditor.h"

namespace sst::jucegui::components
{

struct TypeInOverlay : juce::Component, juce::TextEditor::Listener, style::StyleConsumer
{
    struct Styles : base_styles::Outlined,
                    base_styles::Base,
                    base_styles::BaseLabel,
                    base_styles::ValueBearing
    {
        SCLASS(typeinoverlay);
        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::Outlined::styleClass)
                .withBaseClass(base_styles::Base::styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass)
                .withBaseClass(base_styles::ValueBearing::styleClass);
        }
    };

    ContinuousParamEditor *editor{nullptr};
    std::unique_ptr<juce::TextEditor> textEd;
    TypeInOverlay(ContinuousParamEditor *ed) : style::StyleConsumer(Styles::styleClass), editor(ed)
    {
        textEd = std::make_unique<juce::TextEditor>();
        restyleTextEd();

        textEd->setText(editor->continuous()->getValueAsString());
        textEd->selectAll();

        addAndMakeVisible(*textEd);
        textEd->addListener(this);
    }

    void textEditorReturnKeyPressed(juce::TextEditor &) override
    {
        editor->onBeginEdit();
        editor->continuous()->setValueAsString(textEd->getText().toStdString());
        editor->onEndEdit();
        editor->dismissTypeIn();
    }
    void textEditorEscapeKeyPressed(juce::TextEditor &) override { editor->dismissTypeIn(); }
    void textEditorFocusLost(juce::TextEditor &) override { editor->dismissTypeIn(); }

    void focusGained(FocusChangeType cause) override { textEd->selectAll(); }

    static constexpr int labelHeight{18};
    void resized() override
    {
        restyleTextEd();
        textEd->setBounds(getLocalBounds().withTrimmedTop(labelHeight).reduced(2));
        textEd->setText(textEd->getText(), juce::NotificationType::dontSendNotification);
        textEd->selectAll();
    }
    void paint(juce::Graphics &g) override
    {
        g.fillAll(getColour(Styles::background));
        g.setColour(getColour(Styles::brightoutline));
        g.drawRect(getLocalBounds());
        g.setFont(getFont(Styles::labelfont));
        g.setColour(getColour(Styles::labelcolor));
        g.drawText(editor->continuous()->getLabel(), getLocalBounds().withHeight(labelHeight),
                   juce::Justification::centred);
        g.setColour(getColour(Styles::brightoutline));
        g.drawLine(0, labelHeight - 1, getWidth(), labelHeight - 1, 1);
    }

    void onStyleChanged() override { restyleTextEd(); }

    void restyleTextEd()
    {
        textEd->setFont(getFont(Styles::labelfont));
        textEd->setColour(juce::TextEditor::outlineColourId, getColour(Styles::outline));
        textEd->setColour(juce::TextEditor::textColourId, getColour(Styles::labelcolor));

        textEd->setColour(juce::TextEditor::ColourIds::outlineColourId,
                          juce::Colours::black.withAlpha(0.f));
        textEd->setColour(juce::TextEditor::ColourIds::focusedOutlineColourId,
                          juce::Colours::black.withAlpha(0.f));
        textEd->setColour(juce::TextEditor::ColourIds::backgroundColourId,
                          juce::Colours::black.withAlpha(0.f));
        textEd->setColour(juce::TextEditor::ColourIds::highlightedTextColourId,
                          getColour(Styles::labelcolor));
        textEd->setFont(getFont(Styles::labelfont));
        textEd->setIndents(0, 0);
        textEd->setJustification(juce::Justification::centred);

        textEd->applyFontToAllText(getFont(Styles::labelfont));
        textEd->applyColourToAllText(getColour(Styles::labelcolor));
        textEd->setText(textEd->getText(), juce::NotificationType::dontSendNotification);
        textEd->resized();
    }
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_TYPEINOVERLAY_H
