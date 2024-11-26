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
    auto sq = centeredSquareIn(into).reduced(3);

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
        catch (std::exception &)
        {
            g.setColour(juce::Colours::red);
            g.fillRect(into);
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
            g.setColour(juce::Colours::orchid);
            g.fillRect(into);
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

void paintPowerLight(juce::Graphics &g, const juce::Rectangle<int> &into, bool doFill)
{
    auto rad = std::min(into.getWidth(), into.getHeight()) * 0.7 * 0.5;
    auto bx = into.getCentreX();
    auto by = into.getCentreY();

    if (doFill)
    {
        g.fillEllipse(bx - rad, by - rad, rad * 2, rad * 2);
    }
    else
    {
        g.drawEllipse(bx - rad, by - rad, rad * 2, rad * 2, 1);
    }
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

#define SVG24(k, s)                                                                                \
    case k:                                                                                        \
        paintFromSvg(g, into, "res/glyphs/" s ".svg", 0xFFAFAFAF, 24, 24, as);                     \
        return;

        SVG24(METRONOME, "metronome");
        SVG24(PAN, "pan");
        SVG24(VOLUME, "volume");
        SVG24(TUNING, "tuning");

        SVG24(MODULATION_ADDITIVE, "add-mod");
        SVG24(MODULATION_MULTIPLICATIVE, "mul-mod");

        SVG24(JOG_DOWN, "down");
        SVG24(JOG_UP, "up");
        SVG24(JOG_LEFT, "left");
        SVG24(JOG_RIGHT, "right");
        SVG24(ROUTING_POST_FADER, "routing-post-fader");
        SVG24(ROUTING_PRE_FADER, "routing-pre-fader");
        SVG24(ROUTING_PRE_FX, "routing-pre-fx");
        SVG24(STEP_COUNT, "step-count");
        SVG24(SAVE, "save");
        SVG24(FAVORITE, "favorite");
        SVG24(MEMORY, "memory");

        SVG24(SURGE_LOGO, "surge-logo");
        SVG24(SHORTCIRCUIT_LOGO, "shortcircuit-logo");
        SVG24(MIDI, "midi");

        SVG24(FORWARD_BACKWARD, "forward-backward");
        SVG24(UP_DOWN, "up-down");
        SVG24(LEFT_RIGHT, "left-right");
        SVG24(PLUS_MINUS, "plus-minus");
        SVG24(PLUS, "plus");

        SVG24(SEARCH, "search");
        SVG24(SETTINGS, "settings");

        SVG24(LINK, "link");
        SVG24(LOCK, "lock");
        SVG24(PIN, "pin");
        SVG24(UNPIN, "unpin");
        SVG24(POWER, "power");

        SVG24(SPEAKER, "speaker");
        SVG24(REVERSE, "reverse");
        SVG24(POLYPHONY, "polyphony");
        SVG24(PORTAMENTO, "portamento");

        SVG24(CLOSE, "close");
        SVG24(CURVE, "curve");
        SVG24(EDIT, "edit");
        SVG24(FREEZE, "freeze");
        SVG24(ELLIPSIS_H, "ellipsis-h");
        SVG24(ELLIPSIS_V, "ellipsis-v");
        SVG24(NOTE_PRIORITY, "note-priority");
        SVG24(ARROW_L_TO_R, "arrow-ltor");

        SVG24(FOLDER, "folder");
        SVG24(FILE_MUSIC, "file-music");

    case SHOW_INFO:
        paintFromSvg(g, into, "res/glyphs/show-info.svg", 0xFFAFAFAF, 16, 16, as);
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

    case SMALL_POWER_LIGHT:
    case SMALL_POWER_LIGHT_OFF:
        paintPowerLight(g, into, glyph == SMALL_POWER_LIGHT);
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