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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENT_ADAPTERS_COMPONENTTAGS_H
#define INCLUDE_SST_JUCEGUI_COMPONENT_ADAPTERS_COMPONENTTAGS_H

#include <juce_gui_basics/juce_gui_basics.h>
#include <optional>

namespace sst::jucegui::component_adapters
{
void setTraversalId(juce::Component *, int64_t);
std::optional<int64_t> getTraversalId(juce::Component *);

void setFullTraversalId(juce::Component *, int64_t);
std::optional<int64_t> getFullTraversalId(juce::Component *);

void setClapParamId(juce::Component *, uint32_t);
std::optional<uint32_t> getClapParamId(juce::Component *);

} // namespace sst::jucegui::component_adapters

#endif // COMPONENTTAGS_H
