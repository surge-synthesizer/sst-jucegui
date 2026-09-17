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

int NameJogLabel::prefixWidth()
{
    if (prefix.empty())
        return 0;
    return SST_STRING_WIDTH_INT(getFont(Styles::labelfont),
                                juce::String::fromUTF8(prefix.c_str())) +
           4;
}

juce::Rectangle<int> NameJogLabel::editorArea()
{
    return textArea().withTrimmedLeft(prefixWidth());
}

// filled triangles rather than the jog glyph, which leaves too much air in a
// box this size to read as a control
static void paintJogArrow(juce::Graphics &g, const juce::Rectangle<int> &into, bool up,
                          const juce::Colour &c)
{
    static constexpr float w{11.f}, h{6.f};
    auto cx = into.toFloat().getCentreX();
    auto cy = into.toFloat().getCentreY();

    juce::Path p;
    if (up)
    {
        p.startNewSubPath(cx, cy - h * 0.5f);
        p.lineTo(cx + w * 0.5f, cy + h * 0.5f);
        p.lineTo(cx - w * 0.5f, cy + h * 0.5f);
    }
    else
    {
        p.startNewSubPath(cx, cy + h * 0.5f);
        p.lineTo(cx + w * 0.5f, cy - h * 0.5f);
        p.lineTo(cx - w * 0.5f, cy - h * 0.5f);
    }
    p.closeSubPath();
    g.setColour(c);
    g.fillPath(p);
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
        g.setColour(tx.withAlpha(0.6f));
        g.drawText(prefix, tr.withWidth(prefixWidth()), juce::Justification::centredLeft);
        tr = tr.withTrimmedLeft(prefixWidth());
    }
    if (!renameEditor || !renameEditor->isVisible())
    {
        g.setColour(tx);
        g.drawText(name, tr, juce::Justification::centredLeft);
    }

    auto ja = jogArea();
    auto up = ja.withHeight(ja.getHeight() / 2);
    auto dn = ja.withTrimmedTop(ja.getHeight() / 2);
    auto arrow = jogEnabled ? tx : tx.withAlpha(0.3f);
    auto hover = getColour(Styles::labelcolor_hover);
    paintJogArrow(g, up, true, (jogEnabled && hoveredJog < 0) ? hover : arrow);
    paintJogArrow(g, dn, false, (jogEnabled && hoveredJog > 0) ? hover : arrow);
}

void NameJogLabel::resized()
{
    if (renameEditor)
        renameEditor->setBounds(editorArea());
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
    renameEditor->setBounds(editorArea());
    renameEditor->setFont(getFont(Styles::labelfont));
    renameEditor->setText(name, juce::dontSendNotification);
    renameEditor->applyFontToAllText(getFont(Styles::labelfont));
    renameEditor->setColour(juce::TextEditor::backgroundColourId, getColour(Styles::fill));
    renameEditor->setColour(juce::TextEditor::textColourId, getColour(Styles::labelcolor));
    renameEditor->setVisible(true);
    renameEditor->grabKeyboardFocus();
    renameEditor->selectAll();
    repaint();
}

void NameJogLabel::commitRename()
{
    if (!renameEditor || !renameEditor->isVisible())
        return;
    renameEditor->setVisible(false);
    repaint();
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
    repaint();
}

void NameJogLabel::textEditorFocusLost(juce::TextEditor &)
{
    renameEditor->setVisible(false);
    repaint();
}
} // namespace sst::jucegui::components
