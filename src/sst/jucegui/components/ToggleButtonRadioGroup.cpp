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

#include "sst/jucegui/components/ToggleButton.h"
#include "sst/jucegui/data/Discrete.h"
#include <sst/jucegui/components/ToggleButtonRadioGroup.h>

#include <cassert>

namespace sst::jucegui::components
{
struct subordinateDiscrete : data::Discrete
{
    data::Discrete *under{nullptr};
    int idx{0};
    subordinateDiscrete(data::Discrete *d, int i) : under(d), idx(i)
    {
        assert(under && idx >= under->getMin() && idx <= under->getMax());
    }
    virtual ~subordinateDiscrete() = default;

    std::string getLabel() const override { return "unused"; }
    int getValue() const override { return under->getValue() == idx ? 1 : 0; }

    void setValueFromGUI(const int &f) override
    {
        if (f)
        {
            under->setValueFromGUI(idx);
        }
    }
    void setValueFromModel(const int &f) override {}
};

ToggleButtonRadioGroup::ToggleButtonRadioGroup() {}

ToggleButtonRadioGroup::~ToggleButtonRadioGroup()
{
    if (data)
        data->removeGUIDataListener(this);

    for (const auto &bt : buttons)
        bt->setSource(nullptr);
}

void ToggleButtonRadioGroup::resized()
{
    if (buttons.empty())
        return;
    auto nb = buttons.size();
    auto npad = nb - 1;
    auto margin = 2;
    auto tw = getWidth() - npad * margin;
    auto b = tw * 1.f / nb;
    auto r = getLocalBounds().withWidth(b);
    for (const auto &bt : buttons)
    {
        bt->setBounds(r);
        r = r.translated(b + margin, 0);
    }
}

void ToggleButtonRadioGroup::dataChanged()
{
    auto nBut = data ? (data->getMax() - data->getMin() + 1) : 0;

    if (!data || nBut != buttons.size())
    {
        for (const auto &b : buttons)
            removeChildComponent(b.get());
        buttons.clear();
        buttonSubData.clear();
        if (!data)
            return;

        for (int i = 0; i < nBut; ++i)
        {
            auto b = std::make_unique<ToggleButton>();
            b->setLabel(data->getValueAsStringFor(i));
            addAndMakeVisible(*b);

            auto sd = std::make_unique<subordinateDiscrete>(data, i);
            b->setSource(sd.get());

            buttons.push_back(std::move(b));
            buttonSubData.push_back(std::move(sd));
        }
        if (isShowing())
            resized();
    }
    else
    {
        for (const auto &b : buttons)
            b->dataChanged();
    }
}
} // namespace sst::jucegui::components