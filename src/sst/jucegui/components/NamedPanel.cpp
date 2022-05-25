
#include <sst/jucegui/components/NamedPanel.h>

namespace sst::jucegui::components
{

static constexpr int outerMargin = 2, cornerRadius = 4, headerHeight = 20;

NamedPanel::NamedPanel(const std::string &name) : name(name) {}
NamedPanel::~NamedPanel() {}
void NamedPanel::paint(juce::Graphics &g)
{
    auto b = getLocalBounds().reduced(outerMargin);
    g.setColour(juce::Colours::pink);
    g.fillRoundedRectangle(b.toFloat(), cornerRadius);
    g.setColour(juce::Colours::white);
    g.drawRoundedRectangle(b.toFloat(), cornerRadius, 1);
    auto ht = b.withHeight(headerHeight).reduced(4, 0);
    g.setColour(juce::Colours::white);
    g.drawFittedText(name, ht, juce::Justification::centredLeft, 1);

    auto kn = b.withHeight(1).translated(0, headerHeight).reduced(4, 0);
    g.fillRect(kn);
}
void NamedPanel::resized()
{
    auto b = getLocalBounds().reduced(outerMargin);
    if (headerControlAreaComp)
    {
        auto h = b.withHeight(headerHeight).reduced(4, 2).withTrimmedLeft(getWidth() / 2 - 2);
        headerControlAreaComp->setBounds(h);
    }
    if (contentAreaComp)
    {
        auto c = b.withTrimmedTop(headerHeight).reduced(2);
        contentAreaComp->setBounds(c);
    }
}
} // namespace sst::jucegui::components