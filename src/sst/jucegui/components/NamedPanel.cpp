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

#include <sst/jucegui/components/NamedPanel.h>
#include <sst/jucegui/components/ToggleButton.h>
#include <cassert>
#include <cassert>

namespace sst::jucegui::components
{

NamedPanel::NamedPanel(const std::string &name)
    : style::StyleConsumer(Styles::styleClass), name(name)
{
    setAccessible(true);
    setTitle(name);
    setDescription(name);
    setWantsKeyboardFocus(true);
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
    if (selectable && selected)
    {
        g.setColour(getColour(Styles::backgroundSelected));
    }
    else
    {
        g.setColour(getColour(Styles::background));
    }
    g.fillRoundedRectangle(b.toFloat(), cornerRadius);
    if (isAccented)
    {
        g.setColour(getColour(Styles::accentedPanel));
    }
    else
    {
        g.setColour(getColour(Styles::brightoutline));
    }
    g.drawRoundedRectangle(b.toFloat(), cornerRadius, 1);

    paintHeader(g);
}

void NamedPanel::paintHeader(juce::Graphics &g)
{
    auto b = getLocalBounds().reduced(outerMargin);
    auto ht = b.withHeight(headerHeight).reduced(4, 0);

    if (isTogglable)
        ht = ht.withTrimmedLeft(headerHeight - togglePad);

    auto labelWidth{0};
    if (isTabbed)
    {
        labelWidth = totalTabArea.getWidth();

        for (int i = 0; i < tabNames.size(); ++i)
        {
            auto s = tabNames[i];
            auto p = tabPositions[i];

            if (i == selectedTab)
            {
                g.setFont(getFont(Styles::labelfont));
                g.setColour(getColour(Styles::selectedtab));
                g.drawText("[ " + s + " ]", p, juce::Justification::centred);
            }
            else
            {
                g.setFont(getFont(Styles::labelfont));
                g.setColour(getColour(Styles::labelcolor));
                g.drawText(s, p, juce::Justification::centred);
            }
        }
    }
    else if (centeredHeader)
    {
        g.setFont(getFont(Styles::labelfont));
        g.setColour(getColour(Styles::labelcolor));
        g.drawText(name, ht, juce::Justification::centred);
        labelWidth = SST_STRING_WIDTH_INT(g.getCurrentFont(), name);
    }
    else
    {
        g.setFont(getFont(Styles::labelfont));
        g.setColour(getColour(Styles::labelcolor));
        g.drawText(name, ht, juce::Justification::centredLeft);
        labelWidth = SST_STRING_WIDTH_INT(g.getCurrentFont(), name);
    }

    g.setColour(getColour(Styles::labelrule));
    ht = b.withHeight(headerHeight);

    auto showHamburger = isEnabled() && hasHamburger;

    if (centeredHeader)
    {
        auto q = ht.toFloat()
                     .withWidth((ht.getWidth() - labelWidth - 2) / 2)
                     .translated(0, ht.getHeight() / 2 - 0.5)
                     .withHeight(1)
                     .reduced(4, 0);
        g.fillRect(q);
        q = ht.toFloat()
                .withWidth((ht.getWidth() - labelWidth - 2) / 2)
                .translated((ht.getWidth() - labelWidth - 2) / 2 + labelWidth + 2,
                            ht.getHeight() / 2 - 0.5)
                .withHeight(1)
                .reduced(4, 0);
        g.fillRect(q);

        assert(!showHamburger);
    }
    else
    {
        lastPaintedHeaderTextRegion =
            ht.withTrimmedLeft((isTogglable ? headerHeight - togglePad : 0))
                .withTrimmedRight(showHamburger * hamburgerSize);

        auto q = ht.toFloat()
                     .withTrimmedLeft(labelWidth + 4 + (isTogglable ? headerHeight - togglePad : 0))
                     .translated(0, ht.getHeight() / 2 - 0.5)
                     .withHeight(1)
                     .reduced(4, 0)
                     .withTrimmedRight(showHamburger * hamburgerSize)
                     .withTrimmedRight(additionalHamburgerComponents.size() *
                                       (headerHeight + outerMargin));

        g.fillRect(q);
    }

    if (showHamburger)
    {
        auto hb = getHamburgerRegion();
        auto cx = hb.getCentreX();
        auto cy = hb.getCentreY();
        auto w = hb.getWidth() / 3.5;

        auto sz = 1.5;
        g.fillEllipse(cx - sz, cy - sz, 2 * sz, 2 * sz);
        g.fillEllipse(cx - w - sz, cy - sz, 2 * sz, 2 * sz);
        g.fillEllipse(cx + w - sz, cy - sz, 2 * sz, 2 * sz);
    }
}

juce::Rectangle<int> NamedPanel::getContentArea()
{
    auto b = getLocalBounds().reduced(outerMargin).withTrimmedTop(headerHeight).reduced(2);
    return b;
}

void NamedPanel::resized()
{
    if (contentAreaComp)
    {
        auto c = getContentArea();
        contentAreaComp->setBounds(c);
    }

    if (isTabbed)
    {
        resetTabState();
    }

    if (isTogglable && toggleButton)
    {
        toggleButton->setBounds(getLocalBounds()
                                    .reduced(2)
                                    .withHeight(headerHeight)
                                    .withWidth(headerHeight)
                                    .reduced(togglePad + 1));
    }

    if (!additionalHamburgerComponents.empty())
    {
        resizeHamburgerComponents();
    }
}

void NamedPanel::resizeHamburgerComponents()
{
    auto sx = getWidth() - outerMargin;
    if (hasHamburger)
        sx = getHamburgerRegion().getX();

    auto bbx = juce::Rectangle<int>(sx - headerHeight, outerMargin, headerHeight, headerHeight)
                   .reduced(outerMargin, outerMargin);

    for (auto &h : additionalHamburgerComponents)
    {
        h->setBounds(bbx);
        bbx = bbx.translated(-headerHeight - outerMargin, 0);
    }
}

juce::Rectangle<int> NamedPanel::getHamburgerRegion()
{
    auto b = getLocalBounds().reduced(outerMargin);
    auto ht = b.withHeight(headerHeight).reduced(4, 0);
    auto hb = ht.withLeft(ht.getRight() - hamburgerSize);

    return hb;
}
void NamedPanel::mouseDown(const juce::MouseEvent &event)
{
    if (hasHamburger && onHamburger && getHamburgerRegion().toFloat().contains(event.position))
    {
        onHamburger();
    }

    if (isTabbed)
    {
        auto pst = selectedTab;
        for (int i = 0; i < tabPositions.size(); ++i)
        {
            if (tabPositions[i].toFloat().contains(event.position))
            {
                selectTab(i);
            }
        }
        if (pst != selectedTab)
            repaint();
    }
}

void NamedPanel::mouseMove(const juce::MouseEvent &event)
{
    auto p = localAreaToGlobal(lastPaintedHeaderTextRegion);
    if (lastPaintedHeaderTextRegion.toFloat().contains(event.position))
    {
        activateOptionalCursorForNamedArea(true);
    }
    else
    {
        activateOptionalCursorForNamedArea(false);
    }
}

void NamedPanel::mouseExit(const juce::MouseEvent &event)
{
    activateOptionalCursorForNamedArea(false);
}

void NamedPanel::activateOptionalCursorForNamedArea(bool b)
{
    if (isOptionalCursorOn != b)
    {
        isOptionalCursorOn = b;

        if (optionalCursor.has_value())
        {
            if (isOptionalCursorOn)
            {
                setMouseCursor(*optionalCursor);
            }
            else
            {
                setMouseCursor(juce::MouseCursor::StandardCursorType::NormalCursor);
            }
        }
    }
}

void NamedPanel::resetTabState()
{
    auto f = getFont(Styles::labelfont);

    auto b = getLocalBounds().reduced(outerMargin);
    auto ht = b.withHeight(headerHeight).reduced(4, 0); // the extra margin

    int totalTabSize = 4;
    tabPositions.clear();
    for (const auto &t : tabNames)
    {
        auto fw = SST_STRING_WIDTH_INT(f, "[ " + t + " ]");
        auto tt = ht.withLeft(totalTabSize + 4).withWidth(fw);
        tabPositions.push_back(tt);
        totalTabSize += fw;
    }

    totalTabArea = ht.withWidth(totalTabSize);
}

void NamedPanel::selectTab(int tab)
{
    selectedTab = tab;
    if (onTabSelected)
        onTabSelected(tab);
    repaint();
}

void NamedPanel::setTogglable(bool b)
{
    if (b == isTogglable)
        return;

    isTogglable = b;
    if (!isTogglable)
    {
        if (toggleButton)
        {
            removeChildComponent(toggleButton.get());
            toggleButton.reset();
        }
        resized();
    }
    else
    {
        if (!toggleButton)
        {
            toggleButton = std::make_unique<ToggleButton>();
            toggleButton->setDrawMode(ToggleButton::DrawMode::FILLED);
            toggleButton->setAccessible(true);
            toggleButton->setTitle(name + " Toggle");
            addAndMakeVisible(*toggleButton);
        }
        resized();
    }
    repaint();
}

void NamedPanel::setToggleDataSource(data::Discrete *d)
{
    assert(isTogglable);
    assert(toggleButton);
    toggleButton->setSource(d);
}
} // namespace sst::jucegui::components