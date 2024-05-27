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

#include "sst/jucegui/components/TextPushButton.h"
#include "sst/jucegui/components/ButtonPainter.h"

namespace sst::jucegui::components
{
TextPushButton::TextPushButton() : style::StyleConsumer(Styles::styleClass) {}

TextPushButton::~TextPushButton() {}

void TextPushButton::paint(juce::Graphics &g)
{
    paintButtonBG(this, g);
    g.setFont(getFont(Styles::labelfont));
    if (isHovered && isEnabled())
        g.setColour(getColour(Styles::labelcolor_hover));
    else
    {
        float alpha = isEnabled() ? 1.f : 0.5f;
        g.setColour(getColour(Styles::labelcolor).withAlpha(alpha));
    }
    g.drawText(label, getLocalBounds(), juce::Justification::centred);
}
} // namespace sst::jucegui::components