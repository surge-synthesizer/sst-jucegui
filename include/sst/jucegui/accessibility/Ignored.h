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

#ifndef INCLUDE_SST_JUCEGUI_ACCESSIBILITY_IGNORED_H
#define INCLUDE_SST_JUCEGUI_ACCESSIBILITY_IGNORED_H

#include <juce_gui_basics/juce_gui_basics.h>

namespace sst::jucegui::accessibility
{
template <typename T> struct IgnoredMixin : public T
{
    std::unique_ptr<juce::AccessibilityHandler> createAccessibilityHandler() override
    {
        return std::make_unique<juce::AccessibilityHandler>(*this,
                                                            juce::AccessibilityRole::ignored);
    }
};

struct IgnoredComponent : IgnoredMixin<juce::Component>
{
    IgnoredComponent() { setAccessible(true); }
};

} // namespace sst::jucegui::accessibility

#endif // CONDUIT_IGNORED_H
