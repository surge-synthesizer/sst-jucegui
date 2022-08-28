//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_LABELED_H
#define SST_JUCEGUI_LABELED_H

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
