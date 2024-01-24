/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023-2024, various authors, as described in the GitHub
 * transaction log.
 *
 * sst-basic-blocks is released under the MIT license, as described
 * by "LICENSE.md" in this repository. This means you may use this
 * in commercial software if you are a JUCE Licensee. If you use JUCE
 * in the open source / GPL3 context, your combined work must be
 * released under GPL3.
 *
 * All source in sst-juce-gui available at
 * https://github.com/surge-synthesizer/sst-juce-gui
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

    auto ol = getColour(Styles::brightoutline);
    auto tx = getColour(Styles::labelcolor);
    auto ar = tx;
    if (isHovered)
    {
        tx = getColour(Styles::labelcolor_hover);
        ar = getColour(Styles::jogbutton_hover);
    }

    g.setColour(ol);
    g.drawRoundedRectangle(b, rectCorner, 1);

    if (!data)
        return;

    g.setFont(getFont(Styles::labelfont));
    g.setColour(tx);
    g.drawText(data->getValueAsString(), b, juce::Justification::centred);

    g.setColour(ar);
    auto bl = b.withWidth(b.getHeight()).toNearestInt();
    GlyphPainter::paintGlyph(g, bl, GlyphPainter::GlyphType::JOG_LEFT);

    bl = b.withLeft(b.getRight() - b.getHeight()).toNearestInt();
    GlyphPainter::paintGlyph(g, bl, GlyphPainter::GlyphType::JOG_RIGHT);
}

void JogUpDownButton::mouseDown(const juce::MouseEvent &e)
{
    if (onPopupMenu)
    {
        onPopupMenu();
        return;
    }
    DiscreteParamEditor::mouseDown(e);
}

void JogUpDownButton::mouseUp(const juce::MouseEvent &e)
{
    if (data)
    {
        if (e.position.x < getHeight())
            data->jog(-1);
        if (e.position.x > getWidth() - getHeight())
            data->jog(1);
    }
}
} // namespace sst::jucegui::components