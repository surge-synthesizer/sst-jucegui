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

#include "sst/jucegui/components/DiscreteParamMenuBuilder.h"
#include <cassert>

namespace sst::jucegui::components
{

void DiscreteParamMenuBuilder::populateLinearMenu(juce::PopupMenu &p, juce::Component *c)
{
    auto v = (int)std::round(data->getValue());
    for (auto i = data->getMin(); i <= data->getMax(); ++i)
    {
        auto l = data->getValueAsStringFor(i);
        p.addItem(juce::CharPointer_UTF8(l.c_str()), true, i == v,
                  [i, w = juce::Component::SafePointer(c), d = data]() {
                      if (!w)
                          return;
                      d->setValueFromGUI(i);
                      w->repaint();
                  });
    }
}

void DiscreteParamMenuBuilder::populateGroupListMenu(juce::PopupMenu &main, juce::Component *c)
{
    juce::PopupMenu subMenu;
    std::string currGrp;

    auto v = (int)std::round(data->getValue());

    bool checkSub{false};
    for (const auto &[id, gn] : groupList)
    {
        if (id == v)
            checkSub = true;
        if (gn != currGrp)
        {
            if (!currGrp.empty())
            {
                main.addSubMenu(currGrp, subMenu, true, nullptr, checkSub, 0);
                checkSub = false;
                subMenu = juce::PopupMenu();
            }
            currGrp = gn;
        }
        auto tgt = &subMenu;
        if (gn.empty())
        {
            tgt = &main;
            checkSub = false;
        }
        auto l = data->getValueAsStringFor(id);

        tgt->addItem(juce::CharPointer_UTF8(l.c_str()), true, id == v,
                     [idv = id, w = juce::Component::SafePointer(c), this]() {
                         if (!w)
                             return;
                         data->setValueFromGUI(idv);
                         w->repaint();
                     });
    }
    main.addSubMenu(currGrp, subMenu, true, nullptr, checkSub, 0);
}
void DiscreteParamMenuBuilder::showMenu(juce::Component *c)
{
    auto p = juce::PopupMenu();

    if (!data)
    {
        p.addSectionHeader("ERROR: No discrete data");
        p.showMenuAsync(createMenuOptions());
        return;
    }

    p.addSectionHeader(data->getLabel());
    p.addSeparator();

    switch (mode)
    {
    case Mode::LINEAR:
        populateLinearMenu(p, c);
        break;
    case Mode::GROUP_LIST:
        populateGroupListMenu(p, c);
        break;
    }

    p.showMenuAsync(createMenuOptions());
}

int DiscreteParamMenuBuilder::jogFromValue(int fromThis, int jog)
{
    assert(data);
    switch (mode)
    {
    case Mode::LINEAR:
    {
        fromThis = fromThis + jog;
        if (fromThis < data->getMin())
            fromThis = data->getMax();
        else if (fromThis > data->getMax())
            fromThis = data->getMin();
        return fromThis;
    }
    break;
    case Mode::GROUP_LIST:
    {
        // this is forward jog
        bool returnNext = false;
        int previous = groupList.back().first;
        for (const auto &[id, gn] : groupList)
        {
            if (returnNext && jog > 0)
                return id;
            if (id == data->getValue())
            {
                if (jog < 0)
                    return previous;
                returnNext = true;
            }
            previous = id;
        }
        // If we are here that means we matched the start or end
        if (jog < 0)
            return data->getMax();
        if (jog > 0)
            return data->getMin();
    }
    }
    return fromThis;
}
} // namespace sst::jucegui::components