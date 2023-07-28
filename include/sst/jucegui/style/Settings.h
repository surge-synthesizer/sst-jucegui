/*
 * sst-juce-gui - an open source library of juce widgets
 * built by Surge Synth Team.
 *
 * Copyright 2023, various authors, as described in the GitHub
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

#ifndef INCLUDE_SST_JUCEGUI_STYLE_SETTINGS_H
#define INCLUDE_SST_JUCEGUI_STYLE_SETTINGS_H

#include <memory>

namespace sst::jucegui::style
{

/**
 * The Settings object contains a set of methods to answer user preferences in the
 * UI. To consume this, you should implement `style::SettingsConsumer`
 */
struct Settings
{
    virtual ~Settings() = default;

    virtual bool hideMouse() { return true; }

    typedef std::shared_ptr<Settings> ptr_t;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_SETTINGS_H
