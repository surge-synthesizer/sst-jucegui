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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENT_ADAPTERS_THROWRESCALER_H
#define INCLUDE_SST_JUCEGUI_COMPONENT_ADAPTERS_THROWRESCALER_H
#include <memory>
#include <type_traits>
#include "sst/jucegui/data/Continuous.h"

namespace sst::jucegui::component_adapters
{
template <typename T> struct CubicThrowRescaler : data::Continuous
{
    static_assert(std::is_base_of_v<data::Continuous, T>);
    std::unique_ptr<T> under;
    CubicThrowRescaler(std::unique_ptr<T> u) : under(std::move(u))
    {
        auto cond1 = under->isBipolar() && (under->getMin() == -under->getMax());
        auto cond2 = !under->isBipolar() && under->getMin() == 0;

        assert(cond1 || cond2);

        if (!(cond1 || cond2))
        {
            // do something here
        }
    }

    float scaleFromUnder(float f) const
    {
        // We assume bipolar -max/max or from zero to max. same thing
        auto v01 = f / under->getMax();
        return std::cbrt(v01) * under->getMax();
    }
    float scaleToUnder(float f) const
    {
        auto v01 = f / under->getMax();
        return v01 * v01 * v01 * under->getMax();
    }

    std::string getLabel() const override { return under->getLabel(); }
    float getValue() const override { return scaleFromUnder(under->getValue()); }
    void setValueFromGUI(const float &f) override { under->setValueFromGUI(scaleToUnder(f)); }
    void setValueFromModel(const float &f) override { under->setValueFromModel(scaleToUnder(f)); }
    float getDefaultValue() const override { return scaleFromUnder(under->getDefaultValue()); }
    std::string getValueAsStringFor(float f) const override
    {
        return under->getValueAsStringFor(scaleToUnder(f));
    }
    void setValueAsString(const std::string &s) override { under->setValueAsString(s); }
    float getMin() const override { return under->getMin(); }
    float getMax() const override { return under->getMax(); }
    bool isBipolar() const override { return under->isBipolar(); }
};

} // namespace sst::jucegui::component_adapters
#endif // THROWRESCALER_H
