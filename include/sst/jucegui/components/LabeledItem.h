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

#ifndef INCLUDE_SST_JUCEGUI_COMPONENTS_LABELEDITEM_H
#define INCLUDE_SST_JUCEGUI_COMPONENTS_LABELEDITEM_H

#include <memory>
#include "Label.h"

namespace sst::jucegui::components
{
template <typename T> struct Labeled
{
    Labeled() = default;
    ~Labeled() = default;
    template <typename W> Labeled(std::unique_ptr<W> &&inItem) : item{std::move(inItem)} {}
    std::unique_ptr<T> item;
    std::unique_ptr<Label> label;

    void setVisible(bool b)
    {
        item->setVisible(b);
        label->setVisible(b);
    }
};
} // namespace sst::jucegui::components
#endif // SHORTCIRCUITXT_LABELEDITEM_H
