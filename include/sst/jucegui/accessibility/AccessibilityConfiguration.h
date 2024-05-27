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

#ifndef INCLUDE_SST_JUCEGUI_ACCESSIBILITY_ACCESSIBILITYCONFIGURATION_H
#define INCLUDE_SST_JUCEGUI_ACCESSIBILITY_ACCESSIBILITYCONFIGURATION_H

#include <memory>

namespace sst::jucegui::accessibility
{
struct AccessibilityConfiguration
{
    struct Provider
    {
        bool useKeyboardEdits{true};
    };

    // make this a shared pointer so multiple widges can have the same provider
    std::shared_ptr<Provider> accessibilityConfiguration{std::make_shared<Provider>()};
};
} // namespace sst::jucegui::accessibility

#endif // CONDUIT_ACCESSIBILITYCONFIGURATION_H
