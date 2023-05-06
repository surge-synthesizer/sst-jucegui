/*
 * sst-juce-guil - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023, various authors, as described in the GitHub
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

#include <sst/jucegui/components/GlyphPainter.h>

namespace sst::jucegui::components
{
static juce::Rectangle<float> centeredSquareIn(const juce::Rectangle<int> &into)
{
    if (into.getHeight() == into.getWidth())
        return into.toFloat();

    auto res = into.toFloat();
    if (res.getHeight() > res.getWidth())
    {
        auto sh = (res.getHeight() - res.getWidth()) * 0.5;
        return res.withTrimmedBottom(sh).withTrimmedTop(sh);
    }
    else
    {
        auto sh = (res.getWidth() - res.getHeight()) * 0.5;
        return res.withTrimmedLeft(sh).withTrimmedRight(sh);
    }
    return into.toFloat();
}

static void paintPanGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto sq = centeredSquareIn(into);
    auto h = sq.getHeight() / 4.f;
    auto p = juce::Path();
    p.startNewSubPath(sq.getX(), sq.getY() + 3 * h);
    p.lineTo(sq.getX() + sq.getWidth(), sq.getY() + h);
    p.lineTo(sq.getX() + sq.getWidth(), sq.getY() + 3 * h);
    p.lineTo(sq.getX(), sq.getY() + h);
    p.closeSubPath();
    g.fillPath(p);
}

static void paintVolumeGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto sq = centeredSquareIn(into);
    auto h = sq.getHeight() / 4.f;
    auto p = juce::Path();
    p.startNewSubPath(sq.getX(), sq.getY() + 3 * h);
    p.lineTo(sq.getX() + sq.getWidth(), sq.getY() + 3 * h);
    p.lineTo(sq.getX() + sq.getWidth(), sq.getY() + h);
    p.closeSubPath();
    g.fillPath(p);
}

static void paintTuningGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto sq = centeredSquareIn(into);
    auto h = sq.getHeight() / 4.f;
    auto stag = h / 2.0;
    auto w = sq.getWidth() / 3.25f;

    auto grd = juce::Graphics::ScopedSaveState(g);
    g.addTransform(juce::AffineTransform().translated(sq.getX(), sq.getY()));

    g.drawLine(w, h + stag, w, 3 * h + stag);
    g.fillEllipse(w - h, 3 * h, h, h);
    g.drawLine(2.5 * w, h - stag, 2.5 * w, 3 * h - stag);
    g.fillEllipse(2.5 * w - h, 2 * h, h, h);
}

static void paintCrossGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto sq = centeredSquareIn(into).reduced(1, 1);
    auto h = sq.getHeight();

    auto grd = juce::Graphics::ScopedSaveState(g);
    g.addTransform(juce::AffineTransform().translated(sq.getX(), sq.getY()));

    g.drawLine(0, 0, h, h);
    g.drawLine(0, h, h, 0);
}

static void paintArrowLtoR(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto sq = into.toFloat().reduced(1, 1);
    auto cy = sq.getCentreY();
    auto ah = sq.getHeight() / 6.f;

    auto grd = juce::Graphics::ScopedSaveState(g);
    g.addTransform(juce::AffineTransform().translated(sq.getX(), sq.getY()));

    g.drawLine(0, cy, sq.getWidth(), cy);
    g.drawLine(sq.getWidth(), cy, sq.getWidth() - ah, cy - ah);
    g.drawLine(sq.getWidth(), cy, sq.getWidth() - ah, cy + ah);
}

static void paintJog(juce::Graphics &g, const juce::Rectangle<int> into,
                     GlyphPainter::GlyphType type)
{
    auto sq = centeredSquareIn(into).reduced(1, 1);
    auto h = sq.getHeight();

    auto x13 = sq.getX() + sq.getWidth() / 3.f;
    auto x23 = sq.getX() + 2 * sq.getWidth() / 3.f;

    auto x14 = sq.getX() + sq.getWidth() / 4.f;
    auto xct = sq.getX() + 2 * sq.getWidth() / 4.f;
    auto x34 = sq.getX() + 3 * sq.getWidth() / 4.f;

    auto y13 = sq.getY() + sq.getHeight() / 3.f;
    auto y23 = sq.getY() + 2 * sq.getHeight() / 3.f;

    auto y14 = sq.getY() + sq.getHeight() / 4.f;
    auto yct = sq.getY() + 2 * sq.getHeight() / 4.f;
    auto y34 = sq.getY() + 3 * sq.getHeight() / 4.f;

    auto p = juce::Path();
    switch (type)
    {
    case GlyphPainter::JOG_UP:
    {
        p.startNewSubPath(x14, y23);
        p.lineTo(xct, y13);
        p.lineTo(x34, y23);
        p.closeSubPath();
    }
    break;
    case GlyphPainter::JOG_DOWN:
    {
        p.startNewSubPath(x14, y13);
        p.lineTo(xct, y23);
        p.lineTo(x34, y13);
        p.closeSubPath();
    }
    break;
    case GlyphPainter::JOG_LEFT:
    {
        p.startNewSubPath(x23, y14);
        p.lineTo(x13, yct);
        p.lineTo(x23, y34);
        p.closeSubPath();
    }
    break;
    case GlyphPainter::JOG_RIGHT:
    {
        p.startNewSubPath(x13, y14);
        p.lineTo(x23, yct);
        p.lineTo(x13, y34);
        p.closeSubPath();
    }
    break;
    default:
        // error
        {
            g.setColour(juce::Colours::red);
            g.fillRect(sq);
            return;
        }
        break;
    }
    g.fillPath(p);
}

void GlyphPainter::paint(juce::Graphics &g)
{
    g.setColour(getColour(Styles::controlLabelCol));
    paintGlyph(g, getLocalBounds(), glyph);
};

void GlyphPainter::paintGlyph(juce::Graphics &g, const juce::Rectangle<int> &into,
                              sst::jucegui::components::GlyphPainter::GlyphType glyph)
{
    switch (glyph)
    {
    case PAN:
        paintPanGlyph(g, into);
        return;

    case VOLUME:
        paintVolumeGlyph(g, into);
        return;

    case TUNING:
        paintTuningGlyph(g, into);
        return;

    case CROSS:
        paintCrossGlyph(g, into);
        return;

    case ARROW_L_TO_R:
        paintArrowLtoR(g, into);
        return;

    case JOG_UP:
    case JOG_DOWN:
    case JOG_LEFT:
    case JOG_RIGHT:
        paintJog(g, into, glyph);
        return;

    default:
    {
        auto w = std::min(into.getHeight(), into.getWidth());
        auto s = std::min(4, w - 2);
        for (int i = s; i < w; ++i)
        {
            g.setColour(i % 2 == 0 ? juce::Colours::red : juce::Colours::black);
            g.fillRect(into.reduced(i));
        }
        return;
    }
    }
    g.fillRect(into);
}

} // namespace sst::jucegui::components