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

#ifndef INCLUDE_SST_JUCEGUI_DATA_COMPACTPLOTSOURCE_H
#define INCLUDE_SST_JUCEGUI_DATA_COMPACTPLOTSOURCE_H

#include <utility>

#include "WithDataListener.h"
namespace sst::jucegui::data
{
struct CompactPlotSource : public WithDataListener<CompactPlotSource>
{
    virtual ~CompactPlotSource()
    {
        supressListenerModification = true;
        for (auto *dl : guilisteners)
        {
            dl->sourceVanished(this);
        }
        supressListenerModification = false;
    };

    virtual bool curvePathIsValid() const = 0;

    using point_t = std::pair<float, float>;
    using plotData_t = std::vector<point_t>;
    virtual void recalculateCurvePath(plotData_t &into) = 0;

    virtual size_t getSecondaryCurveCount() const { return 0; }
    virtual void recaculateSecondaryCurvePath(size_t index, plotData_t &into) {}

    using axisBounds_t = std::pair<float, float>;
    virtual axisBounds_t getXAxisBounds() const { return {0, 1}; }
    virtual axisBounds_t getYAxisBounds() const { return {-1, 1}; }
    virtual bool isYAxisFromZero() const { return getYAxisBounds().first == 0; }
};
} // namespace sst::jucegui::data
#endif // COMPACTPLOT_H
