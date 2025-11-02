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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_LINESEGMENT_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_LINESEGMENT_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <string>
#include <sst/jucegui/style/StyleAndSettingsConsumer.h>
#include <sst/jucegui/style/StyleSheet.h>
#include "BaseStyles.h"

namespace sst::jucegui::components
{
struct LineSegment : public juce::Component,
                     public style::StyleConsumer,
                     public style::SettingsConsumer
{
    struct Styles : base_styles::BaseLabel
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"line-segment"};

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withBaseClass(base_styles::BaseLabel::styleClass);
        }
    };

    LineSegment() : style::StyleConsumer(Styles::styleClass)
    {
        setAccessible(false);
        setInterceptsMouseClicks(false, false);
    };
    ~LineSegment() = default;

    int cx0, cy0, cx1, cy1;
    void setEndpointsAndBounds(int ix0, int iy0, int ix1, int iy1)
    {
        cx0 = ix0;
        cy0 = iy0;
        cx1 = ix1;
        cy1 = iy1;
        if (ix0 > ix1)
            std::swap(ix0, ix1);
        if (iy0 > iy1)
            std::swap(iy0, iy1);
        cx0 -= ix0;
        cx1 -= ix0; // yup. recentering to 0,0
        cy0 -= iy0;
        cy1 -= iy0;
        setBounds(ix0, iy0, std::max(ix1 - ix0, 1), std::max(iy1 - iy0, 1));
    }

    enum LineMode
    {
        LINEAR,
        BEZIER_LR,
        BEZIER_UD
    } mode{BEZIER_LR};

    void paint(juce::Graphics &g) override
    {
        g.setColour(getColour(Styles::labelcolor));
        if (!isEnabled())
            g.setColour(getColour(Styles::labelcolor).withAlpha(0.5f));
        switch (mode)
        {
        case LINEAR:
        case BEZIER_UD:
            g.drawLine(cx0, cy0, cx1, cy1, 1);
        case BEZIER_LR:
            drawLRBezier(g);
        }
    }

    void drawLRBezier(juce::Graphics &g)
    {
        auto x1 = cx0;
        auto y1 = cy0;
        auto x4 = cx1;
        auto y4 = cy1;

        float horizontalDistance = std::abs(x4 - x1);
        float controlPointOffset = std::min(horizontalDistance * 0.5f, 100.0f);

        float x2 = x1 + controlPointOffset;
        float y2 = y1;

        float x3 = x4 - controlPointOffset;
        float y3 = y4;

        juce::Path path;
        path.startNewSubPath(x1, y1);
        path.cubicTo(x2, y2, x3, y3, x4, y4);

        g.strokePath(path, juce::PathStrokeType(1.0f));
    }
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LineSegment)
};
} // namespace sst::jucegui::components

#endif // SST_JUCEGUI_LABEL_H
