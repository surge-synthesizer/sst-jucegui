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

#ifndef INCLUDE_SST_JUCEGUI_DATA_LABELED_H
#define INCLUDE_SST_JUCEGUI_DATA_LABELED_H

#include <string>

namespace sst::jucegui::data
{
struct Labeled
{
    virtual ~Labeled() = default;

    virtual std::string getLabel() const = 0;
    virtual bool isHidden() const { return false; }
};
} // namespace sst::jucegui::data
#endif // SST_JUCEGUI_LABELED_H
