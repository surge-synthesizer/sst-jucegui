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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENT_ADAPTERS_CONTINUOUSTOREFERENCE_H
#define INCLUDE_SST_JUCEGUI_COMPONENT_ADAPTERS_CONTINUOUSTOREFERENCE_H

#include <algorithm>
#include <memory>
#include <type_traits>

#include "sst/jucegui/data/Discrete.h"
#include "sst/jucegui/components/DiscreteParamEditor.h"

namespace sst::jucegui::component_adapters
{
template <typename T> struct ContinuousToValueReference : data::Continuous
{
    std::unique_ptr<T> widget;
    float &underlyer;
    ContinuousToValueReference(std::unique_ptr<T> &wid, float &und)
        : widget(std::move(wid)), underlyer(und)
    {
        setup();
    }
    ContinuousToValueReference(float &und) : widget(std::make_unique<T>()), underlyer(und)
    {
        setup();
    }

    void setup() { widget->setSource(this); }

    std::string label;
    void setLabel(const std::string &s)
    {
        label = s;
        widget->repaint();
    }
    std::string getLabel() const override { return label; }

    std::function<std::string(float val)> valueToString{nullptr};
    std::string getValueAsStringFor(float i) const override
    {
        if (valueToString)
        {
            return valueToString(i);
        }

        return data::Continuous::getValueAsStringFor(i);
    }

    // Range / default are settable so the adapter can front a value with arbitrary bounds.
    float minValue{0.f}, maxValue{1.f}, defaultValue{1.f};
    void setRange(float mn, float mx, float def)
    {
        minValue = mn;
        maxValue = mx;
        defaultValue = def;
    }

    std::function<void(float val)> onValueChanged{nullptr};
    float getValue() const override { return underlyer; }
    float getDefaultValue() const override { return defaultValue; }
    float getMin() const override { return minValue; }
    float getMax() const override { return maxValue; }
    void setValueFromGUI(const float &f) override
    {
        underlyer = std::clamp(f, minValue, maxValue);
        if (onValueChanged)
            onValueChanged(underlyer);
    }
    void setValueFromModel(const float &f) override
    {
        underlyer = f;
        widget->repaint();
    }
};
} // namespace sst::jucegui::component_adapters

#endif // SHORTCIRCUITXT_CONTINUOUSTOREFERENCE_H
