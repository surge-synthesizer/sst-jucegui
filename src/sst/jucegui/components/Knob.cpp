/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
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

#include <sst/jucegui/components/Knob.h>
#include "KnobPainter.hxx"

namespace sst::jucegui::components
{
Knob::Knob() : style::StyleConsumer(Styles::styleClass), ContinuousParamEditor(Direction::VERTICAL)
{
}
Knob::~Knob() = default;

void Knob::paint(juce::Graphics &g)
{
    auto b = getLocalBounds();
    if (continuousModulatable())
    {
        knobPainter(g, this, continuousModulatable());
    }
    else
    {
        knobPainter(g, this, continuous());
    }
    if (drawLabel)
    {
        auto textarea = b.withTrimmedTop(b.getWidth());
        if (isHovered)
            g.setColour(getColour(Styles::labelcolor_hover));
        else
            g.setColour(getColour(Styles::labelcolor));
        g.setFont(getFont(Styles::labelfont));
        g.drawText(continuous()->getLabel(), textarea, juce::Justification::centred);
    }
}

} // namespace sst::jucegui::components