//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_STYLECONSUMER_H
#define SST_JUCEGUI_STYLECONSUMER_H

#include <string>
#include "StyleSheet.h"

namespace sst::jucegui::style
{
struct StyleConsumer
{
    explicit StyleConsumer(const std::string &className){};
    virtual ~StyleConsumer() = default;

    void setStyleSuperclass(const char *sc);

    void setStyle(const StyleSheet::ptr_t &s)
    {
        stylep = s;
        onStyleChanged();
    }
    inline StyleSheet::ptr_t style()
    {
        if (!stylep)
            stylep = StyleSheet::getBuiltInStyleSheet(StyleSheet::DARK);
        return stylep;
    }
    virtual void onStyleChanged() {}

  private:
    StyleSheet::ptr_t stylep;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLECONSUMER_H
