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

#include "sst/jucegui/components/TextEditor.h"

namespace sst::jucegui::components
{
TextEditor::TextEditor() : style::StyleConsumer(Styles::styleClass) { setAccessible(true); }

void TextEditor::setAllText(const std::string &text)
{
    setText(text, juce::NotificationType::dontSendNotification);
    if (style())
    {
        applyFontToAllText(style()->getFont(Styles::styleClass, Styles::labelfont));
        applyColourToAllText(style()->getColour(Styles::styleClass, Styles::labelcolor));
    }
}

void TextEditor::onStyleChanged()
{
    setColour(juce::TextEditor::outlineColourId, getColour(Styles::outline));
    setColour(juce::TextEditor::focusedOutlineColourId, getColour(Styles::brightoutline));
    setColour(juce::TextEditor::textColourId, getColour(Styles::labelcolor));

    setColour(juce::TextEditor::ColourIds::backgroundColourId, getColour(Styles::gutter));
    setColour(juce::TextEditor::ColourIds::highlightedTextColourId,
              getColour(Styles::labelcolor_hover));
    setFont(style()->getFont(Styles::styleClass, Styles::labelfont));
    setIndents(3, 3);
    setJustification(juce::Justification::topLeft);

    applyFontToAllText(style()->getFont(Styles::styleClass, Styles::labelfont));
    applyColourToAllText(style()->getColour(Styles::styleClass, Styles::labelcolor));
    setText(getText(), juce::NotificationType::dontSendNotification);
    resized();
}

} // namespace sst::jucegui::components