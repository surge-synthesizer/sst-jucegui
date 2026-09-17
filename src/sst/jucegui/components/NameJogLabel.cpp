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

#include "sst/jucegui/components/NameJogLabel.h"
#include "sst/jucegui/components/GlyphPainter.h"

namespace sst::jucegui::components
{
NameJogLabel::NameJogLabel() : style::StyleConsumer(Styles::styleClass)
{
    renameEditor = std::make_unique<juce::TextEditor>();
    renameEditor->setSelectAllWhenFocused(true);
    renameEditor->setIndents(2, 1);
    renameEditor->addListener(this);
    addChildComponent(*renameEditor);
}

NameJogLabel::~NameJogLabel() {}

void NameJogLabel::setPrefix(const std::string &p)
{
    if (prefix == p)
        return;
    prefix = p;
    repaint();
}

void NameJogLabel::setLabel(const std::string &n)
{
    if (name == n)
        return;
    name = n;
    repaint();
}

void NameJogLabel::setJogEnabled(bool b)
{
    if (jogEnabled == b)
        return;
    jogEnabled = b;
    repaint();
}

juce::Rectangle<int> NameJogLabel::jogArea() const
{
    return getLocalBounds().reduced(1).withTrimmedLeft(getWidth() - jogWidth - 3);
}

juce::Rectangle<int> NameJogLabel::textArea() const
{
    return getLocalBounds().reduced(1).withTrimmedRight(jogWidth + 4).withTrimmedLeft(4);
}

void NameJogLabel::paint(juce::Graphics &g)
{
    auto b = getLocalBounds().reduced(1).toFloat();
    g.setColour(getColour(Styles::fill));
    g.fillRoundedRectangle(b, 1.5f);

    auto tx = getColour(Styles::labelcolor);
    if (!isEnabled())
        tx = tx.withAlpha(0.5f);

    g.setFont(getFont(Styles::labelfont));
    auto tr = textArea();
    if (!prefix.empty())
    {
        auto pw = (int)std::ceil(juce::TextLayout::getStringWidth(
                      g.getCurrentFont(), juce::String::fromUTF8(prefix.c_str()))) +
                  4;
        g.setColour(tx.withAlpha(0.6f));
        g.drawText(prefix, tr.withWidth(pw), juce::Justification::centredLeft);
        tr = tr.withTrimmedLeft(pw);
    }
    g.setColour(tx);
    g.drawText(name, tr, juce::Justification::centredLeft);

    auto ja = jogArea();
    auto up = ja.withHeight(ja.getHeight() / 2);
    auto dn = ja.withTrimmedTop(ja.getHeight() / 2);
    auto arrow = jogEnabled ? tx : tx.withAlpha(0.3f);
    GlyphPainter::paintGlyph(g, up, GlyphPainter::GlyphType::JOG_UP,
                             (jogEnabled && hoveredJog < 0) ? getColour(Styles::labelcolor_hover)
                                                            : arrow);
    GlyphPainter::paintGlyph(g, dn, GlyphPainter::GlyphType::JOG_DOWN,
                             (jogEnabled && hoveredJog > 0) ? getColour(Styles::labelcolor_hover)
                                                            : arrow);
}

void NameJogLabel::resized()
{
    if (renameEditor)
        renameEditor->setBounds(textArea());
}

void NameJogLabel::mouseDown(const juce::MouseEvent &e)
{
    if (e.mods.isPopupMenu())
    {
        if (onPopupMenu)
            onPopupMenu();
        return;
    }

    if (jogEnabled && jogArea().contains(e.getPosition()))
    {
        auto ja = jogArea();
        auto dir = e.getPosition().getY() < ja.getCentreY() ? -1 : 1;
        if (onJog)
            onJog(dir);
        return;
    }

    if (onBeforeRename)
        onBeforeRename();
    beginRename();
}

void NameJogLabel::mouseMove(const juce::MouseEvent &e)
{
    auto ja = jogArea();
    auto nh = 0;
    if (jogEnabled && ja.contains(e.getPosition()))
        nh = e.getPosition().getY() < ja.getCentreY() ? -1 : 1;
    if (nh != hoveredJog)
    {
        hoveredJog = nh;
        repaint();
    }
}

void NameJogLabel::mouseEnter(const juce::MouseEvent &e) { mouseMove(e); }

void NameJogLabel::mouseExit(const juce::MouseEvent &)
{
    if (hoveredJog != 0)
    {
        hoveredJog = 0;
        repaint();
    }
}

void NameJogLabel::beginRename()
{
    if (!renameEditor)
        return;
    renameEditor->setBounds(textArea());
    renameEditor->setFont(getFont(Styles::labelfont));
    renameEditor->setText(name, juce::dontSendNotification);
    renameEditor->applyFontToAllText(getFont(Styles::labelfont));
    renameEditor->setColour(juce::TextEditor::backgroundColourId, getColour(Styles::fill));
    renameEditor->setColour(juce::TextEditor::textColourId, getColour(Styles::labelcolor));
    renameEditor->setVisible(true);
    renameEditor->grabKeyboardFocus();
    renameEditor->selectAll();
}

void NameJogLabel::commitRename()
{
    if (!renameEditor || !renameEditor->isVisible())
        return;
    renameEditor->setVisible(false);
    auto n = renameEditor->getText().toStdString();
    if (n.empty() || n == name)
        return;
    setLabel(n);
    if (onRename)
        onRename(n);
}

void NameJogLabel::textEditorReturnKeyPressed(juce::TextEditor &) { commitRename(); }

void NameJogLabel::textEditorEscapeKeyPressed(juce::TextEditor &)
{
    renameEditor->setVisible(false);
}

void NameJogLabel::textEditorFocusLost(juce::TextEditor &) { renameEditor->setVisible(false); }
} // namespace sst::jucegui::components
