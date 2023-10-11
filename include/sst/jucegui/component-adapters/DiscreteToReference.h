/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023, various authors, as described in the GitHub
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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENT_ADAPTERS_DISCRETETOREFERENCE_H
#define INCLUDE_SST_JUCEGUI_COMPONENT_ADAPTERS_DISCRETETOREFERENCE_H

#include <memory>
#include <type_traits>

#include "sst/jucegui/components/DiscreteParamEditor.h"

namespace sst::jucegui::component_adapters
{
template <typename T, typename V> struct DiscreteToValueReference : data::Discrete
{
    static_assert(std::is_integral<V>());
    std::unique_ptr<T> widget;
    V &underlyer;
    static_assert(std::is_base_of<components::DiscreteParamEditor, T>::value);
    DiscreteToValueReference(std::unique_ptr<T> &wid, V &und)
        : widget(std::move(wid)), underlyer(und)
    {
        setup();
    }
    DiscreteToValueReference(V &und) : widget(std::make_unique<T>()), underlyer(und) { setup(); }

    void setup() { widget->setSource(this); }

    std::string label;
    void setLabel(const std::string &s)
    {
        label = s;
        widget->repaint();
    }
    std::string getLabel() const override { return label; }

    std::function<void(V val)> onValueChanged{nullptr};
    int getValue() const override { return underlyer; }
    void setValueFromGUI(const int &f) override
    {
        underlyer = f;
        if (onValueChanged)
            onValueChanged(f);
    }
    void setValueFromModel(const int &f) override
    {
        underlyer = f;
        widget->repaint();
    }
};
} // namespace sst::jucegui::component_adapters

#endif // CONDUIT_DISCRETETOREFERENCE_H
