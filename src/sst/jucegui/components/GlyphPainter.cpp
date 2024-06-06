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

#include <sst/jucegui/components/GlyphPainter.h>

#include <cmrc/cmrc.hpp>

CMRC_DECLARE(sst::jucegui::resources);

namespace sst::jucegui::components
{
static juce::Rectangle<float> centeredSquareIn(const juce::Rectangle<int> &into)
{
    auto sz = std::min(into.getHeight(), into.getWidth());
    return into.withSizeKeepingCentre(sz, sz).toFloat();
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

static void paintArrowLtoR(juce::Graphics &g, const juce::Rectangle<int> &into, bool includeMul)
{
    auto sq = into.toFloat().reduced(1, 1);
    auto cy = sq.getHeight() / 2.f;
    auto ah = sq.getHeight() / 6.f;

    auto grd = juce::Graphics::ScopedSaveState(g);
    g.addTransform(juce::AffineTransform().translated(sq.getX(), sq.getY()));

    auto wPad = sq.getWidth() * 0.4;

    g.drawLine(includeMul ? wPad : 0, cy, sq.getWidth(), cy);
    g.drawLine(sq.getWidth(), cy, sq.getWidth() - ah, cy - ah);
    g.drawLine(sq.getWidth(), cy, sq.getWidth() - ah, cy + ah);

    if (includeMul)
    {
        g.drawLine(0, cy - wPad * 0.5, wPad, cy + wPad * 0.5);
        g.drawLine(0, cy + wPad * 0.5, wPad, cy - wPad * 0.5);
    }
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

static void paintBigPlusGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto sq = centeredSquareIn(into).reduced(1, 1);
    auto h = sq.getHeight();

    auto grd = juce::Graphics::ScopedSaveState(g);
    g.addTransform(juce::AffineTransform().translated(sq.getX(), sq.getY()));

    g.drawLine(0, h * 0.5, h, h * 0.5, 1.5);
    g.drawLine(h * 0.5, 0, h * 0.5, h, 1.5);
}

static void paintHamburgerGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto sq = centeredSquareIn(into).reduced(1, 1);
    auto h = sq.getHeight();
    auto w = sq.getWidth();

    auto grd = juce::Graphics::ScopedSaveState(g);
    g.addTransform(juce::AffineTransform().translated(sq.getX(), sq.getY()));

    auto rh = juce::Rectangle<float>(2, h * 0.3 - 0.5, w - 4, 1);
    g.fillRoundedRectangle(rh.toFloat(), 1);
    rh = rh.translated(0, h * 0.2);
    g.fillRoundedRectangle(rh.toFloat(), 1);
    rh = rh.translated(0, h * 0.2);
    g.fillRoundedRectangle(rh.toFloat(), 1);
}

static void paintKeyboardGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto sq = centeredSquareIn(into).reduced(1);

    // three keys two black keys so outline the white keys
    for (int i = 0; i <= 3; ++i)
    {
        g.drawLine(sq.getX() + sq.getWidth() * i / 3.f, sq.getY(),
                   sq.getX() + sq.getWidth() * i / 3.f, sq.getBottom());
    }
    g.drawLine(sq.getX(), sq.getBottom(), sq.getRight(), sq.getBottom());

    // then fill the black keys
    auto bkw = sq.getWidth() * 0.2;
    auto bkwh = bkw * 0.5;
    auto bkh = sq.getHeight() * 0.6;
    for (int i = 0; i < 2; ++i)
    {
        auto ps = sq.getX() + sq.getWidth() * (i + 1) / 3.f;
        g.fillRect(ps - bkwh, sq.getY(), bkw, bkh);
    }
}

void paintMonoGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto rad = std::min(into.getWidth(), into.getHeight()) * 0.6;
    auto bx = into.getX() + into.getWidth() * 0.5;
    auto by = into.getY() + (into.getHeight() - rad) * 0.5 + rad;

    for (int i = 0; i < 3; ++i)
    {
        auto p = juce::Path();
        auto rm = 0.3 + i * 0.25;
        p.addCentredArc(bx, by, rad * rm, rad * rm, 0, -juce::MathConstants<float>::pi * 0.25,
                        juce::MathConstants<float>::pi * 0.25, true);
        g.strokePath(p, juce::PathStrokeType(1));
    }
}

