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

#ifndef INCLUDE_SST_JUCEGUI_DATA_CONTINUOUS_H
#define INCLUDE_SST_JUCEGUI_DATA_CONTINUOUS_H

#include <utility>
#include <string>
#include <unordered_set>
#include <variant>
#include <cassert>

#include "Labeled.h"

namespace sst::jucegui::data
{
struct Continuous : public Labeled
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

    struct DataListener
    {
        virtual ~DataListener() = default;
        // FIXME - in the future we may want this more fine grained
        virtual void dataChanged() = 0;
        virtual void sourceVanished(Continuous *) = 0;
    };
    bool supressListenerModification{false};
    void addGUIDataListener(DataListener *l)
    {
        assert(!supressListenerModification);
        if (!supressListenerModification)
            guilisteners.insert(l);
    }
    void removeGUIDataListener(DataListener *l)
    {
        if (!supressListenerModification)
            guilisteners.erase(l);
    }
    void addModelDataListener(DataListener *l) { modellisteners.insert(l); }
    void removeModelDataListener(DataListener *l) { modellisteners.erase(l); }

    virtual float getValue() const = 0;
    virtual void setValueFromGUI(const float &f) = 0;
    virtual void setValueFromModel(const float &f) = 0;
    virtual float getDefaultValue() const = 0;
    virtual float getValue01() { return (getValue() - getMin()) / (getMax() - getMin()); }

    virtual std::string getValueAsStringFor(float f) const { return std::to_string(f); }
    virtual std::string getValueAsString() const { return getValueAsStringFor(getValue()); }
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

  protected:
    std::unordered_set<DataListener *> guilisteners, modellisteners;
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
