//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_SETTINGS_H
#define SST_JUCEGUI_SETTINGS_H

#include <memory>

namespace sst::jucegui::style
{
struct Settings
{
    virtual ~Settings() = default;

    virtual bool hideMouse() { return true; }

    typedef std::shared_ptr<Settings> ptr_t;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_SETTINGS_H
