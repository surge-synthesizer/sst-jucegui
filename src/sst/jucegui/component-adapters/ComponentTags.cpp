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

#include <sst/jucegui/component-adapters/ComponentTags.h>

namespace sst::jucegui::component_adapters
{

template <typename CT, typename JT>
std::optional<CT> unpack(juce::Component *c, const juce::Identifier &id)
{
    auto prop = c->getProperties().getVarPointer(id);
    if (prop)
    {
        return static_cast<CT>(static_cast<JT>(*prop));
    }
    return std::nullopt;
}

#define GETSET(name, type, jucetype)                                                               \
    static juce::Identifier id_##name{"sstjg-" #name};                                             \
    void set##name(juce::Component *c, type id)                                                    \
    {                                                                                              \
        c->getProperties().set(id_##name, (jucetype)id);                                           \
    }                                                                                              \
    std::optional<type> get##name(juce::Component *c)                                              \
    {                                                                                              \
        return unpack<type, jucetype>(c, id_##name);                                               \
    }

GETSET(TraversalId, int64_t, juce::int64);
GETSET(FullTraversalId, int64_t, juce::int64);
GETSET(ClapParamId, uint32_t, juce::int64);

} // namespace sst::jucegui::component_adapters