//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_CONTINUOUS_H
#define SST_JUCEGUI_CONTINUOUS_H

#include <utility>
#include <string>
#include <unordered_set>

#include "Labeled.h"

namespace sst::jucegui::data
{
struct Continuous : public Labeled
{
    virtual ~Continuous() = default;

    struct DataListener
    {
        virtual ~DataListener() = default;
        // FIXME - in the future we may want this more fine grained
        virtual void dataChanged() = 0;
    };
    void addGUIDataListener(DataListener *l) { guilisteners.insert(l); }
    void removeGUIDataListener(DataListener *l) { guilisteners.erase(l); }
    void addModelDataListener(DataListener *l) { modellisteners.insert(l); }
    void removeModelDataListener(DataListener *l) { modellisteners.erase(l); }

    virtual float getValue() const = 0;
    virtual void setValueFromGUI(const float &f) = 0;
    virtual void setValueFromModel(const float &f) = 0;

    virtual float getValue01() { return (getValue() - getMin()) / (getMax() - getMin()); }

    virtual std::string getValueAsStringFor(float f) const { return std::to_string(f); }
    virtual std::string getValueAsString() const { return std::to_string(getValue()); }
    virtual void setValueAsString(const std::string &s) { setValueFromGUI(std::atof(s.c_str())); }

    virtual float getMin() const { return 0; }
    virtual float getMax() const { return 1; }
    virtual float getQuantizedStepSize() const { return 0.1; }

    virtual bool isBipolar() const { return getMin() < 0 && getMax() > 0; }

    virtual void jog(int dir)
    {
        setValueFromGUI(std::clamp(getValue() + dir * getQuantizedStepSize(), getMin(), getMax()));
    }

  protected:
    std::unordered_set<DataListener *> guilisteners, modellisteners;
};

struct ContinunousModulatable : public Continuous
{
    virtual float getModulationValuePM1() const = 0;
    virtual void setModulationValuePM1(const float &f) = 0;
    virtual bool isModulationBipolar() = 0;
    virtual float getQuantizedModulationStepSize() const { return 0.1; }
};

} // namespace sst::jucegui::data

#endif // SST_JUCEGUI_CONTINUOUS_H
