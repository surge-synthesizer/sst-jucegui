//
// Created by Paul Walker on 5/25/22.
//

#include <sst/jucegui/components/VSlider.h>

namespace sst::jucegui::components
{
VSlider::VSlider() : style::StyleConsumer(Styles::styleClass) {}
VSlider::~VSlider() = default;

void VSlider::paint(juce::Graphics &g)
{
    g.fillAll(juce::Colours::red);

    auto v = source->getValue01();
    auto h = (1.0 - v) * getHeight();
    auto r = getLocalBounds().withTrimmedTop(h).withHeight(1).expanded(0, 4);
    g.setColour(juce::Colours::yellow);
    g.fillRect(r);
}

} // namespace sst::jucegui::components