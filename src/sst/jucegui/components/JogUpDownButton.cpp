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

#include "sst/jucegui/components/JogUpDownButton.h"
#include "sst/jucegui/components/GlyphPainter.h"

namespace sst::jucegui::components
{
JogUpDownButton::JogUpDownButton() : style::StyleConsumer(Styles::styleClass) {}
JogUpDownButton::~JogUpDownButton()
{
    if (data)
    {
        data->removeGUIDataListener(this);
    }
}

void JogUpDownButton::paint(juce::Graphics &g)
{
    float rectCorner = 1.5;

    auto b = getLocalBounds().reduced(1).toFloat();

    // auto ol = getColour(Styles::brightoutline);
    auto bg = getColour(Styles::fill);
    auto tx = getColour(Styles::labelcolor);
    auto ar = tx;
    auto har = ar;
    if (isHovered)
    {
        tx = getColour(Styles::labelcolor_hover);
        har = getColour(Styles::jogbutton_hover);
    }

    if (!isEnabled())
    {
        g.setColour(bg.withAlpha(0.5f));
        g.drawRoundedRectangle(b, rectCorner, 1);

        auto col = har.withAlpha(0.5f);
        auto bl = b.withWidth(b.getHeight()).toNearestInt();
        GlyphPainter::paintGlyph(g, bl, GlyphPainter::GlyphType::JOG_LEFT, col);

        bl = b.withLeft(b.getRight() - b.getHeight()).toNearestInt();
        GlyphPainter::paintGlyph(g, bl, GlyphPainter::GlyphType::JOG_RIGHT, col);
        return;
    }

    g.setColour(bg);
    g.fillRoundedRectangle(b, rectCorner);

    if (!data)
        return;

    g.setFont(getFont(Styles::labelfont));
    g.setColour(tx);
    g.drawText(data->getValueAsString(), b, juce::Justification::centred);

    auto jwa = data->jogWrapsAtEnd;
    auto col = hoverX < b.getHeight() ? har : ar;
    if (!jwa && data->getValue() == data->getMin())
    {
        col = ar.withAlpha(0.5f);
    }
    auto bl = b.withWidth(b.getHeight()).toNearestInt();
    GlyphPainter::paintGlyph(g, bl, GlyphPainter::GlyphType::JOG_LEFT, col);

    col = hoverX > b.getRight() - b.getHeight() ? har : ar;
    if (!jwa && data->getValue() == data->getMax())
    {
        col = ar.withAlpha(0.5f);
    }
    bl = b.withLeft(b.getRight() - b.getHeight()).toNearestInt();
    GlyphPainter::paintGlyph(g, bl, GlyphPainter::GlyphType::JOG_RIGHT, col);
}

void JogUpDownButton::mouseDown(const juce::MouseEvent &e)
{
    if (isEnabled() && data &&
        (e.mods.isPopupMenu() ||
         (e.position.x > getHeight() && e.position.x < (getWidth() - getHeight()))))
    {
        if (popupMenuBuilder)
        {
            popupMenuBuilder->setData(data);
            popupMenuBuilder->showMenu(this);
        }
        else if (onPopupMenu)
        {
            onPopupMenu();
        }
        else
        {
            DiscreteParamMenuBuilder builder;
            builder.setData(data);
            builder.showMenu(this);
        }
        return;
    }
    DiscreteParamEditor::mouseDown(e);
}

void JogUpDownButton::mouseUp(const juce::MouseEvent &e)
{
    auto jog = 0;
    if (e.position.x < getHeight())
        jog = -1;
    if (e.position.x > getWidth() - getHeight())
        jog = 1;

    if (data && isEnabled() && jog != 0)
    {
        if (popupMenuBuilder)
        {
            popupMenuBuilder->setData(data);
            data->setValueFromGUI(popupMenuBuilder->jogFromValue(data->getValue(), jog));
        }
        else
        {
            data->jog(jog);
        }
    }
}
} // namespace sst::jucegui::components