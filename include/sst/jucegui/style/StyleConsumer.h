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
    explicit StyleConsumer(const StyleSheet::Class &c) : styleClass(c){};
    virtual ~StyleConsumer() = default;

    const StyleSheet::Class &getStyleClass() { return styleClass; }

    void setStyle(const StyleSheet::ptr_t &s)
    {
        stylep = s;
        onStyleChanged();

        auto jc = dynamic_cast<juce::Component *>(this);
        if (jc)
        {
            for (auto c : jc->getChildren())
            {
                auto sc = dynamic_cast<StyleConsumer *>(c);
                if (sc)
                {
                    sc->setStyle(s);
                }
            }
        }
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
    const StyleSheet::Class &styleClass;
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLECONSUMER_H
