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

#include "sst/jucegui/components/MenuButton.h"

namespace sst::jucegui::components
{
MenuButton::MenuButton() {}

MenuButton::~MenuButton() {}

template <typename T>
void MenuButtonPainter<T>::paintMenuButton(juce::Graphics &g, const std::string &label)
{
    T *that = static_cast<T *>(this);
    float rectCorner = 1.5;

    auto b = that->getLocalBounds().reduced(1).toFloat();

    auto bg = getColour(Styles::fill);
    auto tx = getColour(Styles::labelcolor);
    auto ar = tx;
    if (that->isHovered)
    {
        tx = getColour(Styles::labelcolor_hover);
        ar = getColour(Styles::menuarrow_hover);
    }
    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    g.setFont(getFont(Styles::labelfont));
    g.setColour(tx);
    if (that->centerTextAndExcludeArrow)
        g.drawText(label, b, juce::Justification::centred);
    else
        g.drawText(label, b.withTrimmedLeft(5), juce::Justification::centredLeft);

    g.setColour(ar);
    auto q = b.withTrimmedRight(5);
    q = q.withLeft(q.getRight() - 6);
    auto cy = q.getCentreY();
    auto au = cy - 2;
    auto ad = cy + 2;

    if (!that->centerTextAndExcludeArrow)
    {
        auto cx = q.getCentreX();
        auto aL = cx - 3;
        auto aR = cx + 3;
        auto p = juce::Path();
        p.startNewSubPath(aL, au);
        p.lineTo(aR, au);
        p.lineTo(cx, ad);
        p.closeSubPath();

        g.fillPath(p);
    }
}

void MenuButton::paint(juce::Graphics &g) { paintMenuButton(g, label); }

void MenuButtonDiscreteEditor::paint(juce::Graphics &g)
{
    if (!data)
        g.fillAll(juce::Colours::red);
    paintMenuButton(g, data->getValueAsString());
}

void MenuButtonDiscreteEditor::mouseDown(const juce::MouseEvent &e)
{
    if (popupMenuBuilder)
    {
        popupMenuBuilder->setData(data);
        popupMenuBuilder->showMenu(this);
    }
    else
    {
        DiscreteParamMenuBuilder builder;
        builder.setData(data);
        builder.showMenu(this);
    }
}
} // namespace sst::jucegui::components