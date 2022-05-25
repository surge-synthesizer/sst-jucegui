
#include <sst/jucegui/components/NamedPanel.h>

namespace sst::jucegui::components
{

static constexpr int outerMargin = 2, cornerRadius = 4, headerHeight = 20;

NamedPanel::NamedPanel(const std::string &name)
    : style::StyleConsumer(Styles::styleClass), name(name)
{
}

NamedPanel::~NamedPanel() {}

void NamedPanel::paint(juce::Graphics &g)
{
    if (!style())
    {
        g.fillAll(juce::Colours::red);
        return;
    }
    auto b = getLocalBounds().reduced(outerMargin);
    g.setColour(style()->getColour(Styles::styleClass, Styles::backgroundcol));
    g.fillRoundedRectangle(b.toFloat(), cornerRadius);
    g.setColour(style()->getColour(Styles::styleClass, Styles::bordercol));
    g.drawRoundedRectangle(b.toFloat(), cornerRadius, 1);
    auto ht = b.withHeight(headerHeight).reduced(4, 0);
    g.setColour(style()->getColour(Styles::styleClass, Styles::labelcol));
    g.drawText(name, ht, juce::Justification::centredLeft);

    g.setColour(style()->getColour(Styles::styleClass, Styles::labelrulecol));

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