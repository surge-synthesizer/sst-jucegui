//
// Created by Paul Walker on 5/25/22.
//

#ifndef SST_JUCEGUI_STYLESHEET_H
#define SST_JUCEGUI_STYLESHEET_H

#include <memory>
#include <juce_gui_basics/juce_gui_basics.h>

namespace sst::jucegui::style
{
struct StyleConsumer;
struct StyleSheet
{
    struct Property
    {
        static constexpr int nameLength = 256;
        char pname[nameLength];
        constexpr Property(const char *s) : pname{0}
        {
            for (int i = 0; i < nameLength - 1; ++i)
            {
                pname[i] = s[i];
                if (pname[i] == 0)
                    break;
            }
            pname[nameLength - 1] = 0;
        }
    };

    virtual juce::Colour getColour(StyleConsumer *c, const Property &p) = 0;

    enum BuiltInTypes
    {
        DARK,
        LIGHT,
        HIGH_CONTRAST,
    };
    typedef std::shared_ptr<StyleSheet> ptr_t;

    static ptr_t getBuiltInStyleSheet(const BuiltInTypes &t);
};
} // namespace sst::jucegui::style

#endif // SST_JUCEGUI_STYLESHEET_H
