//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/components/Knob.h>

namespace sst::jucegui::components
{
Knob::Knob() : style::StyleConsumer(Styles::styleClass) {}
Knob::~Knob() = default;

void Knob::paint(juce::Graphics &g)
{
    if (!source)
    {
        g.fillAll(juce::Colours::red);
        return;
    }
    auto b = getLocalBounds();

    auto knobarea = b.withHeight(b.getWidth());

    auto inner = knobarea.reduced(8);
    auto outer = knobarea.reduced(3);
    auto mod = knobarea.reduced(2);
    g.setColour(juce::Colours::blue);
    g.fillEllipse(mod.toFloat());
    g.setColour(juce::Colours::yellow);
    g.fillEllipse(outer.toFloat());
    g.setColour(juce::Colours::black);
    g.fillEllipse(inner.toFloat());

    auto textarea = b.withTrimmedTop(b.getWidth());
    g.setColour(juce::Colours::black);
    g.drawText(source->getLabel(), textarea, juce::Justification::centred);
}

} // namespace sst::jucegui::components