void paintFromSvg(juce::Graphics &g, const juce::Rectangle<int> &into, const std::string &path,
                  int32_t baseColor, int overW, int overH, const juce::Colour &to)
{
    std::unordered_map<std::string, std::unique_ptr<juce::Drawable>> dbls;
    auto dbIt = dbls.find(path);
    if (dbIt == dbls.end())
    {
        auto fs = cmrc::sst::jucegui::resources::get_filesystem();
        try
        {
            auto stp = fs.open(path);
            auto svg = std::string(stp.begin(), stp.end());
            auto res = juce::Drawable::createFromImageData(svg.data(), svg.size());
            dbls[path] = std::move(res);
        }
        catch (std::exception &e)
        {
            // oh well
        }
    }

    dbIt = dbls.find(path);
    if (dbIt != dbls.end())
    {
        auto &svgDrawable = dbIt->second;
        if (svgDrawable)
        {
            auto w = overW > 0 ? overW : svgDrawable->getWidth();
            auto rw = into.getWidth();
            auto h = overH > 0 ? overH : svgDrawable->getHeight();
            auto rh = into.getHeight();

            auto sf = std::min(1.0 * rw / w, 1.0 * rh / h);

            svgDrawable->replaceColour(juce::Colour(baseColor), to);
            svgDrawable->draw(
                g, 1.0, juce::AffineTransform().scaled(sf).translated(into.getX(), into.getY()));

            svgDrawable->replaceColour(to, juce::Colour(baseColor));
        }
        else
        {
            g.fillAll(juce::Colours::orchid);
        }
    }
}

void paintStereoGlyph(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto rad = std::min(into.getWidth(), into.getHeight()) * 0.8 * 0.5;
    auto bx = into.getCentreX();
    auto by = into.getCentreY();

    for (int i = 0; i < 3; ++i)
    {
        auto p = juce::Path();
        auto rm = 0.3 + i * 0.25;
        p.addCentredArc(bx, by, rad * rm, rad * rm, 0, -juce::MathConstants<float>::pi * 0.75,
                        -juce::MathConstants<float>::pi * 0.25, true);
        g.strokePath(p, juce::PathStrokeType(1));
        p.addCentredArc(bx, by, rad * rm, rad * rm, 0, juce::MathConstants<float>::pi * 0.25,
                        juce::MathConstants<float>::pi * 0.75, true);
        g.strokePath(p, juce::PathStrokeType(1));
    }
}

void paintPowerLight(juce::Graphics &g, const juce::Rectangle<int> &into)
{
    auto rad = std::min(into.getWidth(), into.getHeight()) * 0.7 * 0.5;
    auto bx = into.getCentreX();
    auto by = into.getCentreY();

    g.fillEllipse(bx - rad, by - rad, rad * 2, rad * 2);
}

void GlyphPainter::paint(juce::Graphics &g)
{
    paintGlyph(g, getLocalBounds(), glyph, getColour(Styles::labelcolor));
};

void GlyphPainter::paintGlyph(juce::Graphics &g, const juce::Rectangle<int> &into,
                              sst::jucegui::components::GlyphPainter::GlyphType glyph,
                              const juce::Colour &as)
{
    g.setColour(as);
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
        paintArrowLtoR(g, into, false);
        return;

    case ARROW_L_TO_R_WITH_MUL:
        paintArrowLtoR(g, into, true);
        return;

    case JOG_UP:
    case JOG_DOWN:
    case JOG_LEFT:
    case JOG_RIGHT:
        paintJog(g, into, glyph);
        return;

    case BIG_PLUS:
        paintBigPlusGlyph(g, into);
        return;

    case HAMBURGER:
        paintHamburgerGlyph(g, into);
        return;

    case METRONOME:
        paintFromSvg(g, into, "res/glyphs/metronome.svg", 0xFFAFAFAF, 24, 24, as);
        return;

    case MUTE:
        paintFromSvg(g, into, "res/glyphs/mute.svg", 0xFFAFAFAF, 1400, 1400, as);
        return;

    case KEYBOARD:
        paintKeyboardGlyph(g, into);
        return;

    case MONO:
        paintMonoGlyph(g, into);
        return;

    case STEREO:
        paintStereoGlyph(g, into);
        return;

    case STEP_COUNT:
        paintFromSvg(g, into, "res/glyphs/step_count.svg", 0xFFAFAFAF, 24, 24, as);
        return;

    case POWER_LIGHT:
        paintPowerLight(g, into);
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