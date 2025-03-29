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

#ifndef INCLUDE_SST_JUCEGUI_UTIL_VISIBILITYPARENTWATCHER_H
#define INCLUDE_SST_JUCEGUI_UTIL_VISIBILITYPARENTWATCHER_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace sst::jucegui::util
{
/*
 * If you create a member of this class, then you get
 * visibilityChanged called *not* just when your visibility
 * changes but when any of your parent hierarchy visibility
 * changes
 */
struct VisibilityParentWatcher : juce::ComponentMovementWatcher
{
    VisibilityParentWatcher(juce::Component *that) : juce::ComponentMovementWatcher(that) {}
    void componentMovedOrResized(bool wasMoved, bool wasResized) override {}
    void componentPeerChanged() override {}
    void componentVisibilityChanged() override { getComponent()->visibilityChanged(); }
};
} // namespace sst::jucegui::util
#endif // VISIBILITYPARENTWATCHER_H
