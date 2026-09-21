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
#include "sst/jucegui/style/StyleSheet.h"
#include "sst/jucegui/components/GlyphPainter.h"

namespace sst::jucegui::components
{
JogUpDownButton::JogUpDownButton() : style::StyleConsumer(Styles::styleClass)
{
    setAccessible(true);
    setWantsKeyboardFocus(true);
}
JogUpDownButton::~JogUpDownButton()
{
    if (data)
    {
        data->removeGUIDataListener(this);
    }
}

juce::Rectangle<int> JogUpDownButton::leftButtonBound() const
{
    if (arrowPosition == ArrowPosition::LEFT_AND_RIGHT)
    {
        auto bx = getLocalBounds().reduced(1);
        return bx.withWidth(bx.getHeight());
    }
    else
    {
        auto bx = getLocalBounds().reduced(1);
        return bx.withTrimmedLeft(bx.getWidth() - 2 * bx.getHeight()).withWidth(bx.getHeight());
    }
}

juce::Rectangle<int> JogUpDownButton::rightButtonBound() const
{
    auto bx = getLocalBounds().reduced(1);
    return bx.withTrimmedLeft(bx.getWidth() - bx.getHeight());
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

    float alpha = isEnabled() ? 1.f : 0.5f;

    g.setColour(bg.withAlpha(alpha));
    g.fillRoundedRectangle(b, rectCorner);

    if (!data)
        return;

    auto jwa = data->jogWrapsAtEnd;
    auto lbb = leftButtonBound();
    auto rbb = rightButtonBound();

    /*
     * The label lives between the arrows, not across them, and is ellipsised when it will not
     * fit. Drawn centred in the full bounds a long value ran underneath the arrows and off
     * both ends; a value the widget cannot show in full should say so rather than collide with
     * the controls that change it.
     */
    {
        auto fnt = getFont(Styles::labelfont);
        auto textArea = b.withLeft(lbb.getRight() + 2.f).withRight(rbb.getX() - 2.f);

        auto txt = juce::String(data->getValueAsString());
        if (textArea.getWidth() > 0 && SST_STRING_WIDTH_FLOAT(fnt, txt) > textArea.getWidth())
        {
            static const auto ellipsis = juce::String::fromUTF8("\xe2\x80\xa6");
            auto fits = juce::String();
            for (int i = 1; i <= txt.length(); ++i)
            {
                auto cand = txt.substring(0, i);
                if (SST_STRING_WIDTH_FLOAT(fnt, cand + ellipsis) > textArea.getWidth())
                    break;
                fits = cand;
            }
            txt = fits.trimEnd() + ellipsis;
        }

        g.setFont(fnt);
        g.setColour(tx.withAlpha(alpha));
        g.drawText(txt, textArea, juce::Justification::centred);
    }

    auto col = lbb.contains(hoverX, getHeight() / 2) ? har : ar;
    col = col.withAlpha(alpha);
    if (!jwa && data->getValue() == data->getMin())
    {
        col = ar.withAlpha(0.5f);
    }
    GlyphPainter::paintGlyph(g, lbb, GlyphPainter::GlyphType::JOG_LEFT, col);

    col = rbb.contains(hoverX, getHeight() / 2) ? har : ar;
    col = col.withAlpha(alpha);
    if (!jwa && data->getValue() == data->getMax())
    {
        col = ar.withAlpha(0.5f);
    }
    GlyphPainter::paintGlyph(g, rbb, GlyphPainter::GlyphType::JOG_RIGHT, col);
}

bool JogUpDownButton::isOverControl(const juce::Point<int> &e) const
{
    return leftButtonBound().contains(e) || rightButtonBound().contains(e);
}

void JogUpDownButton::mouseDown(const juce::MouseEvent &e)
{
    if (data && (e.mods.isPopupMenu() || !isOverControl(e.position.toInt())))
    {
        showPopup(e.mods);
        return;
    }
    DiscreteParamEditor::mouseDown(e);
}

void JogUpDownButton::showPopup(const juce::ModifierKeys &m)
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
}

void JogUpDownButton::mouseUp(const juce::MouseEvent &e)
{
    if (e.mods.isPopupMenu())
        return;

    auto jog = 0;
    if (leftButtonBound().contains(e.position.toInt()))
        jog = -1;
    if (rightButtonBound().contains(e.position.toInt()))
        jog = 1;

    if (data && jog != 0)
    {
        if (onBeginEdit)
            onBeginEdit();
        if (popupMenuBuilder)
        {
            popupMenuBuilder->setData(data);
            data->setValueFromGUI(popupMenuBuilder->jogFromValue(data->getValue(), jog));
        }
        else
        {
            data->jog(jog);
            repaint();
        }
        if (onEndEdit)
            onEndEdit();
    }
}
} // namespace sst::jucegui::components