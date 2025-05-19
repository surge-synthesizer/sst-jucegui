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

#ifndef INCLUDE_SST_JUCEGUI_DATA_CONTINUOUS_H
#define INCLUDE_SST_JUCEGUI_DATA_CONTINUOUS_H

#include <utility>
#include <string>
#include <unordered_set>
#include <variant>
#include <cassert>

#include "Labeled.h"
#include "WithDataListener.h"

namespace sst::jucegui::data
{
struct Continuous : public Labeled, WithDataListener<Continuous>
{
    virtual ~Continuous()
    {
        supressListenerModification = true;
        for (auto *dl : guilisteners)
        {
            dl->sourceVanished(this);
        }
        supressListenerModification = false;
    };

    virtual float getValue() const = 0;
    virtual void setValueFromGUI(const float &f) = 0;
    virtual void setValueFromGUIQuantized(const float &f) { setValueFromGUI(f); }
    virtual void setValueFromModel(const float &f) = 0;
    virtual float getDefaultValue() const = 0;
    virtual float getValue01() { return (getValue() - getMin()) / (getMax() - getMin()); }

    virtual std::string getValueAsStringFor(float f) const { return std::to_string(f); }
    virtual std::string getValueAsString() const { return getValueAsStringFor(getValue()); }

    // No-units implementation defaults to the regular implementation
    virtual std::string getValueAsStringWithoutUnitsFor(float f) const
    {
        return getValueAsStringFor(f);
    }
    virtual std::string getValueAsStringWithoutUnits() const
    {
        return getValueAsStringWithoutUnitsFor(getValue());
    }

    virtual void setValueAsString(const std::string &s)
    {
        setValueFromGUI(std::clamp((float)std::atof(s.c_str()), getMin(), getMax()));
    }

    virtual float getMin() const { return 0; }
    virtual float getMax() const { return 1; }
    virtual float getMinMaxRange() const { return getMax() - getMin(); }
    virtual float getQuantizedStepSize() const { return getMinMaxRange() / 10.f; }

    virtual bool isBipolar() const { return getMin() < 0 && getMax() > 0; }

    virtual float quantizeValue(float val)
    {
        auto qs = (getMax() - getMin()) / 10.f;
        float qval = std::clamp(qs * (int)std::round(val / qs), getMin(), getMax());
        return qval;
    }
};

struct ContinuousModulatable : public Continuous
{
    virtual float getModulationValuePM1() const = 0;
    virtual void setModulationValuePM1(const float &f) = 0;
    virtual bool isModulationBipolar() const = 0;
    virtual float getQuantizedModulationStepSize() const { return 0.1; }
};

} // namespace sst::jucegui::data

#endif // SST_JUCEGUI_CONTINUOUS_H
