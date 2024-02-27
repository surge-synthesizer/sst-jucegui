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

#ifndef INCLUDE_SST_JUCEGUI_STYLE_CUSTOMIZATIONTOOLS_H
#define INCLUDE_SST_JUCEGUI_STYLE_CUSTOMIZATIONTOOLS_H

#include "StyleAndSettingsConsumer.h"

namespace sst::jucegui::style
{
struct Traverser
{
    /*
     * Given a component travers its children recursively calling
     * the consumer function for each style consumer. If the function
     * returns true, continue descent into its children. If the
     * from component is a style consumer, call the function on it also.
     */
    void forStyleParticpants(juce::Component *from, std::function<bool(StyleConsumer *)> consumer)
    {
        bool continueDescent{true};
        auto asSC = dynamic_cast<StyleConsumer *>(from);
        if (asSC && consumer)
        {
            continueDescent = consumer(asSC);
        }
        if (continueDescent)
        {
            for (auto &k : from->getChildren())
            {
                forStyleParticpants(k, consumer);
            }
        }
    }
};

struct CustomTypeMap
{
    std::vector<std::function<bool(StyleConsumer *)>> remappers;
    template <typename Under> void addCustomClass(const StyleSheet::Class &cl)
    {
        remappers.push_back([&cl](StyleConsumer *c) -> bool {
            auto *asUnder = dynamic_cast<Under *>(c);
            if (asUnder)
            {
                c->setCustomClass(cl);
                return true;
            }
            return false;
        });
    }

    void applyMapTo(juce::Component *c)
    {
        Traverser t;
        t.forStyleParticpants(c, [this](auto *target) {
            for (auto &m : remappers)
            {
                if (m(target))
                    break;
            }
            return true;
        });
    }
};
} // namespace sst::jucegui::style
#endif // SHORTCIRCUITXT_TRAVERSER_H
