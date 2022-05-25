//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_CONTINUOUS_H
#define SST_JUCEGUI_CONTINUOUS_H

#include <utility>
#include <string>

#include "Labeled.h"

namespace sst::jucegui::data
{
struct Continuous : public Labeled
{
    virtual ~Continuous() = default;

    virtual float getValue() const = 0;
    virtual void setValue(const float &f) = 0;

    virtual std::string getValueAsString() const { return std::to_string(getValue()); }
    virtual void setValueAsString(const std::string &s) { setValue(std::atof(s.c_str())); }

    virtual float getMin() const { return 0; }
    virtual float getMax() const { return 1; }
    virtual float getQuantizedStepSize() const { return 0.1; }

    virtual bool isBipolar() const { return getMin() < 0 && getMax() > 0; }

    virtual void jog(int dir)
    {
        setValue(std::clamp(getValue() + dir * getQuantizedStepSize(), getMin(), getMax()));
    }
};

struct ContinunousModulatable : public Continuous
{
    virtual float getModulationValuePM1() const = 0;
    virtual void setModulationValuePM1(const float &f) = 0;
    virtual float getQuantizedModulationStepSize() const { return 0.1; }
};

} // namespace sst::jucegui::data

#endif // SST_JUCEGUI_CONTINUOUS_H
