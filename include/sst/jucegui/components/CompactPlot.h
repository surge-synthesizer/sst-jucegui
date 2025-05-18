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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_COMPACTPLOT_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_COMPACTPLOT_H

#include <utility>
#include <vector>

#include "sst/jucegui/style/StyleAndSettingsConsumer.h"
#include "sst/jucegui/style/StyleSheet.h"
#include "BaseStyles.h"

namespace sst::jucegui::components
{
struct CompactPlot : public juce::Component, public style::StyleConsumer
{
    struct Styles
    {
        using sclass = style::StyleSheet::Class;
        using sprop = style::StyleSheet::Property;
        static constexpr sclass styleClass{"compactplot"};

        PROP(plotAxis);
        PROP(plotLine);
        PROP(plotGradStart);
        PROP(plotGradEnd);

        static void initialize()
        {
            style::StyleSheet::addClass(styleClass)
                .withProperty(plotAxis)
                .withProperty(plotLine)
                .withProperty(plotGradStart)
                .withProperty(plotGradEnd);
        }
    };

    CompactPlot() : style::StyleConsumer(Styles::styleClass) {}
    ~CompactPlot() {}

    virtual bool curvePathIsValid() const = 0;

    using point_t = std::pair<float, float>;
    using plotData_t = std::vector<point_t>;
    virtual void recalculateCurvePath(plotData_t &into) = 0;

    using axisBounds_t = std::pair<float, float>;
    virtual axisBounds_t getXAxisBounds() const { return {0, 1}; }
    virtual axisBounds_t getYAxisBounds() const { return {-1, 1}; }
    virtual bool isYAxisFromZero() const { return getYAxisBounds().first == 0; }

    void paint(juce::Graphics &g) override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CompactPlot);

  protected:
    juce::Path curvePath, positiveCurvePath, negativeCurvePath;
    plotData_t plotData;
};
} // namespace sst::jucegui::components
#endif // COMPACTPLOT_H
