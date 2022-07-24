/*
 * sst-jucegui - a set of widgets for ShortCircuit, SurgeXT2, and others
 * Copyright 2022 to authors per the github transaction log
 *
 * Released under the MIT license. See 'LICENSE.md' for details.
 */

#ifndef SST_JUCEGUI_SETTINGS_H
#define SST_JUCEGUI_SETTINGS_H

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
