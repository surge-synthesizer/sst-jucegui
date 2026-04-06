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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_TICKSEPARATORLABEL_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_TICKSEPARATORLABEL_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include "BaseStyles.h"

namespace sst::jucegui::components
{
/**
 * RuleLabel draws a rule line (or corner) within its bounds, using the label
 * color from the style sheet. Useful for visually grouping related controls.
 *
 * Direction determines what is drawn:
 *   HORIZONTAL  — a single horizontal line through the vertical centre of the bounds
 *   VERTICAL    — a single vertical line through the horizontal centre of the bounds
 *   CORNER_TL   — bend at top-left:  horizontal arm extends right along top,
 *                                    vertical arm extends down along left  (┌)
 *   CORNER_TR   — bend at top-right: horizontal arm extends left along top,
 *                                    vertical arm extends down along right (┐)
 *   CORNER_BL   — bend at bot-left:  horizontal arm extends right along bottom,
 *                                    vertical arm extends up along left    (└)
 *   CORNER_BR   — bend at bot-right: horizontal arm extends left along bottom,
 *                                    vertical arm extends up along right   (┘)
 *
 * Set the component bounds to control where the line/corner appears. The
 * component itself is non-interactive and transparent (no background fill).
 */
struct TickSeparatorLabel : public juce::Component,
                            public style::StyleConsumer,
                            public style::SettingsConsumer
{
    enum Direction
    {
        HORIZONTAL,
        VERTICAL,
        CORNER_TL,
        CORNER_TR,
        CORNER_BL,
        CORNER_BR,
    };

    struct Styles : base_styles::BaseLabel
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"rule-label"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass);
        }
    };

    Direction direction{HORIZONTAL};

    TickSeparatorLabel() : style::StyleConsumer(Styles::styleClass)
    {
        setAccessible(false);
        setInterceptsMouseClicks(false, false);
    }

    void setDirection(Direction d)
    {
        direction = d;
        repaint();
    }

    void paint(juce::Graphics &g) override
    {
        g.setColour(getColour(Styles::labelcolor));

        auto pad = 1;
        if (direction == HORIZONTAL || direction == VERTICAL)
            pad = 0;

        auto b = getLocalBounds().toFloat();
        float x0 = b.getX() + pad;
        float y0 = b.getY() + pad;
        float x1 = b.getRight() - pad;
        float y1 = b.getBottom() - pad;
        float cx = b.getCentreX();
        float cy = b.getCentreY();

        switch (direction)
        {
        case HORIZONTAL:
            g.drawLine(x0, cy, x1, cy, 1.0f);
            break;
        case VERTICAL:
            g.drawLine(cx, y0, cx, y1, 1.0f);
            break;
        case CORNER_TL:
            g.drawLine(x0, y0, x1, y0, 1.0f); // horizontal along top
            g.drawLine(x0, y0, x0, y1, 1.0f); // vertical along left
            break;
        case CORNER_TR:
            g.drawLine(x0, y0, x1, y0, 1.0f); // horizontal along top
            g.drawLine(x1, y0, x1, y1, 1.0f); // vertical along right
            break;
        case CORNER_BL:
            g.drawLine(x0, y1, x1, y1, 1.0f); // horizontal along bottom
            g.drawLine(x0, y0, x0, y1, 1.0f); // vertical along left
            break;
        case CORNER_BR:
            g.drawLine(x0, y1, x1, y1, 1.0f); // horizontal along bottom
            g.drawLine(x1, y0, x1, y1, 1.0f); // vertical along right
            break;
        }
    }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TickSeparatorLabel)
};
} // namespace sst::jucegui::components

#endif // INCLUDE_SST_JUCEGUI_COMPONENTS_RULELABEL_H_SCORED
