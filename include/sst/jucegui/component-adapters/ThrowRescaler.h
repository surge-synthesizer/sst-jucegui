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

#include <cassert>
#include <memory>
#include <type_traits>
#include "sst/jucegui/data/Continuous.h"

namespace sst::jucegui::component_adapters
{
template <typename T>
struct ThrowRescalerBase : data::Continuous, data::WithDataListener<data::Continuous>::DataListener
{
    static_assert(std::is_base_of_v<data::Continuous, T>);

    ThrowRescalerBase(std::unique_ptr<T> u) : underOwned(std::move(u))
    {
        // in this case we don't need to listen since we own under
    }
    ThrowRescalerBase(T *u) : underWeak(std::move(u)) { underWeak->addGUIDataListener(this); }
    virtual ~ThrowRescalerBase()
    {
        if (underWeak)
            underWeak->removeGUIDataListener(this);
    }

    virtual float scaleFromUnder(float f) const = 0;
    virtual float scaleToUnder(float f) const = 0;

    void dataChanged() override {}
    void sourceVanished(data::Continuous *) override
    {
        underWeak = nullptr;
        // TODO: This could then allow this to still operate perhaps?
        // For now we print this message and then assert false if we try
        std::cout << __FILE__ << ":" : << __LINE__
                  << " Unexpected: weak source vanished before rescaler"
                  << std::endl;
    };

    std::string getLabel() const override { return under()->getLabel(); }
    float getValue() const override { return scaleFromUnder(under()->getValue()); }
    void setValueFromGUI(const float &f) override { under()->setValueFromGUI(scaleToUnder(f)); }
    void setValueFromModel(const float &f) override { under()->setValueFromModel(scaleToUnder(f)); }
    float getDefaultValue() const override { return scaleFromUnder(under()->getDefaultValue()); }
    std::string getValueAsStringFor(float f) const override
    {
        return under()->getValueAsStringFor(scaleToUnder(f));
    }
    void setValueAsString(const std::string &s) override { under()->setValueAsString(s); }
    float getMin() const override { return under()->getMin(); }
    float getMax() const override { return under()->getMax(); }
    bool isBipolar() const override { return under()->isBipolar(); }

  protected:
    T *under() const
    {
        if (underOwned)
            return underOwned.get();
        if (underWeak)
            return underWeak;
        assert(false);
        std::cout << __FILE__ << ":" : << __LINE__
                  << " Unexpected: weak source vanished before rescaler"
                  << std::endl;

        return nullptr;
    }

  private:
    std::unique_ptr<T> underOwned;
    T *underWeak{nullptr};
};

template <typename T> struct CubicThrowRescaler : ThrowRescalerBase<T>
{
    // This assumes min = -max or min = 0
    CubicThrowRescaler(std::unique_ptr<T> u) : ThrowRescalerBase<T>(u) { assertSymmetry(); }

    CubicThrowRescaler(T *u) : ThrowRescalerBase<T>(u) { assertSymmetry(); }

    void assertSymmetry()
    {
        auto cond1 =
            this->under()->isBipolar() && (this->under()->getMin() == -this->under()->getMax());
        auto cond2 = !this->under()->isBipolar() && this->under()->getMin() == 0;

        assert(scaleFromUnder(this->under()->getMin()) == this->under()->getMin());
        assert(scaleFromUnder(this->under()->getMax()) == this->under()->getMax());

        assert(cond1 || cond2);

        if (!(cond1 || cond2))
        {
            // do something here
        }
    }

    float scaleFromUnder(float f) const override
    {
        // We assume bipolar -max/max or from zero to max. same thing
        auto v01 = f / this->under()->getMax();
        return std::cbrt(v01) * this->under()->getMax();
    }
    float scaleToUnder(float f) const override
    {
        auto v01 = f / this->under()->getMax();
        return v01 * v01 * v01 * this->under()->getMax();
    }
};

} // namespace sst::jucegui::component_adapters
#endif // THROWRESCALER_H
