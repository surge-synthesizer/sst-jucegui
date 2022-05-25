
#include <sst/jucegui/components/NamedPanel.h>

namespace sst::jucegui::components
{

static constexpr int outerMargin = 2, cornerRadius = 4, headerHeight = 20;

NamedPanel::NamedPanel(const std::string &name)
    : style::StyleConsumer(Styles::className), name(name)
{
}

NamedPanel::~NamedPanel() {}

void NamedPanel::paint(juce::Graphics &g)
{
    auto b = getLocalBounds().reduced(outerMargin);
    g.setColour(juce::Colour(50, 50, 50));
    g.fillRoundedRectangle(b.toFloat(), cornerRadius);
    g.setColour(juce::Colour(70, 70, 70));
    g.drawRoundedRectangle(b.toFloat(), cornerRadius, 1);
    auto ht = b.withHeight(headerHeight).reduced(4, 0);
    g.setColour(juce::Colours::white);
    g.drawText(name, ht, juce::Justification::centredLeft);

    g.setColour(juce::Colour(180, 180, 180));
    auto fw = g.getCurrentFont().getStringWidth(name);
    ht = b.withHeight(headerHeight);
    auto q =
        ht.withTrimmedLeft(fw + 2).translated(0, ht.getHeight() / 2).withHeight(1).reduced(4, 0);
    g.fillRect(q);
}
void NamedPanel::resized()
{
    auto b = getLocalBounds().reduced(outerMargin);
    if (contentAreaComp)
    {
        auto c = b.withTrimmedTop(headerHeight).reduced(2);
        contentAreaComp->setBounds(c);
    }
}
} // namespace sst::jucegui::components